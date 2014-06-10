#include "Arduino.h"
#include "Common.h"
#include "Packet.h"

// Breaks up a long into an array to be put into a packet
void Common::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) ((address >> ((arrayLength * 8) - (8 * i))) & 0xFF);
	}
}

// Returns the frame type of a packet
byte Common::getFrameType(Packet & packet) const
{
    return packet.get(3);
}

// Returns the checksum of a packet
byte Common::getPacketChecksum(Packet & packet) const
{
    return packet.get(packet.length() - 1);
}

//Calculates and returns the checksum by adding all the bytes starting at the frame type and ending at
//the byte before the checksum; returns 0xFF minus the sum
byte Common::calcChecksum(Packet & packet) const
{
    int length = packet.length();
    int sum = 0;
    
    for(int x = 3; x < length - 1; x++)
    {
        sum += packet.get(x);
    }
    
    return 0xff - sum;
    
}

// Calculates and returns the packet length
unsigned int Common::calcLength(Packet & packet) const
{
    return packet.length() - 4;
}

// Returns the packet length
unsigned int Common::getPacketLength(Packet & packet) const
{
    unsigned int length = 0;
    for(int i = 0; i < 2; i++)
    {
        length |= ((unsigned int) packet.get(i + 1) << (8 - (8 * i)));
    }
    return length;
}