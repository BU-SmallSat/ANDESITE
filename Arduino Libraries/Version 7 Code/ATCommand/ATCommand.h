#ifndef ATCommand_h
#define ATCommand_h

#include "Arduino.h"
#include "Common.h"

class ATCommand
{
	public:
		ATCommand(byte frameID, long SH, long SL, int MY, byte options, char c1, char c2);
		ATCommand(byte frameID, long SH, long SL, int MY, byte options, char c1, char c2, byte param);
		byte getATCommand(int n);
		int getCommandLength();
	private:
		int _commandLength;
		#define START 0x7E
		#define MSB 0x00
		byte LSB;
		#define FRAME_TYPE 0x17
		byte _frameID;
		long _SH;
		long _SL;
		int _MY;
		byte _remoteOptions;
		char _c1;
		char _c2;
		byte _parameter;
		bool _hasParameter;
		byte _ATCommand[20];
		void createCommand();
		void breakUp(long address, byte array[], int arrayLength);
};
#endif