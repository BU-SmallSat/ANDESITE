#include <RF22Mesh.h>
#include <SPI.h>
#include <RF22Router.h>
// Adding stuff from CommTest_WSN
#include <SdFat.h>
#include <TinyGPS.h>

#include <Wire.h>
#include <SFE_LSM9DS0.h> 

#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "AndesiteRadio.h"
#include "AndesiteWSN.h"
#include "libandesite.h"
#include <RF22.h>
SdFat SD;
TinyGPSPlus GPS;

// Initiate 9-axis IMU 
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 

AndesiteRadio _Radio;
AndesiteOrbit _Orbit;
AndesiteFile _File;
AndesiteWSN WSN;

#define ACDH_SS_PIN               53
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio
RF22Mesh rf22(SERVER_ADDRESS,9);

//Setting up an incomplete counter
int incomplete=0;
int ledPin=13;
int failcount=0;


void setup() 
{
    Serial.begin(115200);
    delay(500);
    //Serial.println("hello");
  
    pinMode(ACDH_SS_PIN, OUTPUT);
    pinMode(4, OUTPUT);    
    digitalWrite(4, LOW);
    delay(100);

  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  if (!rf22.init())
    Serial.println("rf22 init failed");
  else
    Serial.println("rf22 init success");
  
  // Set transmission power
  rf22.setTxPower(RF22_TXPOW_1DBM);
  
  delay(5000);
  
}
//uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RF22_MAX_MESSAGE_LEN];

uint8_t state = 1; 
uint8_t count = 0;
void loop()
{
  //digitalWrite(ledPin,LOW);
  while (1)
  {
    switch (state) {
    case 1:
      if ( !rf22.setModemConfig(RF22::FSK_Rb2Fd5) ) {
        Serial.println("ERROR: setModemConfig failed 1.");
	while(1) {}
      }
      break;
    case 2:
      if ( !rf22.setModemConfig(RF22::FSK_Rb2_4Fd36) ) {
        Serial.println("ERROR: setModemConfig failed 2.");
	while(1) {}
      }
      break;
    case 3:
      if ( !rf22.setModemConfig(RF22::FSK_Rb4_8Fd45) ) {
        Serial.println("ERROR: setModemConfig failed 3.");
	while(1) {}
      }
      break;
    case 4:
      if ( !rf22.setModemConfig(RF22::FSK_Rb9_6Fd45) ) {
        Serial.println("ERROR: setModemConfig failed 4.");
	while(1) {}
      }
      break;
    case 5:
      if ( !rf22.setModemConfig(RF22::FSK_Rb19_2Fd9_6) ) {
        Serial.println("ERROR: setModemConfig failed 5.");
	while(1) {}
      }
      break;
    case 6:
      if ( !rf22.setModemConfig(RF22::FSK_Rb38_4Fd19_6) ) {
        Serial.println("ERROR: setModemConfig failed 6.");
	while(1) {}
      }
      break;
    case 7:
      if ( !rf22.setModemConfig(RF22::FSK_Rb57_6Fd28_8) ) {
        Serial.println("ERROR: setModemConfig failed 7.");
	while(1) {}
      }
      break;
    case 8:
      if ( !rf22.setModemConfig(RF22::FSK_Rb125Fd125) ) {
        Serial.println("ERROR: setModemConfig failed 8.");
	while(1) {}
      }
      break;
    case 9:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb2Fd5) ) {
        Serial.println("ERROR: setModemConfig failed 9.");
	while(1) {}
      }
      break;
    case 10:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb2_4Fd36) ) {
        Serial.println("ERROR: setModemConfig failed 10.");
	while(1) {}
      }
      break;
    case 11:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb4_8Fd45) ) {
        Serial.println("ERROR: setModemConfig failed 11.");
	while(1) {}
      }
      break;
    case 12:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb9_6Fd45) ) {
        Serial.println("ERROR: setModemConfig failed 12.");
	while(1) {}
      }
      break;
    case 13:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb19_2Fd9_6) ) {
        Serial.println("ERROR: setModemConfig failed 13.");
	while(1) {}
      }
      break;
    case 14:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb38_4Fd19_6) ) {
        Serial.println("ERROR: setModemConfig failed 14.");
	while(1) {}
      }
      break;
    case 15:
      if ( !rf22.setModemConfig(RF22::GFSK_Rb57_6Fd28_8) ) {
        Serial.println("ERROR: setModemConfig failed 15.");
	while(1) {}
      }
      break;
    case 16:  
      if ( !rf22.setModemConfig(RF22::GFSK_Rb125Fd125) ) {
        Serial.println("ERROR: setModemConfig failed 16.");
	while(1) {}
      }
      break;
    case 17:
      if ( !rf22.setModemConfig(RF22::OOK_Rb2_4Bw335) ) {
        Serial.println("ERROR: setModemConfig failed 18.");
	while(1) {}
      }
      break;
    case 18:
      if ( !rf22.setModemConfig(RF22::OOK_Rb4_8Bw335) ) {
        Serial.println("ERROR: setModemConfig failed 19.");
	while(1) {}
      }
      break;
    case 19:
      if ( !rf22.setModemConfig(RF22::OOK_Rb9_6Bw335) ) {
        Serial.println("ERROR: setModemConfig failed 20.");
	while(1) {}
      }
      break;
    case 20:
      if ( !rf22.setModemConfig(RF22::OOK_Rb19_2Bw335) ) {
        Serial.println("ERROR: setModemConfig failed 21.");
	while(1) {}
      }
      break;
    case 21:
      if ( !rf22.setModemConfig(RF22::OOK_Rb38_4Bw335) ) {
        Serial.println("ERROR: setModemConfig failed 22.");
	while(1) {}
      }
      break;
    case 22:
      if ( !rf22.setModemConfig(RF22::OOK_Rb40Bw335) ) {
        Serial.println("ERROR: setModemConfig failed 23.");
	while(1) {}
      }
      break;
    default: 
      //digitalWrite(ledPin,HIGH);
      Serial.println("Finishing Testing all Modulations");
      while(1) {}
  }
    
    
    
    // Wait for a message addressed to us from the client
    
    uint8_t len = sizeof(buf);
    uint8_t from;
    
    if (rf22.recvfromAck(buf, &len, &from))
    {
      count++;
      failcount=0;
      if ((const char*)buf!="Hello World!")
      {
        incomplete++;
      }
      
      
      if (count >= 25) {
        count = 0;
         state++;
        //Serial.println(incomplete);
        Serial.print("Changing State: ");
        Serial.println(state);  
      }
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
      // Send a reply back to the originator client
      //if (!rf22.sendtoWait(data, sizeof(data), from))
      //  Serial.println("sendtoWait failed");
    }
    else{
      //Serial.println("no message");
      failcount++;
      //Serial.print("Fail number: ");
     // Serial.println(failcount);
     // Serial.print("\n");
      if (failcount>10000)
      {
        if(state != 1){
          Serial.print("Modulation for state: ");
          Serial.print(state);
          Serial.println(" failed.");
          Serial.println("Please press 'c' to continue when ready: ");
          
          while (Serial.available() == 0) {/*Serial.println("No character");*/}
          if (Serial.available() > 0) {
            char xx = Serial.read();
            while (xx != 'c') {  
              //Serial.println("Really man? Press 'c': ");
              char xx = Serial.read();
            }
          
            //state++;
            count=0;
            failcount=0; 
            Serial.println("Changing States");
            Serial.end();    // Ends the serial communication once all data is received
            Serial.begin(115200); 
            delay(1000);
            state++;
          }
        }
      }
      
    }
  }
}

