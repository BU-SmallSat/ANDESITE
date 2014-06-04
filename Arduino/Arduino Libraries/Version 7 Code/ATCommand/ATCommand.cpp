#include "Arduino.h"
#include "ATCommand.h"
#include "Common.h"

ATCommand::ATCommand(byte frameID, long SH, long SL, int MY, byte options, char c1, char c2)
{
	_commandLength = 19;
	_frameID = frameID;
	_SH = SH;
	_SL = SL;
	_MY = MY;
	_remoteOptions = options;
	_c1 = c1;
	_c2 = c2;
	_hasParameter = false;

	createCommand();
}

ATCommand::ATCommand(byte frameID, long SH, long SL, int MY, byte options, char c1, char c2, byte param)
{
	_commandLength = 20;
	_frameID = frameID;
	_SH = SH;
	_SL = SL;
	_MY = MY;
	_remoteOptions = options;
	_c1 = c1;
	_c2 = c2;
	_parameter = param;
	_hasParameter = true;

	createCommand();
}

void ATCommand::createCommand()
{
	byte ATFrame[] = {START, MSB, LSB, FRAME_TYPE, _frameID,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, _remoteOptions};

	//Splits up and puts the 64-bit address into the frame
	int shArrayLength = 4;
	byte shArray[shArrayLength];
	breakUp(_SH, shArray, shArrayLength);
	for(int i = 0; i < sizeof(shArray); i++) {
		ATFrame[i + 5] = shArray[i];
	}
	//64 bits is too long for the Arduino to handle so we need to split it up
	int slArrayLength = 4;
	byte slArray[slArrayLength];
	breakUp(_SL, slArray, slArrayLength);
	for(int i = 0; i < sizeof(slArray); i++) {
		ATFrame[i + 9] = slArray[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myArrayLength = 2;
	byte myArray[myArrayLength];
	breakUp(_MY, myArray, myArrayLength);
	for(int i = 0; i < sizeof(myArray); i++) {
		ATFrame[i + 13] = myArray[i];
	}

	//Fills frame into command packet
	for(int i = 0; i < sizeof(ATFrame); i++) {
		_ATCommand[i] = ATFrame[i];
	}

	int index = sizeof(ATFrame);
	//Puts command in command packet
	_ATCommand[index] = _c1;
	index++;
	_ATCommand[index] = _c2;
	index++;

	//If parameter exists, puts it into the command packet
	if(_hasParameter) {
		_ATCommand[index] = _parameter;
		index++;
	}

	//Calculates and puts LSB int the command packet
	_ATCommand[2] = Common().getLSB(_commandLength);

	//Calculates and puts checksum int the command packet
	_ATCommand[index] = Common().getChecksum(_ATCommand, _commandLength);
}

//breaks up the 64 or 16 bit address into bytes
void ATCommand::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) ((address >> (8 * (arrayLength - i))) & 0xFF);
	}
}

byte ATCommand::getATCommand(int n)
{
	return _ATCommand[n];
}

int ATCommand::getCommandLength()
{
	return _commandLength;
}