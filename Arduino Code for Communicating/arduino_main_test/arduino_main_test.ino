#include <Time.h>
#include <XBeeInfo.h>
#include <Xbee.h>
#include <Potentiometer.h>
#include <SoftwareSerial.h>

//Constants
#define analogPin 0
const long SH_COORD = 0x0013A200;
const long SL_COORD = 0x407B6C7D;
const long MY_COORD = 0x0;
const long SH_ROUTER_ZERO = 0x0013A200;
const long SL_ROUTER_ZERO = 0x4078EFB8;
const long SH_ROUTER_ONE = 0x0013A200;
const long SL_ROUTER_ONE = 0x409BB9E7;

SoftwareSerial out(2,3);

//objects and variables
XBeeInfo coord(SH_COORD, SL_COORD, "Coord", MY_COORD);
int potentiometerValue;
int id = 0x00;
bool send = true;

void setup()
{
  Serial.begin(9600);
  out.begin(9600);
  analogReference(EXTERNAL);
  byte time[4];
  Xbee().breakUp(millis(), time, sizeof(time));
  byte millisPacket[19 + sizeof(time) + 1];
  Xbee().txCommandPacket(millisPacket, coord.getSH(), coord.getSL(), coord.getMY(), 0x00, 0x08, 0x00, time, sizeof(time));
  printPacket(millisPacket, sizeof(millisPacket));
}

void loop()
{
  if(out.peek() == 0x7E) {
    byte received[out.available()];
    for(int i = 0; i < sizeof(received); i++) { 
      received[i] = out.read();
    }
    checkPacket(received, sizeof(received));
  }
  
  if(send) {
    sendData();
  }
  sendData();
}

//checks received packet for different pieces of information
void checkPacket(byte received[], int receivedLength)
{
  if(received[3] == 0x90) { //checks for rx received packet
    if(received[17] == 0x02) { //if SEND_DATA_COMMAND
      send = true;
    } else if(received[17] == 0x03) { //if STOP_DATA_COMMAND
      send = false;
    }
  }
  
  if(received[3] == 0xA1) { //checks for route record indicator
    for(int i = 0; i < received[15]; i++) {
      Serial.print("Address");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(received[16 + (2 * i)]);
      Serial.println(received[17 + (2 * i)]);
    }
  }
}

//creates and sends a packet of data
void sendData()
{
  potentiometerValue = analogRead(analogPin);
  Potentiometer pot(potentiometerValue, getTime());
  
  byte txDataPacket[32];
  Serial.println(millis(), HEX);
  Xbee().txDataTestPacket(txDataPacket, coord.getSH(), coord.getSL(), coord.getMY(), id, pot.getTimeStamp(), pot.getValue(), millis());

  //sendPacket(txDataPacket, sizeof(txDataPacket));
  printPacket(txDataPacket, sizeof(txDataPacket));  //for testing purposes

  id++;
}

//sends any packet it is given
void sendPacket(byte packet[], int packetLength)
{
  for (int i = 0; i < packetLength; i++) {
     out.write(packet[i]);
   }
}

//gets the time using the Time library in YYMMDDHHMMSS format
String getTime()
{
  time_t t = now();
  String time = String(year(t) % 100);
  if(month(t) < 10) {
    time += '0';
  }
  time += String(month(t));
  if(day(t) < 10) {
    time += '0';
  }
  time += String(day(t));
  if(hour(t) < 10) {
    time += '0';
  }
  time += String(hour(t));
  if(minute(t) < 10) {
    time += '0';
  }
  time += String(minute(t));
  if(second(t) < 10) {
    time += '0';
  }
  time += String(second(t));
  return time;
}

//prints a packet (for testing purposes)
void printPacket(byte packet[], int packetLength)
{
  for (int i = 0; i < packetLength; i++) {
     if(packet[i] < 0x10) {
       Serial.print('0');
     }
     Serial.print(packet[i], HEX);
     Serial.print(" ");
   }
   Serial.println();
}
