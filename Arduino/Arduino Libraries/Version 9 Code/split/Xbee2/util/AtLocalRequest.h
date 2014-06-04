#ifndef AtLocalRequest_h
#define AtLocalRequest_h

#include "Arduino.h"
#include "Shared.h"

class AtLocalRequest
{
	public:
		void makePacket(byte atPacket[], int frameId, char atCommand[]);
		void makePacket(byte atPacket[], int frameId, char atCommand[], char parameters[], int parameterLength);
	private:
		#define MSB_LENGTH 0x00
		#define LSB_LENGTH 0x04
		#define FRAME_TYPE 0x08
};
#endif