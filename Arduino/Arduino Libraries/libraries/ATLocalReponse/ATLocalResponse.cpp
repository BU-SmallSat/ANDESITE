#include "Arduino.h"
#include "ATLocalResponse.h"

byte ATLocalResponse::getFrameID(byte packet[])
{
	return packet[FRAME_ID_INDEX];
}

//Takes command array and turns it into a string
String ATLocalResponse::getCommand(byte packet[])
{
	char first = packet[COMMAND_INDEX];
	char second = packet[COMMAND_INDEX];
	return first + second;
}

byte ATLocalResponse::getStatus(byte packet[])
{
	return packet[STATUS_INDEX];
}

int ATLocalResponse::getData(byte* data, byte packet[], int packetLength)
{
	if(packetLength > 9) {
		int length = packetLength - 1 - DATA_INDEX;
		for(int i = 0; i < length; i++) {
			data[i] = packet[DATA_INDEX + i];
		}
		return length;
	}
}