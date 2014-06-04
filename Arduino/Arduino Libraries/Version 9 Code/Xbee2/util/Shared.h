#ifndef Shared_h
#define Shared_h

#include "Arduino.h"

class Shared
{
	public:
		void breakUp(long address, byte array[], int arrayLength);
		byte getChecksum(byte packet[], int length);
		byte getMSB(int packetLength);
		byte getLSB(int packetLength);
		byte getStart();
		int getMSBIndex();
		int getLSBIndex();
	private:
		#define START 0x7E
		#define MSB_INDEX 1
		#define LSB_INEDX 2
};
#endif