#ifndef RecievedPacket_h
#define RecievedPacket_h

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "../Shared/Shared.h"
#include "../RXExplicit/RXExplicit.h"

class RecievedPacket
{
	public:
		void storeIncoming(byte* bufferTop, byte* buffetBottom, SoftwareSerial &serial);
		void getPacket(byte* packet, byte* bufferTop, byte* buffetBottom);
	private:
		SoftwareSerial* _stream;
};
#endif