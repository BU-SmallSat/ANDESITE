#include "Arduino.h"
#include "RXExplicit.h"

byte RXExplicit::getDataType(byte packet[])
{
	return packet[DATA_TYPE_INDEX]
}

byte RXExplicit::getRun(byte packet[])
{
	return packet[RUN_INDEX]
}

byte RXExplicit::getDataID(byte packet[])
{
	return packet[DATA_ID_INDEX]
}

byte RXExplicit::getReceiveOptions(byte packet[])
{
	return packet[OPTIONS_INDEX]
}

long RXExplicit::getSH(byte packet[])
{
	int length = 4;
	long SH;

	for(int i = 0; i < length; i++) {
		SH += (packet[SH_INEDX + i] << (24 - (8 * i)));
	}
	return SH;
}

long RXExplicit::getSL(byte packet[])
{
	int length = 4;
	long SL;

	for(int i = 0; i < length; i++) {
		SL += (packet[SL_INEDX + i] << (24 - (8 * i)));
	}
	return SL;
}

int RXExplicit::getMY(byte packet[])
{
	int length = 2;
	long MY;

	for(int i = 0; i < length; i++) {
		MY += (packet[MY_INEDX + i] << (8 - (8 * i)));
	}
	return MY;
}

byte RXExplicit::getSourceEndpoint(byte packet[])
{
	return packet[SOURCE_ENDPOINT_INDEX];
}

byte RXExplicit::getDestEndpoint(byte packet[])
{
	return packet[DEST_ENDPOINT_INDEX];
}

int RXExplicit::getClusterId(byte packet[])
{
	return packet[CLUSTER_ID_INDEX];
}

int RXExplicit::getprofileId(byte packet[])
{
	return packet[PROFILE_ID_INDEX];
}

void RXExplicit::getData(byte data[], byte packet[], int packetLength)
{
	int length = packetLength - 1 - DATA_INDEX;

	for(int i = 0; i < length; i++) {
		data[i] = packet[DATA_INDEX + i];
	}
}

int RXExplicit::getPotentiometerValue(byte data[])
{
	int value = data[1];
	value += data[0] << 8;
	return value;
}

String RXExplicit::getTimeStamp(byte data[])
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

long RXExplicit::getMillis(byte data[])
{
	int millisLength = 4;
	int millisStart = 8;
	long millis;

	for(int i = 0, i < millisLength; i++) {
		millis += (data[millisStart + i] << (24 - (8 * i)));
	}
	return millis;
}

String RXExplicit::getRoutingTableEntries(byte packet[], int packetLength)
{
	int length = packetLength - 1;
	String routes;

	for(int i = ROUTING_ENTRIES_INDEX; i < length; i += 5) {
		byte dest[] = {packet[i + 1], packet[i]};
		byte status = packet[i + 2];
		byte nextHop[] = {packet[i + 4], packet[i + 3]};
		routes += String(dest[1]) + String(dest[0]) + String(status) +
			String(nextHop[1]) + String(nextHop[0])
	}
	return routes;
}