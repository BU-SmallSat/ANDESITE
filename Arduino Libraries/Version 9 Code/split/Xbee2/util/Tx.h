#ifndef TX_h
#define TX_h

#include "Arduino.h"
#include "Shared.h"

class TX
{
	public:
		void txCommandPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
			int dataType, int dataId, byte data[], int dataLength);
		void txDataPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId,
			String timeStamp, int value);
	private:
		#define MSB 0x00
		#define LSB 0x00
		#define frameType 0x10
		#define frameId 0x0
		#define broadcastRadius 0x00
		#define options 0x00
};
#endif