#ifndef RADIOSTREAM_H
#define RADIOSTREAM_H
#include "Arduino.h"
#include <SoftwareSerial.h>
#include "Packet.h"
#include "Common.h"

using namespace std;

class RadioStream
{
public:
    // constructor that takes rx pin and tx pin
    RadioStream(int rPin, int tPin);
    
    // destructor to get rid of dynamically allocated memory
    ~RadioStream();
    
    // return the next available packet;
    // returns NULL if there is no available packet
    Packet * getNextPacket();
    
    // sends a packet
    void send(Packet * packet);
    
    // returns how many bytes are available in the serial stream
    int available();
    
private:
    // variables
    SoftwareSerial *radio;
    Packet packet;
    
    //methods
    // Evaluates packet to see if it is a complete packet
    bool isCompletePacket(Packet & packet) const;
};

#endif