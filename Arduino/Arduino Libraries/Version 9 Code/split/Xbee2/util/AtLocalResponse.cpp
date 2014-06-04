#include "Arduino.h"
#include "AtLocalResponse.h"

byte AtLocalResponse::getFrameID(byte packet[])
{
	return packet[FRAME_ID_INDEX];
}

String AtLocalResponse::getCommand(byte packet[])
{
	char first = packet[COMMAND_INDEX];
	char second = packet[COMMAND_INDEX];
	return first + second;
}

byte AtLocalResponse::getStatus(byte packet[])
{
	return packet[STATUS_INDEX];
}

void AtLocalResponse::getData(byte data[], byte packet[], int packetLength)
{
	if(packetLength > 9) {
		int length = packetLength - 1 - DATA_INDEX;
		for(int i = 0; i < length; i++) {
			data[i] = packet[DATA_INDEX + i];
		}
	}
}