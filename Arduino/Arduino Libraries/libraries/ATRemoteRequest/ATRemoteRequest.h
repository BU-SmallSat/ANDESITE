#ifndef AT_h
#define AT_h

#include "Arduino.h"
#include "Shared.h"

class AT
{
	public:
		void atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY, 
			int frameId, int remoteCommandOptions, char atCommand[]);
		void atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY,
			int frameId, int remoteCommandOptions, char atCommand[], char parameters[]);
	private:
		#define MSB 0x0
		#define LSB 0x0
		#define FRAME_TYPE 0x17
		#define CHECKSUM 0x0
};
#endif