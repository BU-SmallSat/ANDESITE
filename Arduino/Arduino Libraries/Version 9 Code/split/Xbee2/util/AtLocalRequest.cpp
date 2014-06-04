#include "Arduino.h"
#include "AtLocalRequest.h"

void AtLocalRequest::makePacket(byte atPacket[], int frameId, char atCommand[])
{
	byte packet[] = {Shared().getStart(), MSB_LENGTH, LSB_LENGTH, FRAME_TYPE, frameId, 0, 0, 0};
	packet[5] = atCommand[0];
	packet[6] = atCommand[1];
	packet[7] = Shared().getChecksum(packet, sizeof(packet));

	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}

void AtLocalRequest::makePacket(byte atPacket[], int frameId, char atCommand[], char parameters[], int parameterLength)
{
	byte packetFrame[] = {Shared().getStart(), MSB_LENGTH, 0, FRAME_TYPE, frameId, 0x0, 0x0};
	packetFrame[5] = atCommand[0];
	packetFrame[6] = atCommand[1];
	
	//transfer everything from packetFrame to packet
	int frameLength = sizeof(packetFrame);
	int packetLength = frameLength + parameterLength + 1;
	byte packet[packetLength];
	for(int i = 0; i < frameLength; i++)
	{
		packet[i] = packetFrame[i];
	}
	
	//put the parameters in the packet
	for(int i = 7; i < packetLength - 1; i++)
	{
		packet[i] = parameters[i - 7];
	}
	
	//calculate the length and the checksum
	packet[Shared().getLSBLengthIndex()] = Shared().getLSB(packetLength);
	packet[packetLength - 1] = Shared().getChecksum(packet, packetLength);
	
	for(int i = 0; i < packetLength; i++) {
		atPacket[i] = packet[i];
	}
}