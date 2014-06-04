#include "Arduino.h"
#include "AtRemoteResponse.h"

byte AtRemoteResponse::getFrameID(byte packet[])
{
	return packet[FRAME_ID_INDEX];
}

String AtRemoteResponse::getCommand(byte packet[])
{
	char first = packet[COMMAND_INDEX];
	char second = packet[COMMAND_INDEX];
	return first + second;
}

byte AtRemoteResponse::getStatus(byte packet[])
{
	return packet[STATUS_INDEX];
}

void AtRemoteResponse::getData(byte data[], byte packet[], int packetLength)
{
	if(packetLength > 9) {
		int length = packetLength - 1 - DATA_INDEX;
		for(int i = 0; i < length; i++) {
			data[i] = packet[DATA_INDEX + i];
		}
	}
}

long AtRemoteResponse::getSH(byte packet[])
{
	int length = 4;
	long SH;

	for(int i = 0; i < length; i++) {
		SH += (packet[SH_INEDX + i] << (24 - (8 * i)));
	}
	return SH;
}

long AtRemoteResponse::getSL(byte packet[])
{
	int length = 4;
	long SL;

	for(int i = 0; i < length; i++) {
		SL += (packet[SL_INEDX + i] << (24 - (8 * i)));
	}
	return SL;
}

int AtRemoteResponse::getMy(byte packet[])
{
	int length = 2;
	long MY;

	for(int i = 0; i < length; i++) {
		MY += (packet[MY_INEDX + i] << (8 - (8 * i)));
	}
	return MY;
}