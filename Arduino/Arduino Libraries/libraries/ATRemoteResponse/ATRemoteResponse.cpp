#include "Arduino.h"
#include "ATRemoteResponse.h"

byte ATRemoteResponse::getFrameID(byte packet[])
{
	return packet[FRAME_ID_INDEX];
}

//Takes command array and turns it into a string
String ATRemoteResponse::getCommand(byte packet[])
{
	char first = packet[COMMAND_INDEX];
	char second = packet[COMMAND_INDEX];
	return first + second;
}

byte ATRemoteResponse::getStatus(byte packet[])
{
	return packet[STATUS_INDEX];
}

void ATRemoteResponse::getData(byte data[], byte packet[], int packetLength)
{
	if(packetLength > 9) {
		int length = packetLength - 1 - DATA_INDEX;
		for(int i = 0; i < length; i++) {
			data[i] = packet[DATA_INDEX + i];
		}
	}
}

long ATRemoteResponse::getSH(byte packet[])
{
	int length = 4;
	long SH;

	//converts the array of bytes back into SH
	for(int i = 0; i < length; i++) {
		SH += (packet[SH_INEDX + i] << (24 - (8 * i)));
	}
	return SH;
}

long ATRemoteResponse::getSL(byte packet[])
{
	int length = 4;
	long SL;

	//converts the array of bytes back into SL
	for(int i = 0; i < length; i++) {
		SL += (packet[SL_INEDX + i] << (24 - (8 * i)));
	}
	return SL;
}

int ATRemoteResponse::getMy(byte packet[])
{
	int length = 2;
	long MY;

	//converts the array of bytes back into MY
	for(int i = 0; i < length; i++) {
		MY += (packet[MY_INEDX + i] << (8 - (8 * i)));
	}
	return MY;
}