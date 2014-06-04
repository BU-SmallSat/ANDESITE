#ifndef Shared_h
#define Shared_h

#include "Arduino.h"

class Shared
{
	public:
		void breakUp(long address, byte array[], int arrayLength);
		byte getChecksum(byte packet[], int length);
		byte getLSB(int packetLength);
		byte getStart();
		int getMsbLengthIndex();
		int getLxbLengthIndex();
                /*String byteToHex(byte b);
                String byteToHex(byte b[], int bLength);*/
	private:
		#define START 0x7E
		#define MSB_LENGTH_INDEX 1
		#define LSB_LENGTH_INDEX 2
};
#endif