#include <RF22Mesh.h>
#include <SPI.h>
#include <RF22Router.h>
// Adding stuff from CommTest_WSN
#include <SdFat.h>
#include <TinyGPS.h>
#include <EEPROM.h>
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

// initialize address for storage in EEPROM
int addr=0;

//Setting up a Failed bit
int Failcheck=1;


// Singleton instance of the radio
RF22Mesh rf22(CLIENT_ADDRESS);

double successRate[22];
SdFat SD;
#define ACDH_SD_PIN               9


File dataFile;




void setup() 
{
  
    Serial.begin(115200);
    delay(500);
    
    
     // Setup SD card
   while ( !SD.begin(ACDH_SD_PIN) ) 
   {
        Serial.println("ERROR: SD card initialization failed.");
    }



    
    //load();
   
    //Serial.println("hello");
  
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
  // Set transmission power
  rf22.setTxPower(RF22_TXPOW_1DBM);
  rf22.setRetries(0);
  //pinMode(ledPin,OUTPUT);    //Debugging LED blink setup
}
uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RF22_MAX_MESSAGE_LEN];

uint8_t state = 1; 
double failed = 0; // Number of times sendtoWait fails
double failed2=0; //Number of times sendtoWait fails while trying to connect 
double count = 0;  // Number of time sendtoWait succeeds












void loop()
{
  Serial.println("hello");
  //digitalWrite(ledPin, LOW);
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
     case 23:
        
        Serial.println("Writing to EEPROM");
        //save();
       
       dataFile = SD.open("rate1.txt", FILE_WRITE);
       if(SD.exists("rate1.txt"))
       {
         for (int i=0;i<22;i++)
         {
           
      
            dataFile.println(successRate[i]);
            
          
         }
         
       }
        dataFile.close();
        
        
        Serial.println("EEPROM written");
        //load();
        state++;
        break;
    default: 
      Serial.println("Finishing Testing all Modulations");
      while(1){}

      //digitalWrite(ledPin,HIGH);  
      
      
  }
    
    //Debugging Led Blink setup
   
    
    //Serial.println("Sending to rf22_datagram_server");
    
    // Send a message to rf22_server
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
      if (failed2>=2)
      {
         Failcheck=0; 
      }
    
      if (failed >= 10) 
      {
        Serial.print("Message has failed 5 times with Modulation: ");
        Serial.println(state);
        Serial.println("Please press 'c' to continue when ready: ");
        Serial.end();    // Ends the serial communication once all data is received
        Serial.begin(115200); 
        while (Serial.available() == 0) {/*Serial.println("No character");*/}
        if (Serial.available() > 0) {
          char xx = Serial.read();
          while (xx != 'c') {  
            //Serial.println("Really man? Press 'c': ");
            char xx = Serial.read();
          }
          
          Failcheck=1;
          //state++;
          count=0; 
          failed=0;
          failed2=0;
          Serial.println("Changing States");
          state++;
        }
        
        //while(1) {};
      }
    }
    else
    {
      count++;
      Failcheck=0;
      if (count >= 25) {
        Serial.println(count);
        Serial.println(failed);
        successRate[state] = ((count / (failed + count)) * 100);
        Serial.print("The transmission rate for modulation ");
        Serial.print(state);
        Serial.print(" is ");
        Serial.println(successRate[state]);
        count = 0;
        failed = 0;
        state++;
        
      }
      
      // Now wait for a reply from the server
      // Serial.println(rf22.lastRssi(), HEX); // of the ACK
//      uint8_t len = sizeof(buf);
//      uint8_t from;   
//      if (rf22.recvfromAckTimeout(buf, &len, 2000, &from))
//      {
//        Serial.print("got reply from : 0x");
//        Serial.print(from, HEX);
//        Serial.print(": ");
//        Serial.println((char*)buf);
//      }
//      else
//      {
//        Serial.println("No reply, is rf22_datagram_server running?");
//      }
    }
    //delay(500);
  }
}


