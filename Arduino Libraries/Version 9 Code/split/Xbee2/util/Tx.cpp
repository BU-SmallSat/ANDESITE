#include "Arduino.h"
#include "TX.h"

void TX::txCommandPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
	int dataType, int dataId, byte data[], int dataLength)
{
		byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				broadcastRadius, options, dataType, dataId};
	
	//split up and put 64-bit address in
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

	transmitPacket[Shared().getLSBIndex()] = Shared().getLSB(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = Shared().getChecksum(transmitPacket, transmitPacketLength);

	for(int j = 0; j < transmitPacketLength; j++) {
		txPacket[j] = transmitPacket[j];
	}
}

void TX::txDataPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId, 
	String timeStamp, int value)
{
	int dataLength = 2, timeStampLength = 6;
	byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, SENSOR_READING, dataId};
	
	//split up and put 64-bit address in
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

	transmitPacket[Shared().getLSBIndex()] = Shared().getLSB(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = Shared().getChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		txPacket[i] = transmitPacket[i];
	}
}