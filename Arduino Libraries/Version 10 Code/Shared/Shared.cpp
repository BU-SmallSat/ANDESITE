#include "Arduino.h"
#include "Shared.h"

//Splits an int or a long into an array of bytes
void Shared::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) ((address >> ((arrayLength * 8) - (8 * i))) & 0xFF);
	}
}

byte Shared::getFrameType(byte packet[])
{
    return packet[FRAME_TYPE_INDEX];
}

//Calculates checksum for a packet
byte Shared::calcChecksum(byte packet[], int length)
{
	int sum = 0x0;

	for (int x = 3; x <length - 1; x++) { 
		sum += packet[x];
	}

	byte checksum = 0xFF - (byte)(sum & 0xFF);
	return checksum;
}

byte Shared::getPacketChecksum(byte packet[], int length)
{
    return packet[length - 1];
}

int Shared::getPacketLength(byte packet[])
{
    return int((packet[LENGTH_MSB_INDEX] << 8) + (packet[LENGTH_LSB_INDEX] & 0xFF));
}

//calculates the length of a packet
int Shared::calcLength(int length)
{
    return length - 4;
}

//calculates the MSB of a packet
byte Shared::getLengthMsb(int packetLength)
{
	byte msb = (byte) (((packetLength - 4) >> 8) & 0xFF);
	return msb;
}

//calculates the LSB of a packet
byte Shared::getLengthLsb(int packetLength)
{
	byte lsb = (byte) (packetLength - 4 & 0xFF);
	return lsb;
}

byte Shared::getStart()
{
	return START;
}

int Shared::getLengthMsbIndex()
{
	return LENGTH_MSB_INDEX;
}

int Shared::getLengthLsbIndex()
{
	return LENGTH_LSB_INDEX;
}