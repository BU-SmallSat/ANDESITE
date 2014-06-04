#include <Time.h>
#include <TxExplicit.h>
#include <XbeeInfo.h>
#include <RxExplicit.h>
#include <Shared.h>
#include <SoftwareSerial.h>

SoftwareSerial out(2, 3);

//Constants
#define analogPin 0
#define resetPin 1
const int SEND_DATA_COMMAND = 0x02;
const int STOP_DATA_COMMAND = 0x03;
const int DONE_SENDING_DATA = 0x07;
const int MILLIS_VALUE_REQUEST = 0x08;
const int IN_SYNC = 0x11;
const int ADDRESS_INFO = 0x12;
const int DEST_RADIO = 0x13;
const int SET_SECOND_RADIO_RECEIVED = 0x14;
const int START_OVER = 0x15;
const int STARTED_OVER = 0x16;
const int SET_DEST_RADIO_RECEIVED = 0x17;
const int MILLIS_VALUE_RESPONSE = 0x18;
const int RX_EXPLICIT = 0x91;
const long SH_COORD = 0x0;
const long SL_COORD = 0x0;
const long MY_COORD = 0xFFFE;
const long SH_BROADCAST = 0x0;
const long SL_BROADCAST = 0x0000FFFF;
const long MY_BROADCAST = 0xFFFE;
const int THREE_VOLTS = 931;
const byte START_BIT = 0x7E;
const int TX_EXPLICIT_LENGTH = 26;
const int TX_SENSOR_LENGTH = 27;
const int TX_NO_DATA_LENGTH = 19;
const int TX_NETWORK_TEST_LENGTH = 31;

//Other Variables
unsigned long startMillis = 0;
boolean inSync = false;
Shared shared;
RxExplicit rx;
TxExplicit tx;
byte packet[] = {0x7e, 0x0, 0x21, 0x91, 0x0, 0x13, 0xa2,
          0x0, 0x40, 0xa8, 0xbd, 0xe2, 0x2d, 0x7d, 0xe8,
          0xe8, 0x0, 0x11, 0xc1, 0x5, 0x1, 0x9, 0x1, 0x1,
          0x2, 0xaa, 0x46, 0x1, 0x1, 0x0, 0x0, 0x11, 0x0,
          0x0, 0x0, 0xc0, 0x10};
int counter = 0;
byte rdbuff[100];
XbeeInfo broadcast(0x0, 0x0000ffff, 0xfffe, "BROADCAST");
XbeeInfo coord(0x0, 0x0, 0xfffe, "COORD");
XbeeInfo secondRadio(0x0, 0x0, 0x0, "SECOND");
XbeeInfo destRadio(0x0, 0x0, 0x0, "DESTINATION");
int secondLimit = 10;
int minuteLimit = 10;
int startSecond = 0;
int startMinute = 0;
int run = 1;
int dataId = 0x1;
boolean sendData = false;
boolean sendDone = false;

void setup()
{
  out.begin(19200);
  Serial.begin(19200);
  //set the analog reference to the voltage being given to
  //the potentiometer
  analogReference(EXTERNAL);
}

void loop()
{
  if(analogRead(resetPin) > THREE_VOLTS && inSync == false)
  {
    startMillis = millis();
    inSync = true;
    Serial.println("reset clock");
  }
  
  if(out.available() > 0)
  {
    int availableLength = out.available();
    for(int x = 0; x < availableLength; x++, counter++)
    {
      rdbuff[counter] = out.read();
      int packetLength = shared.getPacketLength(rdbuff);
      if(counter > 4 && packetLength == (counter-3) &&
        shared.calcChecksum(rdbuff, counter+1) ==
        shared.getPacketChecksum(rdbuff, counter+1))
      {
        byte packet[counter+1];
        for(int n = 0; n <= counter; n++)
        {
          packet[n] = rdbuff[n];
          Serial.print(packet[n], HEX);
          Serial.print(", ");
        }
        Serial.println();
        
        if(shared.getFrameType(packet) == RX_EXPLICIT)
        {
        
          if(rx.getDataType(packet) == ADDRESS_INFO)
          {
              long secondRadioInfo[3];
              rx.getSecondRadioAddress(secondRadioInfo, packet);
              secondRadio.setSh(secondRadioInfo[0]);
              secondRadio.setSl(secondRadioInfo[1]);
              secondRadio.setMy(secondRadioInfo[2]);
              sendUpdate(SET_SECOND_RADIO_RECEIVED);
              Serial.println("set second radio");
          }
          else if(rx.getDataType(packet) == SEND_DATA_COMMAND)
          {
            sendData = true;
            startSecond = getSecond();
            startMinute = getMinute();
            Serial.println("sending data");
          }
          else if(rx.getDataType(packet) == STOP_DATA_COMMAND)
          {
            sendData= false;
            Serial.println("stopping data");
          }
          else if(rx.getDataType(packet) == START_OVER)
          {
            run = 1;
            dataId = 1;
            sendData = false;
            inSync = false;
            sendUpdate(STARTED_OVER);
            Serial.println("started over");
          }
          else if(rx.getDataType(packet) == DEST_RADIO)
          {
            int dataLength = 1;
            byte data[dataLength];
            rx.getData(data, packet, counter+1);
            if(data[0] == 1)
            {
              destRadio.setSh(coord.getSh());
              destRadio.setSl(coord.getSl());
              destRadio.setMy(coord.getMy());
            }
            else if(data[0] == 2)
            {
              destRadio.setSh(secondRadio.getSh());
              destRadio.setSl(secondRadio.getSl());
              destRadio.setMy(secondRadio.getMy());
            }
            
            sendUpdate(SET_DEST_RADIO_RECEIVED, data, dataLength);
            Serial.println("set destination radio");
          }
          else if(rx.getDataType(packet) == MILLIS_VALUE_REQUEST)
          {
            Serial.print("sending insync: ");
            byte data[1];
            if(inSync)
            {
              data[0] = 1;
              Serial.println("true");
            }
            else
            {
              data[0] = 2;
              Serial.println("false");
            }
            sendUpdate(MILLIS_VALUE_RESPONSE, data, 1);
          }
        }
        counter = -1;
      }
    }
  }
  
  if(sendData)
  {
    sendNetworkTestData();
    if(dataId >= 0xff)
    {
      dataId = 1;
      run++;
    }
    else
    {
      dataId++;
    }
  }
  
  if(sendData && !sendDone && getSecond() - startSecond >= secondLimit)
  {
    sendData = false;
    if(dataId == 1)
    {
      run--;
      dataId = 0xff;
    }
    else
    {
      dataId--;
    }
    byte data[] = {run, dataId};
    sendUpdate(DONE_SENDING_DATA, data, sizeof(data));
    sendDone = true;
    Serial.println("done sending data");
  }
  if(getMinute() - startMinute >= minuteLimit)
  {
    sendData = false;
  }
}

//gets the time using the Time library in DDHHMMSS format
String getTime()
{
  time_t t = now();
  String time = String(day(t));
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

void sendSensorData()
{
  byte txSensorPacket[TX_SENSOR_LENGTH];
  tx.makePacket(txSensorPacket, destRadio.getSh(), destRadio.getSl(),
    destRadio.getMy(), run, dataId, analogRead(analogPin), getTime());
  for(int x = 0; x < TX_SENSOR_LENGTH; x++)
  {
    out.write(txSensorPacket[x]);
    Serial.print(txSensorPacket[x], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

void sendNetworkTestData()
{
  byte txNetworkTestPacket[TX_NETWORK_TEST_LENGTH];
  tx.makePacket(txNetworkTestPacket, destRadio.getSh(),
    destRadio.getSl(), destRadio.getMy(), run, dataId,
    analogRead(analogPin), getTime(), millis() - startMillis);
    
  for(int x = 0; x < TX_NETWORK_TEST_LENGTH; x++)
  {
    out.write(txNetworkTestPacket[x]);
    Serial.print(txNetworkTestPacket[x], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

void sendUpdate(int dataType)
{
  Serial.print("update: ");
  Serial.println(dataType, HEX);
  byte txUpdate[TX_NO_DATA_LENGTH];
  tx.makePacket(txUpdate, coord.getSh(), coord.getSl(),
    coord.getMy(), dataType);
    
  for(int x = 0; x < TX_NO_DATA_LENGTH; x++)
  {
    out.write(txUpdate[x]);
    Serial.print(txUpdate[x], HEX);
    Serial.print(", ");
  }
  Serial.println();
}

void sendUpdate(int dataType, byte data[], int dataLength)
{
  Serial.print("update: ");
  Serial.println(dataType, HEX);
  int length = TX_NO_DATA_LENGTH + dataLength;
  byte txUpdate[length];
  tx.makePacket(txUpdate, coord.getSh(), coord.getSl(),
    coord.getMy(), dataType, data, dataLength);
    
  for(int x = 0; x < length; x++)
  {
    out.write(txUpdate[x]);
    Serial.print(txUpdate[x], HEX);
    Serial.print(", ");
  }
  Serial.println();
}
