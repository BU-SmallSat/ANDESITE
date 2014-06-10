#include "Arduino.h"
#include "Rx.h"

byte Rx::getDataType(Packet * packet)
{
	return packet->get(DATA_TYPE_INDEX);
}

byte Rx::getRun(Packet * packet)
{
	return packet->get(RUN_INDEX);
}

byte Rx::getDataId(Packet * packet)
{
	return packet->get(DATA_ID_INDEX);
}

byte Rx::getReceiveOptions(Packet * packet)
{
	return packet->get(OPTIONS_INDEX);
}

long Rx::getSh(Packet * packet)
{
	int length = 4;
	long sh;

	//converts the array of bytes back into SH
	for(int i = 0; i < length; i++) {
		sh += (packet->get(SH_INDEX + i) << (24 - (8 * i)));
	}
	return sh;
}

long Rx::getSl(Packet * packet)
{
	int length = 4;
	long sl;

	//converts the array of bytes back into SL
	for(int i = 0; i < length; i++) {
		sl += (packet->get(SL_INDEX + i) << (24 - (8 * i)));
	}
	return sl;
}

unsigned int Rx::getMy(Packet * packet)
{
	int length = 2;
	long my;

	//converts the array of bytes back into MY
	for(int i = 0; i < length; i++) {
		my += (packet->get(MY_INDEX + i) << (8 - (8 * i)));
	}
	return my;
}

void Rx::getData(Packet * packet, byte data[])
{
	int length = packet->length() - 1 - DATA_INDEX;

	for(int i = 0; i < length; i++) {
		data[i] = packet->get(DATA_INDEX + i);
	}
}