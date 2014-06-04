#ifndef AtLocalResponse_h
#define AtLocalResponse_h

#include "Arduino.h"
#include "Shared.h"

class AtLocalResponse
{
	public:
		byte getFrameID(byte packet[]);
		String getCommand(byte packet[]);
		byte getStatus(byte packet[]);
		void getData(byte data[], byte packet[], int packetLength);
	private:
		#define FRAME_ID_INDEX 4
		#define COMMAND_INDEX 5
		#define STATUS_INDEX 7
		#define DATA_INDEX 8
};
#endif