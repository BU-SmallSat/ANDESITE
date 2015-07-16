#include <RF22Mesh.h>
#include <SPI.h>
#include <Rf22Router.h>
#include <SdFat.h>
#include <TinyGPS.h>
#include<Wire.h>
#include <SFE_LSM9DS0.h> 

#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "AndesiteRadio.h"
#include "AndesiteWSN.h"
#include "libadnesite.h"
#include <RF22.h>
SdFat SD;
TinyGPSPlus GPS;

//initiate 9-axis IMU 
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 

AdnesiteTadio _Radio;
AndesiteOrbit _Orbit;
AndesiteFile _File_
AndesiteWSN WSN; 

#define ACDH_SS_PIN            53
#define ACDH_SD_PIN          9
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

//singleton instance of the radio
RF22Mesh rf22(SERVER_ADDRESS);

//Initializing incomplete message counter
int incomplete=0;
int failcount;

void setup()
{
  Serial.begin(115200);
  delay(500);
  
  while(!SD.begin(ACDH_SD_PIN))
  {
    Serial.println("ERROR: SD card initialization failed.");
  }
  
  pinMode(ACDH_SS_PIN, OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(4, LOW);
  delay(100);
  
  //defaults after init are 434.0 MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  while(!rf22.init())
  {
    Serial.println("rf22 init failed");
  }
  Serial.println("rf22 init success");
  
  //Set modulation (determined by close range test)
  rf22.setModemConfig(RF22::FSK_Rb38_4Fd19_6)

  
  delay(500);
  
}

uint8_t buf[RF22_MAX_MESSAGE_LEN];
uint8 _t state=1;
uint8_t count=0;
uint8_t trial=1;

void loop()
{
  while (1)
  {
    switch (state)
    {
      case 1:
        if (!rf22.setTxPower(RF22_TXPOW_20DBM);
        {
          Serial.println("Error: setPower failed 1.");
          while(1){}
        }
        state++;
      break;
      case 2:
        if (!rf22.setTxPower(RF22_TXPOW_17DBM);
          {
            Serial.println("Error: setPower failed 2.");
            while(1){}
          }
        state++;
      break:
      case 3:
        if (!rf22.setTxPower(RF22_TXPOW_14DBM);
          {
            Serial.println("Error: setPower failed 3.");
            while(1){}
          }
        state++;
      break;
      case 4:
        if (!rf22.setTxPower(RF22_TXPOW_11DBM);
          {
            Serial.println("Error: setPower failed 4.");
            while(1){}
          }
        state++;
      break;
      case 5:
        if (!rf22.setTxPower(RF22_TXPOW_8DBM);
          {
            Serial.println("Error: setPower failed 5.");
            while(1){}
          }
        state++;
      break;
      case 6:
        if (!rf22.setTxPower(RF22_TXPOW_5DBM);
          {
            Serial.println("Error: setPower failed 5.");
            while(1){}
          }
        state++;
      break;
      case 7:
        if (!rf22.setTxPower(RF22_TXPOW_2DBM);
          {
            Serial.println("Error: setPower failed 5.");
            while(1){}
          }
        state++;
      break;
      case 8:
        if (!rf22.setTxPower(RF22_TXPOW_1DBM);
          {
            Serial.println("Error: setPower failed 5.");
            while(1){}
          }
        state++;
      break;
      case 9:
        Serial.println("Writing to SD card");
        switch (trial)
        {
          case 1:
          dataFile = SD.open("resultst1.txt", FILE_WRITE);
          if(SD.exists("resultst1.txt"))
          {
            for (int i=0;i<9;i++)
            {
               dataFile.println(incompletes[i]);
            }
          }
          dataFile.close();
          break;
          case 2:
          dataFile = SD.open("resultst2.txt", FILE_WRITE);
          if(SD.exists("resultst2.txt"))
          {
            for (int i=0;i<9;i++)
            {
               dataFile.println(incompletes[i]);
            }
          }
          dataFile.close();
          break;
          case 3:
          dataFile = SD.open("resultst3.txt", FILE_WRITE);
          if(SD.exists("resultst3.txt"))
          {
            for (int i=0;i<9;i++)
            {
               dataFile.println(incompletes[i]);
            }
          }
          dataFile.close();
          break;
          
        }
        
       
       Serial.println("EEPROM written");
        //load();
        state++;
        break;
        default:
          Serial.print("Testing all powers trial ");
          Serial.println(trial);
          while(1){}
          
          if (trial<3)
          {
            state=1;
            trial++;
          }
          else 
          {
            Serial.println("Finished all testing");
          }
          while(1){}
    }
    
    //Wait for a message addressed to us from the client
    
    uint8_t len=sizeof(buf);
    uint8_t from;
    int rssivalue[10];                            //initializing the rssivalue array
    int placeholder=0;                            //initializing a placeholder in order to calculate the mean of the rssivalues.
    int meanrssi[8];                              //initializing the mean rssi array
    int incompletes[8];                           //initializing the number of incompletes per power
    if (rf22.recvfromAck(buf, &len, &from))
    {
      count++;
      failcount=0;
      if ((const char*)buf!="Hello World!")
      {
        incomplete++;
      }
      rssivalue[count]=rf22.lastRssi();
      Serial.println(rssivalue[count]);
      
      if (count>=10)                              //This code will run once 10 messages are successfully sent.                                 
      {
        for (int i=0;i<count;i++)                 //For loop to sum the rssivalues;
        {
          placeholder=placeholder+rssivalue[i];
        }
        meanrssi[state-1]=placeholder/10;         //calculating mean rssivalue and storing it
        incompletes[state-1]=incomplete;          //storing the number of incompletes for this power scheme
        incomplete=0;
        Serial.print("Changing state from ");
        Serial.print(state);
        Serial.print(" to ");
        state++;
        Serial.println(state);
      }
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      //Serial.println("no message");
      failcount++;
      if (failcount>60000)                        // Testing and fine tuning is needed such that the exact time 
      {                                           // it takes to reach a specified failcount is known
        if(state != 0)
        {
          Serial.print("Power scheme for state: ");
          Serial.print(state);
          Serial.println("failed");
          Serial.print("Changing to state: ");
          state++;
          Serial.println(state);
         count=0;
         failcount=0
         delay(1000);
         Serial.println("State changed");
        }
      }        
    }
  }
}     
      


