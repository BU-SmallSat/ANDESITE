#ifndef COMMON_H
#define COMMON_H

#include "Arduino.h"
#include "Packet.h"

class Common
{
public:
    
    // Breaks up a long into an array to be put into a packet
    void breakUp(long address, byte array[], int arrayLength);
    
    // Returns the frame type of a packet
    byte getFrameType(Packet & packet) const;
    
    // Returns the checksum of a packet
    byte getPacketChecksum(Packet & packet) const;
    
    // Calculates and returns the checksum
    byte calcChecksum(Packet & packet) const;
    
    // Calculates and returns the packet length
    unsigned int calcLength(Packet & packet) const;
    
    // Returns the packet length
    unsigned int getPacketLength(Packet & packet) const;
};
#endif