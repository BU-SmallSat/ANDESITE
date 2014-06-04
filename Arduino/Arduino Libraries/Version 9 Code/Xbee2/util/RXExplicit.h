#ifndef RXExplicit_h
#define RXExplicit_h

#include "Arduino.h"
#include "Shared.h"

class RXExplicit
{
	public:
		byte getDataType(byte packet[]);
		byte getRun(byte packet[]);
		byte getDataID(byte packet[]);
		byte getReceiveOptions(byte packet[]);
		long getSH(byte packet[]);
		long getSL(byte packet[]);
		int getMY(byte packet[]);
		byte getSourceEndpoint(byte packet[]);
		byte getDestEndpoint(byte packet[]);
		int getClusterId(byte packet[]);
		int getprofileId(byte packet[]);
		void getData(byte data[], byte packet[], int packetLength);
		int getPotentiometerValue(byte data[]);
		String getTimeStamp(byte data[]);
		long getMillis(byte data[]);
		String getRoutingTableEntries(byte packet[], int packetLength);
	private:
		#define DATA_TYPE_INDEX 21
		#define RUN_INDEX 22
		#define DATA_ID_INDEX 23
		#define OPTIONS_INDEX 20
		#define SH_INDEX 4
		#define SL_INDEX 8
		#define MY_INDEX 12
		#define SOURCE_ENDPOINT_INDEX 14
		#define DEST_ENDPOINT_INDEX 15
		#define CLUSTER_ID_INDEX 16
		#define PROFILE_ID_INDEX 18
		#define DATA_INDEX 24
		#define ROUTING_ENTRIES_INDEX 26
};
#endif