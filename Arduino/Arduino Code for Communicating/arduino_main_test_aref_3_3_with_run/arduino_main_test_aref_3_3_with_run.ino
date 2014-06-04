#include <Time.h>
#include <XBeeInfo.h>
#include <Xbee.h>
#include <SoftwareSerial.h>

//Constants
#define analogPin 0
#define resetPin 1
const int MILLIS_VALUE = 0x08;
const int SEND_DATA_COMMAND = 0x02;
const int STOP_DATA_COMMAND = 0x03;
const int RX_RECEIVED = 0x90;
const long SH_COORD = 0x0013A200;
const long SL_COORD = 0x407B6C7D;
const long MY_COORD = 0x0;
const long SH_ROUTER_ZERO = 0x0013A200;
const long SL_ROUTER_ZERO = 0x4078EFB8;
const long SH_ROUTER_ONE = 0x0013A200;
const long SL_ROUTER_ONE = 0x409BB9E7;

int frameTypeIndex = 3;
int dataTypeIndex = 15;
int dataIdIndex = 16;
int counter = 0;
int secondLimit = 10;
int minuteLimit = 10;
int startSecond = 0;
int startMinute = 0;
unsigned long startMillis = 0;
int run = 1;

SoftwareSerial out(2,3);

//objects and variables
XBeeInfo coord(SH_COORD, SL_COORD, "Coord", MY_COORD);
int potentiometerValue;
int dataId = 0x01;
bool send = false;
bool sendMillis = true;
int end = 19;
byte received[30];
boolean inSync = false;

void setup()
{
  Serial.begin(9600);
  out.begin(9600);
  analogReference(EXTERNAL);
}

void loop()
{
  if(analogRead(resetPin) > 931 && inSync == false)
  {
    startMillis = millis();
    Serial.print("Reset Pin: ");
    Serial.println(analogRead(resetPin));
    Serial.print("Potentiometer Value Pin: ");
    Serial.println(analogRead(analogPin));
    Serial.print("Start Millis: ");
    Serial.println(startMillis);
    inSync = true;
  }
  /*if(out.peek() == 0x7E) {
    byte received[out.available()];
    for(int i = 0; i < sizeof(received); i++) { 
      received[i] = out.read();
      Serial.print(received[i]);
    }
    checkPacket(received, sizeof(received));
  }*/
  
  for(int x = 0; x < out.available() && counter < end; x++)
  {
    int data = out.read();
    if(data == 0x7e)
    {
      counter = 0;
      Serial.println();
    }
      
    received[counter] = data;
    Serial.print(received[counter], HEX);
    Serial.print(", ");
    counter++;
  }
  if(counter == end)
  {
    checkPacket(received, sizeof(received));
    counter = 0;
    Serial.println();
  }
  
  if(send) {
    if(sendMillis)
    {
      /*byte time[4];
      Xbee().breakUp(millis() - startMillis, time, sizeof(time));
      byte millisPacket[19 + sizeof(time) + 1];
      Xbee().txCommandPacket(millisPacket, coord.getSH(), 
                    coord.getSL(), coord.getMY(), 0x00, 
                    MILLIS_VALUE, dataId, time, sizeof(time));*/
      //printPacket(millisPacket, sizeof(millisPacket));
      //sendPacket(millisPacket, sizeof(millisPacket));
      sendMillis = false;
      startSecond = getSecond();
      startMinute = getMinute();
    }
    sendData();
  }
}

//checks received packet for different pieces of information
void checkPacket(byte received[], int receivedLength)
{
  //checks for rx received packet
  if(received[frameTypeIndex] == RX_RECEIVED) {
    //if SEND_DATA_COMMAND
    if(received[dataTypeIndex] == SEND_DATA_COMMAND) {
      send = true;
      Serial.println("true");
      //if STOP_DATA_COMMAND
    } else if(received[dataTypeIndex] == STOP_DATA_COMMAND) {
      send = false;
      //Serial.println("false");
    }
  }
  
  //checks for route record indicator
  if(received[frameTypeIndex] == 0xA1) {
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
  //Potentiometer pot(potentiometerValue, getTime());
  
  byte txDataPacket[33];
  unsigned long millisVal = millis() - startMillis;
  //Serial.println(millis(), HEX);
  Xbee().txDataTestPacket(txDataPacket, coord.getSH(), 
                coord.getSL(), coord.getMY(), dataId, 
                getTime(), potentiometerValue, millisVal, run);
  //Serial.println(millisVal);
  sendPacket(txDataPacket, sizeof(txDataPacket));
  //for testing purposes
  printPacket(txDataPacket, sizeof(txDataPacket));
  Serial.println(dataId, HEX);
  if(dataId >= 0xFF)
  {
    dataId = 1;
    run++;
  }
  else
  {
    dataId++;
  }
}

//sends any packet it is given
void sendPacket(byte packet[], int packetLength)
{
  for (int i = 0; i < packetLength; i++) {
     out.write(packet[i]);
   }
   //delay(2);
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
  int currentSecond = second(t);
  int currentMinute = minute(t);
  int passedSeconds = abs(currentSecond - startSecond);
  int passedMinutes = currentMinute - startMinute;
  /*if(passedSeconds > secondLimit)
  {
    send = false;
  }*/
  if(passedMinutes > minuteLimit)
  {
    send = false;
    Serial.println("Done.");
  }
  //Serial.println(passedSeconds);
  return time;
}

int getSecond()
{
  time_t t = now();
  return second(t);
}

int getMinute()
{
  time_t t = now();
  return minute(t);
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

//forces xbee to rejoin network
void rejoinNetwork()
{
  char cb[] = {'C', 'B'};
  char param[] = {4};
  byte rejoinCommand[9];
  Xbee().atLocalCommand(rejoinCommand, 0x1, cb, param);
  printPacket(rejoinCommand, 9);
}
