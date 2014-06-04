#ifndef Request_h
#define Request_h

#include "Arduino.h"
#include "Common.h"

class Request
{
	public:
		Request(long xBeeSH, long xBeeSL, int xBeeMY, int frameId, int personalId, int data[]);
		Request(long xBeeSH, long xBeeSL, int xBeeMY, int frameId, int personalId);
		byte getRequest(int n);
	protected:
		long _xBeeSH;
		long _xBeeSL;
		int _xBeeMY;
		int _data[] = null;
		#define START 0x7E
		#define MSB 0x00
		#define LSB 0x00
		#define FRAME_TYPE 0x10
		int _frameId;
		#define BROADCAST_RADIUS 0x00
		#define OPTIONS 0x01
		byte _packetFrame[];
		#define frameIdIndex 4
		int _personalId;
		int _LSBIndex;
		int _personalIdIndex;
		int _packetToSendLength;
		byte _packetToSend[];
	private:
		void makePacket();
		void breakUp(long address, byte array[], int arrayLength);
};
#endif