#ifndef Xbee_h
#define Xbee_h

#include "Arduino.h"


class Xbee
{
	public:
		void breakUp(long address, byte array[], int arrayLength);
		void txCommandPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
			int dataType, int dataId, byte data[], int dataLength);
		void txDataPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId,
			String timeStamp, int value, int run);
		void atLocalCommand(byte atPacket[], int frameId, char atCommand[]);
		void atLocalCommand(byte atPacket[], int frameId, char atCommand[], char parameters[]);
		void atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY, 
			int frameId, int remoteCommandOptions, char atCommand[]);
		void atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY,
			int frameId, int remoteCommandOptions, char atCommand[], char parameters[]);
		void txDataTestPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId, 
			String timeStamp, int value, unsigned long timeMillis, int run);
                void toByteArray(int val, byte b[2]);
                int toInt(byte b[2]);
	private:
		//#define hexArray {'0', '1', '2', '3', '4','5','6','7','8','9','a','b','c','d','e','f'}
		#define START 0x7E
		#define MSBIndex 1
		#define LSBIndex 2
		#define SENSOR_READING 1
                #define NETWORK_TEST 9
		byte getChecksum(byte packet[], int length);
		byte getLSB(int packetLength);
};
#endif