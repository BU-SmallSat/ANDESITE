#include "Arduino.h"
#include "RxExplicit.h"

byte RxExplicit::getDataType(byte packet[])
{
	return packet[DATA_TYPE_INDEX];
}

byte RxExplicit::getReceiveOptions(byte packet[])
{
	return packet[OPTIONS_INDEX];
}

long RxExplicit::getSh(byte packet[])
{
	int length = 4;
	long SH = 0;

	//converts the array of bytes back into SH
	for(int i = 0; i < length; i++) {
		SH |= ((long) packet[SH_INDEX + i] << (24 - (8 * i)));
	}
	return SH;
}

long RxExplicit::getSl(byte packet[])
{
	int length = 4;
	long SL = 0;

	//converts the array of bytes back into SL
	for(int i = 0; i < length; i++) {
		SL |= ((long) packet[SL_INDEX + i] << (24 - (8 * i)));
	}
	return SL;
}

long RxExplicit::getMy(byte packet[])
{
	int length = 2;
	long MY = 0;

	//converts the array of bytes back into MY
	for(int i = 0; i < length; i++) {
		MY |= ((long) packet[MY_INDEX + i] << (8 - (8 * i)));
	}
	return MY;
}

byte RxExplicit::getSourceEndpoint(byte packet[])
{
	return packet[SOURCE_ENDPOINT_INDEX];
}

byte RxExplicit::getDestEndpoint(byte packet[])
{
	return packet[DEST_ENDPOINT_INDEX];
}

long RxExplicit::getClusterId(byte packet[])
{
	int length = 2;
	long clusterId = 0;

	//converts the array of bytes back into cluster id
	for(int i = 0; i < length; i++) {
		clusterId |= ((long) packet[CLUSTER_ID_INDEX + i] << (8 - (8 * i)));
	}
	return clusterId;
}

long RxExplicit::getProfileId(byte packet[])
{
	int length = 2;
	long profileId = 0;

	//converts the array of bytes back into profile id
	for(int i = 0; i < length; i++) {
		profileId |= ((long) packet[PROFILE_ID_INDEX + i] << (8 - (8 * i)));
	}
	return profileId;
}

void RxExplicit::getData(byte data[], byte packet[], int packetLength)
{
	int length = packetLength - 1 - DATA_INDEX;

	for(int i = 0; i < length; i++) {
		data[i] = packet[DATA_INDEX + i];
	}
}

void RxExplicit::getSecondRadioAddress(long address[], byte packet[])
{
	int staticAddressLength = 4;
	int dynamicAddressLength = 2;
	long sh = 0;
	long sl = 0;
	long my = 0;
	int x = 0;

	//converts the array of bytes back into SH
	for(; x < staticAddressLength; x++) {
	sh |= ((long) packet[DATA_INDEX + x] << (24 - (8 * x)));
	}
	int start = x;
	//converts the array of bytes back into SL
	for(; x < start + staticAddressLength; x++) {
		sl |= ((long) packet[DATA_INDEX + x] << (24 - (8 * (x - start))));
	}
	start = x;

	for(; x < start + dynamicAddressLength; x++) {
		my |= ((long) packet[DATA_INDEX + x] << (8 - (8 * (x - start))));
	}
	
	//puts addresses together
	address[0] = sh;
	address[1] = sl;
	address[2] = my;
}