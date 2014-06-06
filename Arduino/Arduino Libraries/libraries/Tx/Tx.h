#ifndef TX_H
#define TX_H

#include "Arduino.h"
#include "Common.h"
#include "XbeeInfo.h"
#include "Packet.h"

class Tx
{
	public:
                // creates a general network packet with a data type followed by data
		void makeNetworkPacket(Packet & packet, XbeeInfo & xbeeInfo, int dataType, int dataId, byte data[], int dataLength);
                
                // creates a general network packet with a data type
                void makeNetworkPacket(Packet & packet, XbeeInfo & xbeeInfo, int dataType);
		//Packet * txNetworkPacket(XbeeInfo & xbeeInfo, int dataId, String timeStamp, int voltage);
	private:
                // constants for making the packets
		#define FRAME_TYPE 0x10
		#define FRAME_ID 0x0
		#define BROADCAST_RADIUS 0x0
		#define OPTIONS 0x0
		#define SENSOR_READING 0x01
		#define NETWORK_TEST_READING 0x3
		#define START_BYTE 0x7e
};
#endif