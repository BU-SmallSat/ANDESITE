#include <TXExplicit.h>
#include <Shared.h>
#include <TX.h>
#include <XBeeInfo.h>
#include <Time.h>

#define analogPin 0
#define resetPin 1
const int MILLIS_VALUE = 0x08;
const int SEND_DATA_COMMAND = 0x02;
const int STOP_DATA_COMMAND = 0x03;
const int RX_RECEIVED = 0x90;
const int RX_EXPLICIT = 0x91;
const long SH_COORD = 0x0013A200;
const long SL_COORD = 0x407B6C7D;
const long MY_COORD = 0x0;
const long SH_ROUTER_ZERO = 0x0013A200;
const long SL_ROUTER_ZERO = 0x4078EFB8;
const long SH_ROUTER_ONE = 0x0013A200;
const long SL_ROUTER_ONE = 0x409BB9E7;
const long SH_BROADCAST = 0x0;
const long SL_BROADCAST = 0x00000FFFF;
const long MY_BROADCAST = 0xFFFE;

int frameTypeIndex = 3;
int dataTypeIndex = 21;
int dataIdIndex = 16;
int counter = 0;
int secondLimit = 20;
int minuteLimit = 10;
int startSecond = 0;
int startMinute = 0;
unsigned long startMillis = 0;
int run = 1;
boolean sendRun = true;

XBeeInfo coord(SH_COORD, SL_COORD, "Coord", MY_COORD);
int potentiometerValue;
int dataId = 0x01;
int end = 23;

void setup()
{
  Serial.begin(9600);
  analogReference(EXTERNAL);
}

void loop()
{
  potentiometerValue = analogRead(analogPin);
  
  byte txDataPacket[26];
  TX().txDataPacket(txDataPacket, coord.getSH(), 
    coord.getSL(), coord.getMY(), dataId, getTime(),
    potentiometerValue);
  
  byte explicitPacket[26];
  TXExplicit().makePacket(explicitPacket, coord.getSH(), 
    coord.getSL(), coord.getMY(), 1, dataId, 0x1234, 0x23);

  if(dataId >= 0xFF)
  {
    dataId = 1;
    run++;
  }
  else
  {
    dataId++;
  }

  printPacket(txDataPacket, sizeof(txDataPacket));
  printPacket(explicitPacket, sizeof(explicitPacket));
  delay(1000);
}

void printPacket(byte packet[], int packetLength)
{
  for (int i = 0; i < packetLength; i++) {
    if(packet[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(packet[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}

String getTime()
{
  time_t t = now();
  String time;
  
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
