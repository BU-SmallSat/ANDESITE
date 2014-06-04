#ifndef TXExplicit_h
#define TXExplicit_h

#include "Arduino.h"
#include "Shared.h"

class TXExplicit
{
	public:
		void makePacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
			int transactionNumber, int clusterId, int startIndex);
		//void makePacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
		//	int dataType, int dataId, byte data[], int dataLength);
		void getRoutingTables(byte routingTable[], long xBeeSH, long xBeeSL, long xBeeMY,
			int frameId, int transactionNumber, int startIndex);
	private:
		#define MSB 0x00
		#define LSB 0x00
		#define frameType 0x11
		#define broadcastRadius 0x00
		#define options 0x00
		#define sourceEndpoint 0x0
		#define destEndpoint 0x0
		#define profileId0 0x0
		#define profileId1 0x0
		#define checksum 0x0
};
#endif