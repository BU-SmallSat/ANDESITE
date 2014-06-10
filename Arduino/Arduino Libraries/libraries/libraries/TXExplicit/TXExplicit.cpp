#include "Arduino.h"
#include "TxExplicit.h"

void TxExplicit::makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy,
	int clusterId, int transactionNumber, int startIndex)
{
	int frameId = 0x1;
	byte packetFrame[] = {Shared().getStart(), LENGTH_MSB, LENGTH_LSB, TX_EX_FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		SOURCE_ENDPOINT, DEST_ENDPOINT, 0, 0, PROFILE_ID_0, PROFILE_ID_1, BROADCAST_RADIUS, OPTIONS,
		transactionNumber, startIndex, CHECKSUM}; //packet frame, the 0s are adresses to be added later
	int packetLength = sizeof(packetFrame);

	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xbeeSh, sh, shLength);
	Shared().breakUp(xbeeSl, sl, slLength);

	for(int i = 0; i < shLength; i++) {
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xbeeMy, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	int clusterLength = 2;
	byte cluster[clusterLength];
	Shared().breakUp(clusterId, cluster, clusterLength);
	for(int i = 0; i < clusterLength; i++) {
		packetFrame[i + 17] = cluster[i];
	}

	//puts msb, lsb, and checksum into frame
	packetFrame[Shared().getLengthMsbIndex()] = Shared().getLengthMsb(packetLength);
	packetFrame[Shared().getLengthLsbIndex()] = Shared().getLengthLsb(packetLength);
	packetFrame[packetLength - 1] = Shared().calcChecksum(packetFrame, packetLength);

	//fills the frame into the packet
	for(int i = 0; i < packetLength; i++) {
		packet[i] = packetFrame[i];
	}
}

void TxExplicit::makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy, int run, 
	int dataId, int value, String timeStamp)
{
	byte frameId = 0x0;
	int dataLength = 2, timeStampLength = 4;
	byte packetFrame[] = {Shared().getStart(), LENGTH_MSB, LENGTH_LSB, TX_REG_FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		BROADCAST_RADIUS, OPTIONS, SENSOR_READING, run, dataId}; //packet frame, the 0s are adresses to be added later
	
	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 32 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xbeeSh, sh, shLength);
	Shared().breakUp(xbeeSl, sl, slLength);
			
	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xbeeMy, my, myLength);
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

	//puts msb, lsb, and checksum into the packet to send
	transmitPacket[Shared().getLengthMsbIndex()] = Shared().getLengthMsb(transmitPacketLength);
	transmitPacket[Shared().getLengthLsbIndex()] = Shared().getLengthLsb(transmitPacketLength);
	transmitPacket[transmitPacketLength - 1] = Shared().calcChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		packet[i] = transmitPacket[i];
	}
}

void TxExplicit::makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy, int dataType)
{
	byte frameId = 0x1;
	byte packetFrame[] = {Shared().getStart(), LENGTH_MSB, LENGTH_LSB, TX_REG_FRAME_TYPE, frameId,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BROADCAST_RADIUS, OPTIONS, dataType, CHECKSUM}; //packet frame, the 0s are adresses to be added later
	
	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 32 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xbeeSh, sh, shLength);
	Shared().breakUp(xbeeSl, sl, slLength);

	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xbeeMy, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	int frameLength = sizeof(packetFrame);

	for(int x = 0; x < frameLength; x++) {
		packet[x] = packetFrame[x];
	}

	//puts msb, lsb, and checksum into frame
	packet[Shared().getLengthMsbIndex()] = Shared().getLengthMsb(frameLength);
	packet[Shared().getLengthLsbIndex()] = Shared().getLengthLsb(frameLength);
	packet[frameLength - 1] = Shared().calcChecksum(packetFrame, frameLength);
}

void TxExplicit::makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy,
	int dataType, byte data[], int dataLength)
{
	int frameId = 0x1;
	byte packetFrame[] = {Shared().getStart(), LENGTH_MSB, LENGTH_LSB, TX_REG_FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		BROADCAST_RADIUS, OPTIONS, dataType}; //packet frame, the 0s are adresses to be added later
	
	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xbeeSh, sh, shLength);
	Shared().breakUp(xbeeSl, sl, slLength);
			
	for(int i = 0; i < shLength; i++) {
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xbeeMy, my, myLength);
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

	//keeps track of index for filling in information
	int start = i;
	//puts data in packetToSend
	if(data) {
		for(; i < transmitPacketLength - 1; i++) {
			transmitPacket[i] = data[i - start];
		}
	}

	//puts msb, lsb, and checksum into frame
	transmitPacket[Shared().getLengthMsbIndex()] = Shared().getLengthMsb(sizeof(transmitPacket));
	transmitPacket[Shared().getLengthLsbIndex()] = Shared().getLengthLsb(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = Shared().calcChecksum(transmitPacket, transmitPacketLength);

	for(int j = 0; j < transmitPacketLength; j++) {
		packet[j] = transmitPacket[j];
	}
}

void TxExplicit::makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy, int run, 
	int dataId, int value, String timeStamp, unsigned long millis) //packet frame, the 0s are adresses to be added later
{
	byte frameId = 0x0;
	int dataLength = 2, timeStampLength = 4, millisLength = 4;
	byte packetFrame[] = {Shared().getStart(), LENGTH_MSB, LENGTH_LSB, TX_REG_FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		BROADCAST_RADIUS, OPTIONS, NETWORK_TEST, run, dataId};
	
	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xbeeSh, sh, shLength);
	Shared().breakUp(xbeeSl, sl, slLength);

	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xbeeMy, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	//creates packet to be sent
	int frameLength = sizeof(packetFrame);
	int transmitPacketLength = frameLength + dataLength + timeStampLength + millisLength + 1;
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

	start = index;

	byte byteMillis[millisLength];
	Shared().breakUp(millis, byteMillis, millisLength);
	for(; index < start + millisLength; index++) {
		transmitPacket[index] = byteMillis[index - start];
	}

	//puts msb, lsb, and checksum into frame
	transmitPacket[Shared().getLengthMsbIndex()] = Shared().getLengthMsb(transmitPacketLength);
	transmitPacket[Shared().getLengthLsbIndex()] = Shared().getLengthLsb(transmitPacketLength);
	transmitPacket[transmitPacketLength - 1] = Shared().calcChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		packet[i] = transmitPacket[i];
	}
}