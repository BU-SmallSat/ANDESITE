#include "Arduino.h"
#include "TXRequest.h"
#include "Potentiometer.h"
#include "XBeeInfo.h"
#include "Common.h"

TXRequest::TXRequest(long xBeeSH, long xBeeSL, int xBeeMY, int value, String timeStamp)
{
	_xBeeSH = xBeeSH;
	_xBeeSL = xBeeSL;
	_xBeeMY = xBeeMY;
	_value = value;
	_timeStamp = timeStamp;
	getPacketToSend();
}

void TXRequest::getPacketToSend()
{
	byte packetFrame[] = {START, MSB, 0, TX_REQUEST,
    		FRAME_ID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BROADCAST_RADIUS, OPTIONS};

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

	//Makes array that is actual packet to send
	int frameLength = sizeof(packetFrame);
        int dataLength = 8;
	int packetToSendLength = frameLength + dataLength + 1;
	byte packetToSend[packetToSendLength];

	//Fills frame in packetToSend
	for(int i = 0; i < frameLength; i++) {
		packetToSend[i] = packetFrame[i];
	}
        
    //Puts value and timeStamp into packetToSend
    byte byteValue[4];
    Common().toByte(_value, byteValue);
    int n = 0;
    for(; n < sizeof(int); n++) {
        packetToSend[frameLength + n] = byteValue[sizeof(int) - 1 - n];
    }
        
    int timeStampLength = 12;
    int start = n;
    String temp;
   
    byte byteTimeStamp[timeStampLength / 2];
    /*for(int x = 0; x < timeStampLength ; x+=2) {
        temp = _timeStamp.substring(x, x+2);
        byteTimeStamp[x/2] = byte(temp.toInt());
    }*/
    _timeStamp.getBytes(byteTimeStamp, timeStampLength / 2);

    for(; n < packetToSendLength - frameLength - 1; n++) {
        packetToSend[n + frameLength] = byteTimeStamp[n - start];
    }

    int lsbIndex = 2;
    int checksumIndex = packetToSendLength - 1;
    packetToSend[lsbIndex] = byte(Common().getLSB(packetToSendLength));

    int sum = 0x0;

    for (int x = 3; x <packetToSendLength - 1; x++) { 
            sum += packetToSend[x];
    }

    int checksum = 0xFF - (byte)sum;
    packetToSend[checksumIndex] = checksum;

    //packetToSend[checksumIndex] = byte(Common().getChecksum(packetToSend, packetToSendLength));

	for(int i = 0; i < packetToSendLength; i++) {
		_transmitRequest[i] = packetToSend[i];
	}
	_transmitLength = packetToSendLength;
}

//breaks up the 64 or 16 bit address into bytes
void TXRequest::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) (address % 0x100);
		address /= 0x100;
	}
}

byte TXRequest::getTXRequest(int n)
{
	return _transmitRequest[n];
}

int TXRequest::getTXLength()
{
	return _transmitLength;
}

long TXRequest::getXBeeInfo(int n)
{
	long array[] = {_xBeeSH, _xBeeSL, _xBeeMY};
	return array[n];
}