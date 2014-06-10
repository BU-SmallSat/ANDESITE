#include "Arduino.h"
#include <SoftwareSerial.h>
#include "Packet.h"
#include "Common.h"
#include "RadioStream.h"


RadioStream::RadioStream(int rPin, int tPin)
{
	SoftwareSerial radio(rPin, tPin);
	radio.begin(19200);
}

RadioStream::~RadioStream()
{
    delete &radio;
    Serial.println("Radio destructor called");
}

// return the next available packet
Packet * RadioStream::getNextPacket()
{
    if(isCompletePacket(packet))
    {
        packet.clear();
    }
    while((*radio).available() > 0)
    {
        packet.add((*radio).read());
        if(isCompletePacket(packet))
        {
                return &packet;
        }
    }
    return NULL;
}

// sends a packet
void RadioStream::send(Packet * packet)
{
    for(int x = 0; x < packet->length(); x++)
    {
        radio->write(packet->get(x));
    }
}

// returns how many bytes are available in the serial stream
int RadioStream::available()
{
    return radio->available();
}

// Evaluates packet to see if it is a complete packet
bool RadioStream::isCompletePacket(Packet & packet) const
{
    if(packet.length() > 3
            && Common().calcLength(packet) == Common().getPacketLength(packet)
            && Common().calcChecksum(packet) == Common().getPacketChecksum(packet))
    {
        return true;
    }
    return false;
}