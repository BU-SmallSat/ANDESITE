#ifndef RxExplicit_h
#define RxExplicit_h

#include "Arduino.h"
#include "../Shared/Shared.h"

class RxExplicit
{
	public:
		byte getDataType(byte packet[]);
		byte getReceiveOptions(byte packet[]);
		long getSh(byte packet[]);
		long getSl(byte packet[]);
		long getMy(byte packet[]);
		byte getSourceEndpoint(byte packet[]);
		byte getDestEndpoint(byte packet[]);
		long getClusterId(byte packet[]);
		long getProfileId(byte packet[]);
		void getData(byte data[], byte packet[], int packetLength);
        void getSecondRadioAddress(long data[], byte packet[]);
	private:
		#define DATA_TYPE_INDEX 21
		#define OPTIONS_INDEX 20
		#define SH_INDEX 4
		#define SL_INDEX 8
		#define MY_INDEX 12
		#define SOURCE_ENDPOINT_INDEX 14
		#define DEST_ENDPOINT_INDEX 15
		#define CLUSTER_ID_INDEX 16
		#define PROFILE_ID_INDEX 18
		#define DATA_INDEX 22
};
#endif