#include "Arduino.h"
#include "SoftwareSerial.h"
#include "../Shared/Shared.h"
#include "../RXExplicit/RXExplicit.h"
#include "RecievedPacket.h"

//Fills the buffer with any avaliable data
void RecievedPacket::storeIncoming(byte* bufferTop, byte*bufferBottom, SoftwareSerial &serial)
{
	_stream = &serial; //sets stream to the address of the SoftwareSerial
	while(_stream->available()) {
		*bufferBottom = _stream->read();
		bufferBottom++;
	}
}

//Takes complete packets out of the data
void RecievedPacket::getPacket(byte* packet, byte* bufferTop, byte* bufferBottom)
{
	while(*bufferTop) {
		if(*bufferTop == 0x7e) {
			int packetLength = Shared().getPacketLength(bufferTop);

			for(int i = 0; i < packetLength; i++) {
				packet[i] = bufferTop[i];
			}
			bufferTop += packetLength;
		} else {
			bufferTop++;
		}
	}
}