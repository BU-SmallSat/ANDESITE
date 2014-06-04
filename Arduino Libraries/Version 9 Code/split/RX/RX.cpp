#include "Arduino.h"
#include "RX.h"

byte RX::getDataType(byte packet[])
{
	return packet[DATA_TYPE_INDEX]
}

byte RX::getRun(byte packet[])
{
	return packet[RUN_INDEX]
}

byte RX::getDataID(byte packet[])
{
	return packet[DATA_ID_INDEX]
}

byte RX::getReceiveOptions(byte packet[])
{
	return packet[OPTIONS_INDEX]
}

long RX::getSH(byte packet[])
{
	int length = 4;
	long SH;

	//converts the array of bytes back into SH
	for(int i = 0; i < length; i++) {
		SH += (packet[SH_INEDX + i] << (24 - (8 * i)));
	}
	return SH;
}

long RX::getSL(byte packet[])
{
	int length = 4;
	long SL;

	//converts the array of bytes back into SL
	for(int i = 0; i < length; i++) {
		SL += (packet[SL_INEDX + i] << (24 - (8 * i)));
	}
	return SL;
}

int RX::getMY(byte packet[])
{
	int length = 2;
	long MY;

	//converts the array of bytes back into MY
	for(int i = 0; i < length; i++) {
		MY += (packet[MY_INEDX + i] << (8 - (8 * i)));
	}
	return MY;
}

void RX::getData(byte data[], byte packet[], int packetLength)
{
	int length = packetLength - 1 - DATA_INDEX;

	for(int i = 0; i < length; i++) {
		data[i] = packet[DATA_INDEX + i];
	}
}

//The rest of these take just the data as an argument
int RX::getPotentiometerValue(byte data[])
{
	int value = data[1];
	value += data[0] << 8;
	return value;
}

String RX::getTimeStamp(byte data[])
{
	int timeStampLength = 6;
	String timeStamp;

	for(int i = 0; i < timeStampLength; i++) {
		if(data[2 + i] < 10) {
			timeStamp += '0';
		}
		timeStamp += String(data[2 + i]);
	}
}

long RX::getMillis(byte data[])
{
	int millisLength = 4;
	int millisStart = 8;
	long millis;

	//converts the array of bytes back into millis
	for(int i = 0, i < millisLength; i++) {
		millis += (data[millisStart + i] << (24 - (8 * i)));
	}
	return millis;
}