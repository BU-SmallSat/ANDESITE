#include "Arduino.h"
#include "Shared.h"

void Shared::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) ((address >> ((arrayLength * 8) - (8 * i))) & 0xFF);
	}
}

byte Shared::getChecksum(byte packet[], int length)
{
	int sum = 0x0;

	for (int x = 3; x <length - 1; x++) { 
		sum += packet[x];
	}

	byte checksum = 0xFF - (byte)(sum & 0xFF);
	return checksum;
}

byte Shared::getMSB(int packetLength)
{
	byte MSB = (byte) (((packetLength - 4) >> 8) & 0xFF);
	return MSB;
}

byte Shared::getLSB(int packetLength)
{
	byte LSB = (byte) (packetLength - 4 & 0xFF);
	return LSB;
}

byte Shared::getStart()
{
	return START;
}

int Shared::getMSBIndex()
{
	return MSB_INDEX;
}

int Shared::getLSBIndex()
{
	return LSB_INDEX;
}