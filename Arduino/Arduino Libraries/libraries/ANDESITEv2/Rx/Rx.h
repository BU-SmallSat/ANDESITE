#ifndef RX_H
#define RX_H

#include "Arduino.h"
#include "Common.h"
#include "Packet.h"

class Rx
{
	public:
		byte getDataType(Packet * packet);
		byte getRun(Packet * packet);
		byte getDataId(Packet * packet);
		byte getReceiveOptions(Packet * packet);
		long getSh(Packet * packet);
		long getSl(Packet * packet);
		unsigned int getMy(Packet * packet);
		void getData(Packet * packet, byte data[]);
	private:
		#define DATA_TYPE_INDEX 15
		#define RUN_INDEX 16
		#define DATA_ID_INDEX 17
		#define OPTIONS_INDEX 14
		#define SH_INDEX 4
		#define SL_INDEX 8
		#define MY_INDEX 12
		#define DATA_INDEX 17
};
#endif