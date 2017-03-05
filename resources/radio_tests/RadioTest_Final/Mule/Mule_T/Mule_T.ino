#include <RF22Mesh.h>
#include <SPI.h>
#include <RF22Router.h>
// Adding stuff from CommTest_WSN
#include <SdFat.h>
//#include <TinyGPS.h>
//#include <EEPROM.h>
//#include <Wire.h>
//#include <SFE_LSM9DS0.h>  
//#include <SdFat.h>
//#include "AndesiteCollect.h"
//#include "AndesiteData.h"
//#include "AndesiteOrbit.h"
//#include "AndesiteFile.h"
//#include "AndesiteRadio.h"
//#include "AndesiteWSN.h"
//#include "libandesite.h"
#include <RF22.h>
#include <string.h>
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

#define CLIENT_ADDRESS 0 //(1,3,5,7)
#define NUM_NODES 1
//#define SERVER_ADDRESS //(2,4,6,8) 

// initialize address for storage in EEPROM
int addr=0;
//int CLIENT_ADDRESS[]={1, 3, 5, 7};
//uint8_t SERVER_ADDRESS[8]={1,2,3, 4,5, 6,7, 8};
uint8_t SERVER_ADDRESS[NUM_NODES]={1};

int failed[NUM_NODES]; // Number of times sendtoWait fails
int sentCount[NUM_NODES];
int incomplete[NUM_NODES];
int successCount[NUM_NODES];

// Singleton instance of the radio
//int rf22

RF22Mesh radio(CLIENT_ADDRESS);

double successRate[22][NUM_NODES];
double rssi_avg[22];
double success_msg[22][NUM_NODES];
double fail_msg[22][NUM_NODES];

uint8_t response[] = "0000: And hello back to you"; 
uint8_t data[] = "0000: Hello World";
// Dont put this on the stack:
uint8_t buf[RF22_MAX_MESSAGE_LEN];
uint8_t len = sizeof(buf);
uint8_t from;
uint16_t timeout = 300;

SdFat SD;
#define ACDH_SD_PIN               9


File dataFile;

/*
void writeNewFile()
{
  //digitalWrite(4, HIGH);
  //digitalWrite(10, LOW);
  int z=0;
  char file_name[50];
  sprintf(file_name, "rate%d.txt", z);
  while (SD.exists(file_name))
  {
    Serial.print(file_name);
    Serial.println(" already exists, trying new file");
    z++;
    sprintf(file_name, "rate%d.txt", z);
  }
  Serial.print("creating file: ");
  Serial.println(file_name);
  File dataFile = SD.open(file_name, FILE_WRITE);
  //if(dataFile.available()){
    Serial.println("writing to file...");
    for (int i=0;i<22;i++){           
      dataFile.print(i);
      dataFile.print(',');
      dataFile.print(successRate[i]);
      dataFile.print(',');
      dataFile.print(rssi_avg[i]);
      dataFile.print(',');
      dataFile.print(success_msg[i]);
      dataFile.print(',');
      dataFile.print(fail_msg[i]);
      dataFile.println(',');
    }
  //}
  //else{
    // Serial.println("error in file created");
  //}
  dataFile.close();
  //digitalWrite(10,HIGH);
  //digitalWrite(4, LOW);
}
*/
void createNewFile()
{
  //digitalWrite(4, HIGH);
  //digitalWrite(10, LOW);
  int z=0;
  char file_name[50];
  sprintf(file_name, "rate%d.txt", z);
  while (SD.exists(file_name))
  {
    Serial.print(file_name);
    Serial.println(" already exists, trying new file");
    z++;
    sprintf(file_name, "rate%d.txt", z);
  }
  Serial.print("Creating file: ");
  Serial.println(file_name);
  File dataFile = SD.open(file_name, FILE_WRITE);
  //if(dataFile.available()){
    Serial.println("writing to file");
    dataFile.println(file_name);
  //}
  //else{
    //Serial.println("error in file created");
  //}
  dataFile.close();
  //digitalWrite(10,HIGH);
  //digitalWrite(4, LOW);
}


boolean setModulation(RF22::ModemConfigChoice config){
        if ( !radio.setModemConfig(config) ) {
          Serial.println("ERROR: setModemConfig failed 4.");
          while(1) {}
        }
}


void setup() 
{ 
    Serial.begin(115200);
    delay(500);

    //Serial.println("Started");
     // Setup SD card
   /*
   while ( !SD.begin(ACDH_SD_PIN) ) 
   {
        Serial.println("ERROR: SD card initialization failed.");
    }
    pinMode(10, OUTPUT);
    pinMode(ACDH_SS_PIN, OUTPUT);
    //createNewFile();
    */
    pinMode(4, OUTPUT);    
    digitalWrite(4, LOW);
    delay(100);

  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
    while (!radio.init())
    {
      Serial.println("rf22 init failed");
    }
    Serial.println("rf22 init success");  
  // Set transmission power
  radio.setTxPower(RF22_TXPOW_20DBM);
  radio.setRetries(1);
  radio.setTimeout(300);
  //pinMode(ledPin,OUTPUT);    //Debugging LED blink setup
}

RF22::ModemConfigChoice modulations[22] = {RF22::FSK_Rb2Fd5, RF22::FSK_Rb2_4Fd36, RF22::FSK_Rb4_8Fd45, RF22::FSK_Rb9_6Fd45, RF22::FSK_Rb19_2Fd9_6,
RF22::FSK_Rb38_4Fd19_6, RF22::FSK_Rb57_6Fd28_8, RF22::FSK_Rb125Fd125, RF22::GFSK_Rb2Fd5, RF22::GFSK_Rb2_4Fd36, RF22::GFSK_Rb4_8Fd45,
RF22::GFSK_Rb9_6Fd45, RF22::GFSK_Rb19_2Fd9_6, RF22::GFSK_Rb38_4Fd19_6, RF22::FSK_Rb57_6Fd28_8, RF22::FSK_Rb125Fd125, RF22::OOK_Rb2_4Bw335,
RF22::OOK_Rb4_8Bw335, RF22::OOK_Rb9_6Bw335, RF22::OOK_Rb19_2Bw335, RF22::OOK_Rb38_4Bw335, RF22::OOK_Rb40Bw335};

void loop()
{
  for(int state = 0; state<22;state++)
  {
    for(int node= 0;node<NUM_NODES;node++){
      failed[node] = 0;
      sentCount[node] = 0;
      incomplete[node] = 0;
      successCount[node] =0;
    }
    setModulation(modulations[state]);
    //Serial.println("Advance to the next modulation");
    int   rssi=0;
    uint8_t firstDigit = '0';
    uint8_t secondDigit = '0';
    uint8_t thirdDigit = '0';
    uint8_t fourthDigit = '0';
    int next = 0;
    while (!next)
    {
      //Serial.println("serial read"); 
      //successCount++;
      for(int node = 0; node<NUM_NODES;node++)
      {
        //Serial.print("Node ");
        //Serial.print(node);
        //Serial.print(":: ");
        if(successCount[node] > 999){
          int onesPlace = successCount[node]%10;
          fourthDigit = char(onesPlace+48);
          int tensPlace = ((successCount[node] - onesPlace)/10)%10;
          thirdDigit = char(tensPlace+48);
          int hundredPlace = ((((successCount[node] - onesPlace)/10)-tensPlace)/10)%10;
          secondDigit = char(hundredPlace+48);
          int thousPlace =  (((((successCount[node] - onesPlace)/10)-tensPlace)/10)-hundredPlace)/10;
          firstDigit = char(thousPlace+48);
        }
        if(successCount[node] > 99){
          firstDigit = '0';
          int onesPlace = successCount[node]%10;
          fourthDigit = char(onesPlace+48);
          int tensPlace = ((successCount[node] - onesPlace)/10)%10;
          thirdDigit = char(tensPlace+48);
          int hundredPlace = (((successCount[node] - onesPlace)/10)-tensPlace)/10;
          secondDigit = char(hundredPlace+48);
        }
        else if(successCount[node] > 9){
          firstDigit = '0';
          secondDigit = '0';
          int onesPlace = successCount[node]%10;
          fourthDigit = char(onesPlace+48);
          int tensPlace = (successCount[node] - onesPlace)/10;
          thirdDigit = char(tensPlace+48);
        }
        else{
          firstDigit = '0';
          secondDigit = '0';
          thirdDigit = '0';
          fourthDigit = char(successCount[node]+48);
        }
        data[0]= firstDigit;
        data[1] = secondDigit;
        data[2] = thirdDigit;
        data[3] = fourthDigit;
        int recieved = 0;
        while(!recieved && !next){
          sentCount[node]++;
          //Serial.println((const char*)data);
          if (radio.sendtoWait(data, sizeof(data), SERVER_ADDRESS[node])!= RF22_ROUTER_ERROR_NONE) 
          {
            //Serial.println("sendtoWait failed");
            //Serial.println("Tallying Failed Messages");
            failed[node]++; 
            if(failed[node]>=3){
              Serial.println("progressing to the next node");
              break;  
            }
          }
          else
          {
            if (radio.recvfromAckTimeout(buf, &len, timeout, &from))
            {  
              //Serial.println("String recieved!");
              String str_buf((const char*)buf);
              response[0] = firstDigit;
              response[1] = secondDigit;
              response[2] = thirdDigit;
              response[3] = fourthDigit;
              String str_res((const char*)response);
              //Serial.print("recieved string: ");
              //Serial.println(str_buf);
              //Serial.print("compared stirng: ");
              //Serial.println(str_res);
              if (!str_buf.equals(str_res))
              {
                incomplete[node]++;
              }
              else{
                //Serial.print("Successful message recieved: ");
                //Serial.println(str_buf);
                successCount[node]++; 
                if(successCount[node]>999){
                  Serial.println("waiting to continue to next modulation...");
                  while(Serial.read()!='c'){}
                  next = 1;
                }
                rssi = rssi + radio.rssiRead();
                recieved =1;
              }
            }
          }
          if(Serial.read() == 'c'){
            next = 1;
          }
        }
        if(Serial.read() == 'c'){
          next = 1;
        }  
      }
    }
    //rssi_avg[i] = rssi/count[i];
    Serial.print("For modulation ");
    Serial.println(state+1);
    for(int node= 0;node<NUM_NODES;node++){
      successRate[state][node] = ((successCount[node] / sentCount[node]) * 100);
      success_msg[state][node] = successCount[node];
      fail_msg[state][node] = failed[node];
      Serial.print("For node ");
      Serial.println(node+1);
      Serial.print("The total number of attempted transmissions: ");
      Serial.println(sentCount[node]);
      Serial.print("The number of failed transmissions without ack: ");
      Serial.println(failed[node]);
      Serial.print("The number of sucessful receptions and transmissions: ");
      Serial.println(successCount[node]);
      Serial.print("The number of incorrectly recieved messages: ");
      Serial.println(incomplete[node]);
    }
    if(state >= 21){
      Serial.println("*");
      while(1) {}
    }
  }
  /*
  Serial.print("Writing to SD card, filename: ");
  //writeNewFile();
  Serial.println("SD card written");
  Serial.println("*");
  */
}

