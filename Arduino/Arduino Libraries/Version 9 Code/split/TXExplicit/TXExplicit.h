#ifndef TxExplicit_h
#define TxExplicit_h

#include "Arduino.h"
#include "Shared.h"

class TxExplicit
{
	public:
		void makePacket(byte packet[], long xbeeSh, long xbeeSl, long xBeeMy,
			int clusterId, int transactionNumber, int startIndex);
                void makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy, int run,
			int dataId, int value, String timeStamp);
                void makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy, int dataType);
                void makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy,
			int dataType, byte data[], int dataLength);
                void makePacket(byte packet[], long xbeeSh, long xbeeSl, long xbeeMy, int run,
			int dataId, int value, String timeStamp, unsigned long millis);
	private:
		#define LENGTH_MSB 0x00
		#define LENGTH_LSB 0x00
		#define TX_EX_FRAME_TYPE 0x11
		#define BROADCAST_RADIUS 0x00
		#define OPTIONS 0x00
		#define SOURCE_ENDPOINT 0x0
		#define DEST_ENDPOINT 0x0
		#define PROFILE_ID_0 0x0
		#define PROFILE_ID_1 0x0
		#define CHECKSUM 0x0
		#define TX_REG_FRAME_TYPE 0x10
		#define SENSOR_READING 0x01
                #define NETWORK_TEST 0x9
};
#endif