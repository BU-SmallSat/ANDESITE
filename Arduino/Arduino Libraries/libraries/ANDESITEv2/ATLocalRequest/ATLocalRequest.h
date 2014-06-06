#ifndef ATLocalRequest_h
#define ATLocalRequest_h

#include "Arduino.h"
#include "../Shared/Shared.h"

class ATLocalRequest
{
	public:
		void atLocalRequest(byte atPacket[], int frameID, char atCommand[]);
		void atLocalRequest(byte atPacket[], int frameID, char atCommand[], char parameters[], int parametersLength);
	private:
		#define MSB 0x00
		#define LSB 0x04
		#define FRAME_TYPE 0x08
};
#endif