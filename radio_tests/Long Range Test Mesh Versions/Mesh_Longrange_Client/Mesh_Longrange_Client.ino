#include <RF22Mesh.h>
#include <SPI.h>
#include <RF22Router.h>

#include <SdFat.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <SFE_LSM9DS0.h> 
#include <SdFat.h>
#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "AndesiteRadio.h"
#include "AndesiteWSN.h"
#include "libandesite.h"
#include <RF22.h>
#include <string.h>

SdFat SD;
TinyGPSPlus GPS;

//Initiate 9 axis IMU
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 

AndesiteRadio _Radio;
AndesiteOrbit _Orbit;
AndesiteFile _File;
AndesiteWSN WSN;

#define ACDH_SS_PIN          53
#define ACDH_SD_PIN          9
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

//Setting up a failbit
int failcheck=1;

//Singleton instance of the radio
RF22Mesh rf22(CLIENT_ADDRESS);

int prefails[8]={0,0,0,0,0,0,0,0};
int fails[8]={0,0,0,0,0,0,0,0};
int sends[8]={0,0,0,0,0,0,0,0};
File dataFile;

void setup()
{
  Serial.begin(115200);
  delay(500);
  
  //Setup SD card
  while(!SD.begin(ACDH_SD_PIN))
  {
    Serial.println("ERROR: SD card initialization failed.");
  }
  
  pinMode(ACDH_SS_PIN, OUTPUT);
  pinMode(4, OUTPUT);    
  digitalWrite(4, LOW);
  delay(100);
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  while (!rf22.init())
  {
    Serial.println("RF22 init failed");
  }
  Serial.println("init success"); 
 //Set modulation (determined by close range test)
 rf22.setModemConfig(RF22::FSKRb38_4Fd19_6)
 delay(500);
}

uint8_t data[]="Hello World!";
uint8_t buf[RF22_MAX_MESSAGE_LEN];

uint8_t state=1;
int failed=0;                //Number of times sendtoWait fails
int failed2=0;               //Number of times sendtoWait fails while waiting to connect
int count=0;                 //Number of times sendtoWait succeeds

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
          dataFile = SD.open("prefails1.txt", FILE_WRITE);
          if(SD.exists("prefails1.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(prefails[i]);
            }
          }
          dataFile.close();
          dataFile = SD.open("fails1.txt", FILE_WRITE);
          if(SD.exists("fails1.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(fails[i]);
            }
          }
          dataFile.close();
          dataFile = SD.open("sends1.txt", FILE_WRITE);
          if(SD.exists("sends1.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(sends[i]);
            }
          }
          dataFile.close();
          break;
          case 2:
          dataFile = SD.open("prefails2.txt", FILE_WRITE);
          if(SD.exists("prefails2.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(prefails[i]);
            }
          }
          dataFile.close();
          dataFile = SD.open("fails2.txt", FILE_WRITE);
          if(SD.exists("fails2.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(fails[i]);
            }
          }
          dataFile.close();
          dataFile = SD.open("sends2.txt", FILE_WRITE);
          if(SD.exists("sends2.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(sends[i]);
            }
          }
          dataFile.close();
          case 3:
          dataFile = SD.open("prefails3.txt", FILE_WRITE);
          if(SD.exists("prefails3.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(prefails[i]);
            }
          }
          dataFile.close();
          dataFile = SD.open("fails3.txt", FILE_WRITE);
          if(SD.exists("fails3.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(fails[i]);
            }
          }
          dataFile.close();
          dataFile = SD.open("sends3.txt", FILE_WRITE);
          if(SD.exists("sends3.txt"))
          {
            for (int i=0;i<8;i++)
            {
               dataFile.println(sends[i]);
            }
          }
          dataFile.close();
        }
        
       
       Serial.println("SD card written");
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
            prefails[8]={0,0,0,0,0,0,0,0};
            fails[8]={0,0,0,0,0,0,0,0};
            sends[8]={0,0,0,0,0,0,0,0};
            count=0;
            failed=0
            failed2=0;
            
          }
          else 
          {
            Serial.println("Finished all testing");
          }
          while(1){}
          
    }
    if (rf22.sendtoWait(data, sizeof(data), SERVER_ADDRESS)!= RF22_ROUTER_ERROR_NONE) 
    {
      Serial.println("sendtoWait failed");
      if (!Failcheck)
      {
        Serial.println("Tallying Failed Messages");
        failed++;
      }
      else 
      {
        Serial.println("Waiting to connect");
        failed2++;
      }
      if (failed2>=3)    // I need to tailor this so i know exactly how long between states
      {
        Failcheck=0;
      }
      if (failed>=10)
      {
        Serial.print("Message has failed 15 times with power scheme: ");
        Serial.println(state);
        state++
        Failcheck=1;
        count=0;
        failed=0
        failed2=0;
        Serial.println("Changing States");
      }
    }
    else
    {
      count++;
      Failcheck=0;
      if (count>=10)
      {
        Serial.print("State: ");
        Serial.println(state);
        Serial.print("Preconnection fails: ");
        Serial.println(failed2);
        Serial.print("Failed
        prefails[state]=failed2;
        fails[state]=failed;
        sends[state]=count;
        state++;   
      }
    }  
  }
}








