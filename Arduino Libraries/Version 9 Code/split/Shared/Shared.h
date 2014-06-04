#ifndef Shared_h
#define Shared_h

#include "Arduino.h"

class Shared
{
	public:
		void breakUp(long address, byte array[], int arrayLength);
                byte calcChecksum(byte packet[], int length);
                byte getPacketChecksum(byte packet[], int length);
		byte getLengthMsb(int packetLength);
		byte getLengthLsb(int packetLength);
		byte getStart();
		int getLengthMsbIndex();
		int getLengthLsbIndex();
                int getPacketLength(byte packet[]);
                byte getFrameType(byte packet[]);
                int calcLength(int length);
	private:
		#define START 0x7E
		#define LENGTH_MSB_INDEX 1
		#define LENGTH_LSB_INDEX 2
                #define FRAME_TYPE_INDEX 3
};
#endif