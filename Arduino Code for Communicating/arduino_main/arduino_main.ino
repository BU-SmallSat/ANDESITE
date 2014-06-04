#include <Time.h>
#include <XBeeInfo.h>
#include <Xbee.h>
#include <Potentiometer.h>
#include <Common.h>
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
bool send = false;

void setup()
{
  Serial.begin(9600);
  out.begin(9600);
  analogReference(EXTERNAL);
}

void loop()
{
  byte recieved[out.available()];
  for(int i = 0; i < sizeof(recieved); i++) { 
    recieved[i] = out.read();
  }
  if(recieved[17] == 0x02) { //if SEND_DATA_COMMAND
    send = true;
  } else if(recieved[17] == 0x03) { //if STOP_DATA_COMMAND
    send = false;
  }
  
  if(send) {
    sendData();
  }
}

//creates and sends a packet of data
void sendData()
{
  potentiometerValue = analogRead(analogPin);
  Potentiometer pot(potentiometerValue, getTime());
  
  byte txDataPacket[28];
  Xbee().txDataPacket(txDataPacket, coord.getSH(), coord.getSL(), coord.getMY(), id, pot.getTimeStamp(), pot.getValue());

  sendPacket(txDataPacket, sizeof(txDataPacket));
  //printPacket(txDataPacket, sizeof(txDataPacket));  //for testing purposes

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
