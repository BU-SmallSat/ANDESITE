#ifndef ATRemoteResponse_h
#define ATRemoteResponse_h

#include "Arduino.h"
#include "Shared.h"

class ATRemoteResponse
{
	public:
		byte getFrameID(byte packet[]);
		String getCommand(byte packet[]);
		byte getStatus(byte packet[]);
		void getData(byte data[], byte packet[], int packetLength);
		long getSH(byte packet[]);
		long getSL(byte packet[]);
		int getMy(byte packet[]);
	private:
		#define FRAME_ID_INDEX 4
		#define COMMAND_INDEX 15
		#define STATUS_INDEX 17
		#define DATA_INDEX 18
		#define SH_INDEX 5
		#define SL_INDEX 9
		#define MY_INDEX 13
};
#endif