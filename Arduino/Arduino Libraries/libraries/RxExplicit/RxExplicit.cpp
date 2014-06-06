#include "Arduino.h"
#include "RxExplicit.h"

// returns the data type byte
byte RxExplicit::getDataType(Packet & packet)
{
	return packet.get(DATA_TYPE_INDEX);
}

// returns the receive options byte
byte RxExplicit::getReceiveOptions(Packet & packet)
{
	return packet.get(OPTIONS_INDEX);
}

// returns the serial high address
long RxExplicit::getSh(Packet & packet)
{
	int length = 4;
	long sh = 0;

	//converts the array of bytes back into a long
	for(int i = 0; i < length; i++) {
		sh |= ((long) packet.get(SH_INDEX + i) << (24 - (8 * i)));
	}
	return sh;
}

// returns the serial low address
long RxExplicit::getSl(Packet & packet)
{
	int length = 4;
	long sl = 0;

	//converts the array of bytes back into a long
	for(int i = 0; i < length; i++) {
		sl |= ((long) packet.get(SL_INDEX + i) << (24 - (8 * i)));
	}
	return sl;
}

// returns the 16-bit address
unsigned int RxExplicit::getMy(Packet & packet)
{
	int length = 2;
	unsigned int my = 0;

	//converts the array of bytes back into an unsigned int
	for(int i = 0; i < length; i++) {
		my |= ((unsigned int) packet.get(MY_INDEX + i) << (8 - (8 * i)));
	}
	return my;
}

// returns the source endpoint byte
byte RxExplicit::getSourceEndpoint(Packet & packet)
{
	return packet.get(SOURCE_ENDPOINT_INDEX);
}

// returns the destination endpoint byte
byte RxExplicit::getDestEndpoint(Packet & packet)
{
	return packet.get(DEST_ENDPOINT_INDEX);
}

// returns the cluster id
unsigned int RxExplicit::getClusterId(Packet & packet)
{
	int length = 2;
	unsigned int clusterId = 0;

	//converts the array of bytes back into an unsigned int
	for(int i = 0; i < length; i++) {
		clusterId |= ((unsigned int) packet.get(CLUSTER_ID_INDEX + i) << (8 - (8 * i)));
	}
	return clusterId;
}

// returns the profile id
unsigned int RxExplicit::getProfileId(Packet & packet)
{
	int length = 2;
	unsigned int profileId = 0;

	//converts the array of bytes back into an unsigned int
	for(int i = 0; i < length; i++) {
		profileId |= ((unsigned int) packet.get(PROFILE_ID_INDEX + i) << (8 - (8 * i)));
	}
	return profileId;
}

// puts the RF data in the data packet
void RxExplicit::getData(Packet & packet, Packet & data)
{
	int length = packet.length() - 1 - DATA_INDEX;

        // add the bytes to the data packet
	for(int i = 0; i < length; i++) {
		data.add(packet.get(DATA_INDEX + i));
	}
}

// finds the serial high, serial low, and 16-bit address
void RxExplicit::getSecondRadioAddress(Packet & packet, long & sh, long & sl, unsigned int & my)
{
	int staticAddressLength = 4;
	int dynamicAddressLength = 2;
	int x = 0;

	//converts the array of bytes back into a long
	for(; x < staticAddressLength; x++) {
	sh |= ((long) packet.get(DATA_INDEX + x) << (24 - (8 * x)));
	}
	int start = x;
        
	//converts the array of bytes back into a long
	for(; x < start + staticAddressLength; x++) {
		sl |= ((long) packet.get(DATA_INDEX + x) << (24 - (8 * (x - start))));
	}
	start = x;

        // convert the array of bytes back into an unsigned int
	for(; x < start + dynamicAddressLength; x++) {
		my |= ((unsigned int) packet.get(DATA_INDEX + x) << (8 - (8 * (x - start))));
	}
}