#include "Arduino.h"
#include "TX.h"

//Sends TX commant packet
void TX::txCommandPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameID,
	int dataType, int dataId, byte data[], int dataLength)
{
	byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, dataType, dataId}; //packet frame, the 0s are adresses to be added later

	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xBeeSH, sh, shLength);
	Shared().breakUp(xBeeSL, sl, slLength);

	for(int i = 0; i < shLength; i++) {
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	//creates packet to be sent
	int frameLength = sizeof(packetFrame);
	int transmitPacketLength = frameLength + dataLength + 1;
	byte transmitPacket[transmitPacketLength];

	//Fills frame in packetToSend
	int i = 0;
	for(; i < frameLength; i++) {
		transmitPacket[i] = packetFrame[i];
	}

	int start = i;
	//puts data in packetToSend
	if(data) {
		for(; i < transmitPacketLength - 1; i++) {
			transmitPacket[i] = data[i - start];
		}
	}

	//puts msb, lsb, and checksum into frame
	transmitPacket[Shared().getMSBIndex()] = Shared().getMSB(sizeof(transmitPacket));
	transmitPacket[Shared().getLSBIndex()] = Shared().getLSB(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = Shared().getChecksum(transmitPacket, transmitPacketLength);

	for(int j = 0; j < transmitPacketLength; j++) {
		txPacket[j] = transmitPacket[j];
	}
}

//Sends TX data packet
void TX::txDataPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId, 
	String timeStamp, int value)
{
	byte frameID = 0x0;
	int dataLength = 2, timeStampLength = 4;
	byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, SENSOR_READING, dataId}; //packet frame, the 0s are adresses to be added later
	
	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xBeeSH, sh, shLength);
	Shared().breakUp(xBeeSL, sl, slLength);

	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	//creates packet to be sent
	int frameLength = sizeof(packetFrame);
	int transmitPacketLength = frameLength + dataLength + timeStampLength + 1;
	byte transmitPacket[transmitPacketLength];

	//Fills frame in packetToSend
	int index = 0;
	for(; index < frameLength; index++) {
		transmitPacket[index] = packetFrame[index];
	}

	int start = index;

	//Puts value and timeStamp into packetToSend
	byte byteValue[dataLength];
	Shared().breakUp(value, byteValue, dataLength);
	for(; index < start + dataLength; index++) {
		transmitPacket[index] = byteValue[index - start];
	}

	String temp;
	byte byteTimeStamp[timeStampLength];
	for(int i = 0; i < timeStampLength; i++) {
		temp = timeStamp.substring(2 * i, 2 * (i + 1));
		byteTimeStamp[i] = (byte)(temp.toInt() & 0xFF);
	}

	start = index;

	for(; index < timeStampLength + start; index++) {
		transmitPacket[index] = byteTimeStamp[index - start];
	}

	//puts msb, lsb, and checksum into frame
	transmitPacket[Shared().getMSBIndex()] = Shared().getMSB(transmitPacketLength);
	transmitPacket[Shared().getLSBIndex()] = Shared().getLSB(transmitPacketLength);
	transmitPacket[transmitPacketLength - 1] = Shared().getChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		txPacket[i] = transmitPacket[i];
	}
}