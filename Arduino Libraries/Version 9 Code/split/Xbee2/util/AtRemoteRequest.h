#ifndef AtRemoteRequest_h
#define AtRemoteRequest_h

#include "Arduino.h"
#include "Shared.h"

class AtRemoteRequest
{
	public:
		void makePacket(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY, 
			int frameId, int remoteCommandOptions, char atCommand[]);
		void makePacket(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY,
			int frameId, int remoteCommandOptions, char atCommand[], char parameters[]);
	private:
		#define MSB_LENGTH 0x0
		#define LSB_LENGTH 0x0
		#define FRAME_TYPE 0x17
		#define CHECKSUM 0x0
};
#endif