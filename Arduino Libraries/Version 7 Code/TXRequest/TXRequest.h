#ifndef TXRequest_h
#define TXRequest_h

#include "Arduino.h"
#include "Potentiometer.h"
#include "XBeeInfo.h"
#include "Common.h"

class TXRequest 
{
	public:
		TXRequest(long xBeeSH, long xBeeSL, int xBeeMY, int value, String timeStamp);
		byte getTXRequest(int n);
		int getTXLength();
		long getXBeeInfo(int n);
	private:
		#define TX_REQUEST 0x10
		long _xBeeSH;
		long _xBeeSL;
		int _xBeeMY;
		int _value;
		String _timeStamp;
		#define START 0x7E
		#define MSB 0x00
		#define LSB 0x00
		#define FRAME_ID 0x01
		#define BROADCAST_RADIUS 0x00
		#define OPTIONS 0x01
		int _transmitRequest[26];
		int _transmitLength;
		void getPacketToSend();
		void breakUp(long address, byte array[], int arrayLength);

};
#endif