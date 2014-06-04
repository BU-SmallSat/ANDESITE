#include "Arduino.h"
#include "Common.h"
#include "Request.h"

Request(long xBeeSH, long xBeeSL, int xBeeMY, int frameId, int personalId, int data[])
{
	_xBeeSH = xBeeSH
	_xBeeSL = xBeeSL;
	_xBeeMY = xBeeMY;
	_data = data;
	_frameId = frameId;
	_personalId = personalId;
	int numParameters = sizeof(data) + 1;
	byte packetFrame = {START, MSB, LSB, FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	_packetFrame = packetFrame;
	_packetToSendLength = sizeof(packetFrame) + numParameters + 1;
	_packetToSend = new int [_packetToSendLength];
	makePacket();
}

Request(long xBeeSH, long xBeeSL, int xBeeMY, int frameId, int personalId)
{
	_xBeeSH = xBeeSH
	_xBeeSL = xBeeSL;
	_xBeeMY = xBeeMY;
	_frameId = frameId;
	_personalId = personalId;
	int numParameters = 1;
	packetFrame = {START, MSB, LSB, FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	_packetFrame = packetFrame;
	_packetToSendLength = sizeof(packetFrame) + numParameters + 1;
	_packetToSend = new int [_packetToSendLength];
	makePacket();
}

void Request::makePacket()
{
	//Splits up and puts the 64-bit address into the frame
	int shArrayLength = 4;
	byte shArray[shArrayLength];
	breakUp(_xBeeSH, shArray, shArrayLength);
	for(int i = 0; i < sizeof(shArray); i++) {
		packetFrame[i + 5] = shArray[i];
	}
	//64 bits is too long for the Arduino to handle so need to split it up
	int slArrayLength = 4;
	byte slArray[slArrayLength];
	breakUp(_xBeeSL, slArray, slArrayLength);
	for(int i = 0; i < sizeof(slArray); i++) {
		packetFrame[i + 9] = slArray[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myArrayLength = 2;
	byte myArray[myArrayLength];
	breakUp(_xBeeMY, myArray, myArrayLength);
	for(int i = 0; i < sizeof(myArray); i++) {
		packetFrame[i + 13] = myArray[i];
	}

	int i = 0;
	for(; i < frameLength; i++) {
		_packetToSend[i] = _packetFrame[i];
	}

	packetToSend[i] = _personalId;

	int start = i;

	//puts data in packetToSend
	if(data != null)
	{
		for(; i < _packetToSendLength - 1; i++) {
			_packetToSend[i] = data[i - start];
		}
	}

	packetToSend[_lsbIndex] = Common().getLSB(_packetToSend);
	packetToSend[_packetToSendLength - 1] = Common().getChecksum(_packetToSend);
}

void Request::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) (address % 0x100);
		address /= 0x100;
	}
}