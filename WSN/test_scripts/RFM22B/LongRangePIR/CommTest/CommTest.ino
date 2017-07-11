#include <RF22Mesh.h>
//#include <SPI.h>
#include <RF22Router.h>
// Adding stuff from CommTest_WSN
//#include <SdFat.h>
//#include <TinyGPS.h>
//#include <Wire.h>
//#include <SFE_LSM9DS0.h> 
//#include "AndesiteCollect.h"
//#include "AndesiteData.h"
//#include "AndesiteOrbit.h"
//#include "AndesiteFile.h"
//#include "AndesiteRadio.h"
//#include "AndesiteWSN.h"
//#include "libandesite.h"
#include <RF22.h>
//SdFat SD;
//TinyGPSPlus GPS;
// Initiate 9-axis IMU 
//#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
//#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
//LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 
//AndesiteRadio _Radio;
//AndesiteOrbit _Orbit;
//AndesiteFile _File;
//AndesiteWSN WSN;
#define ACDH_SS_PIN               53
#define CLIENT_ADDRESS 0 
#define SERVER_ADDRESS 1
#define MOD_NUM 1
double successRate[MOD_NUM];
int rssi_avg[100];
double success_msg[MOD_NUM];
double fail_msg[MOD_NUM];
// Singleton instance of the radio
RF22Mesh rf22(SERVER_ADDRESS);
uint8_t data[] = "0000: Hello World";
uint8_t response[] = "0000: And hello back to you"; 
// Dont put this on the stack:
uint8_t buf[RF22_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);
uint8_t from;
uint16_t timeout = 500;
int ledPin=13;
boolean setModulation(RF22::ModemConfigChoice config){
        if ( !rf22.setModemConfig(config) ) {
          Serial.println("ERROR: setModemConfig failed 4.");
    while(1) {}
        }
}
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
  while (!rf22.init())
   {
      Serial.println("rf22 init failed");
   }
    Serial.println("rf22 init success");
  
  // Set transmission power
  rf22.setTxPower(RF22_TXPOW_20DBM);
  /* while (Serial.read()!='c'){
    Serial.println("Wait");
    delay(200);
    }
  */
  
  for(int i = 0; i < 100; i++){
    rssi_avg[i] = 0;
  }
  
  delay(5000);
}
//Setting up an incomplete counter
int incomplete=0;
int failed = 0;
uint8_t state = 1; 
int successCount = 0;
int sentCount = 0;
RF22::ModemConfigChoice modulations[MOD_NUM] = {RF22::FSK_Rb38_4Fd19_6};
//RF22::ModemConfigChoice modulations[MOD_NUM] = {RF22::GFSK_Rb38_4Fd19_6, RF22::GFSK_Rb57_6Fd28_8};
/*
RF22::ModemConfigChoice modulations[MOD_NUM] = {RF22::FSK_Rb2Fd5, RF22::FSK_Rb2_4Fd36, RF22::FSK_Rb4_8Fd45, RF22::FSK_Rb9_6Fd45, RF22::FSK_Rb19_2Fd9_6,
RF22::FSK_Rb38_4Fd19_6, RF22::FSK_Rb57_6Fd28_8, RF22::FSK_Rb125Fd125, RF22::GFSK_Rb2Fd5, RF22::GFSK_Rb2_4Fd36, RF22::GFSK_Rb4_8Fd45,
RF22::GFSK_Rb9_6Fd45, RF22::GFSK_Rb19_2Fd9_6, RF22::GFSK_Rb38_4Fd19_6, RF22::GFSK_Rb57_6Fd28_8, RF22::GFSK_Rb125Fd125, RF22::OOK_Rb2_4Bw335,
RF22::OOK_Rb4_8Bw335, RF22::OOK_Rb9_6Bw335, RF22::OOK_Rb19_2Bw335, RF22::OOK_Rb38_4Bw335, RF22::OOK_Rb40Bw335};
*/
void loop()
{    
    // Wait for a message addressed to us from the client
  for(int i = 0; i<MOD_NUM;i++){
    setModulation(modulations[i]);
    //uint8_t len = sizeof(buf);
    uint8_t from;
    int rssi_calc = 0;
    int rssi_val = 0;
    int rssi_counter = 0;
    //Serial.println("Advance to the next modulation");
    while (Serial.read()!='c')
    {
      if (rf22.recvfromAckTimeout(buf, &len, timeout, &from))
      {
        String str_buf((const char*)buf);
        data[0]= buf[0];
        data[1] = buf[1];
        data[2] = buf[2];
        data[3] = buf[3];
        String str_data((const char*)data);
        if (!str_buf.equals(str_data))
        {
          incomplete++;
          //Serial.print("recieved string: ");
          //Serial.println(str_buf);
          //Serial.print("compared stirng: ");
          //Serial.println(str_data);
        }
        else{
          //count++;
          //Serial.println("Successful message recieved!");
          //Serial.print("Successful message recieved: ");
          //Serial.println(str_buf);
          response[0]= buf[0];
          response[1] = buf[1];
          response[2] = buf[2];
          response[3] = buf[3];
          String str_res((const char*)response);
          Serial.print("sending response ");
          Serial.print(char(response[0]));
          Serial.print(char(response[1]));
          Serial.print(char(response[2]));
          Serial.print(char(response[3]));
          Serial.print(":: RSSI: ");
          if (rf22.sendtoWait(response, sizeof(response), CLIENT_ADDRESS)!= RF22_ROUTER_ERROR_NONE){
            failed++;
          }
          else{
            successCount++;
      rssi_val=rf22.lastRssi();
      Serial.println(rssi_val);
      rssi_calc += rssi_val;
            //Serial.print("count: ");
            //Serial.println(successCount);
          }
        }
      
        if (successCount%10 == 0)
        {
          rssi_avg[rssi_counter] = (rssi_calc)/10;
          rssi_counter++;
          rssi_calc = 0;
          //Serial.print("rssi value: ");
          //Serial.println(rssivalue);
        }  
       
       //Serial.print("got request from : 0x");
       //Serial.print(from, HEX);
       //Serial.print(": ");
       //Serial.println((char*)buf);
       // Send a reply back to the originator client
       //if (!rf22.sendtoWait(data, sizeof(data), from))
       //  Serial.println("sendtoWait failed");
      }
    }
    Serial.print("For modulation: ");
    Serial.println(state);
    Serial.print("The number of failed transmission messages without ack: ");
    Serial.println(failed);
    Serial.print("The number of successfull receptions and transmissions: ");
    Serial.println(successCount);
    Serial.print("The number of incorrectly recieved messages: ");
    Serial.println(incomplete);
  Serial.print("The rssi averages: ");
  for(int itr = 0;itr<99;itr++){
    Serial.print(rssi_avg[itr]);
    Serial.print(" ");
  }
  Serial.println(rssi_avg[99]);
  for(int i = 0; i < 100; i++){
    rssi_avg[i] = 0;
  }
    incomplete=0;
    failed = 0;
    successCount=0;
    state++;
  }
  //digitalWrite(ledPin,HIGH);
  //Serial.println("Finishing Testing all Modulations");
  Serial.println("*");
  while(1) {}
}