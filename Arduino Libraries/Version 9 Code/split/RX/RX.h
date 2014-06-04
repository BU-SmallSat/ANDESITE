#ifndef RX_h
#define RX_h

#include "Arduino.h"
#include "../Shared/Shared.h"

class RX
{
	public:
		byte getDataType(byte packet[]);
		byte getRun(byte packet[]);
		byte getDataID(byte packet[]);
		byte getReceiveOptions(byte packet[]);
		long getSH(byte packet[]);
		long getSL(byte packet[]);
		int getMY(byte packet[]);
		void getData(byte data[], byte packet[], int packetLength);
		int getPotentiometerValue(byte data[]);
		String getTimeStamp(byte data[]);
		long getMillis(byte data[]);
	private:
		#define DATA_TYPE_INDEX 15
		#define RUN_INDEX 16
		#define DATA_ID_INDEX 16
		#define OPTIONS_INDEX 14
		#define SH_INDEX 4
		#define SL_INDEX 8
		#define MY_INDEX 12
		#define DATA_INDEX 17
};
#endif