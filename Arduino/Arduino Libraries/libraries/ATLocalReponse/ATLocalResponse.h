#ifndef ATLocalResponse_h
#define ATLocalResponse_h

#include "Arduino.h"
#include "../Shared/Shared.h"

class ATLocalResponse
{
	public:
		byte getFrameID(byte packet[]);
		String getCommand(byte packet[]);
		byte getStatus(byte packet[]);
		int getData(byte* data, byte packet[], int packetLength);
	private:
		#define FRAME_ID_INDEX 4
		#define COMMAND_INDEX 5
		#define STATUS_INDEX 7
		#define DATA_INDEX 8
};
#endif