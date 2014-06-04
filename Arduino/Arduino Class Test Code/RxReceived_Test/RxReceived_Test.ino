#include <RxReceived.h>

uint8_t packet[] = {
 0x7E, 0x00, 0x11, 0x90, 0x00, 0x13, 0xA2, 0x00, 0x40, 0x52, 0x2B, 0xAA, 0x7D, 
 0x84, 0x01, 0x52, 0x78, 0x44, 0x61, 0x74, 0x61, 0x0D }; // size 22; from "Wireless Sensors" book pg. 130
 
const int packetLength = sizeof(packet)/sizeof(packet[0]);

RxReceived test( packet, 22 );

void setup(){
  Serial.begin(9600);
}

void loop(){
  
  for(int x = 0; x < packetLength; x++){
  Serial.println(test.getPacketAt(x), HEX);
  }
  Serial.println(test.getSH());
  Serial.println(test.getSL());
  Serial.println(test.getMY());
  
  for(int x = 0; x <= 5; x++)
  {
    Serial.println(test.getDataAt(x), HEX);
  }

  delay(1000);
}





