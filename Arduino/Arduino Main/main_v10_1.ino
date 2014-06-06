#include <SoftwareSerial.h>

#include "Constants.h"

#include "Common.h"

#include "Node.h"

#include "Packet.h"

#include "XbeeInfo.h"

#include "Tx.h"

#include "RxExplicit.h"

#include "RadioStream.h"


// create all the variables for testing
RadioStream stream(2, 3);

//Coordinator w/ low and high address 0 & my address 0xfffe
XbeeInfo coord(0, 0, 0xfffe);

//Default (0,0xffff,0xfffe)
XbeeInfo radio2;

//?
RxExplicit rx;

Packet txPacket, sensorPacket;

Packet * rxPacket;

bool radio2Found = false, sentAddress = false, readyToSync = false,
     inSync = false, sendData = false;

byte counter = 0;

byte dummyData[] = {1, 2, 3, 4, 5};



void setup()

{
  //19200 bits per sec
  Serial.begin(19200);
}


void loop()
{
  // sync the clocks when readyToSync is true
  if(readyToSync)
  {
    inSync = true;
    readyToSync = false;
    Serial.println("syncing");
  }
  
  if(inSync)
  {
    Tx().makeNetworkPacket(txPacket, coord, SYNC_CONFIRMATION);
    stream.send(&txPacket);
    inSync = false;
    sendData = true;
    Serial.println("sending sync confirmation");
  }
  
  if(sendData)
  {
    Tx().makeNetworkPacket(sensorPacket, radio2, SENSOR_READING, counter, dummyData, 4);
    stream.send(&sensorPacket);
  }
  
  // get the packet from the stream
  rxPacket = stream.getNextPacket();
  
  // if there is an available packet print tester methods
  if(rxPacket != NULL)
  {
    printPacket(rxPacket);
    Serial.print("Frame Type: ");
    Serial.println(Common().getFrameType(*rxPacket), HEX);
    Serial.print("Data Type: ");
    Serial.println(rx.getDataType(*rxPacket), HEX);
    Serial.print("SH: ");
    Serial.println(rx.getSh(*rxPacket), HEX);
    Serial.print("SL: ");
    Serial.println(rx.getSl(*rxPacket), HEX);
    Serial.print("MY: ");
    Serial.println(rx.getMy(*rxPacket), HEX);
    if(Common().getFrameType(*rxPacket) == RX_EXPLICIT)
    {
      int dataType = rx.getDataType(*rxPacket);
      if(dataType == RADIO_2_ADDRESS)
      {
        radio2Found = true;
        radio2.setSh(rx.getSh(*rxPacket));
        radio2.setSl(rx.getSl(*rxPacket));
        radio2.setMy(rx.getMy(*rxPacket));
        if(radio2Found && !sentAddress)
        {
          Tx().makeNetworkPacket(txPacket, coord, RECEIVED_RADIO_2_ADDRESS);
          stream.send(&txPacket);
          sentAddress = true;
        }
        //Serial.println("sent address");
      }
      else if(dataType == RECEIVED_ADDRESS)
      {
        readyToSync = true;
        Serial.println("ready to sync");
      }
    }
  }
  counter++;
}

void printPacket(Packet * p)
{
  for(int x = 0; x < p->length(); x++)
  {
    Serial.print(p->get(x), HEX);
    Serial.print(", ");
  }
  Serial.println();
}

