#include <SPI.h>
// Adding stuff from CommTest_WSN
#include <SdFat.h>
#include <RF22.h>
#include <string.h>
#include <RF22ReliableDatagram.h>
#include <RF22Router.h>
#include "libandesite.h"
#define ACDH_SS_PIN               53

#define CLIENT_ADDRESS 1
#define NUM_NODES 5                                                     
#define SERVER_ADDRESS 2 

RF22ReliableDatagram RF22(CLIENT_ADDRESS);


uint8_t response[] = "TransferReady"; 
uint8_t data[] = "::ReadyTesting";
// Dont put this on the stack:
//testing
uint8_t from;
uint16_t timeout = 300;

SdFat SD;
#define ACDH_SD_PIN               9
const char * file_name = "test.txt";

File dataFile;

void setup() 
{ 
    Serial.begin(115200);
    delay(500);
    
    //Serial.println("Started");
     // Setup SD card
   
   while ( !SD.begin(ACDH_SD_PIN) ) 
   {
        Serial.println("ERROR: SD card initialization failed.");
    }
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    delay(100);
	      // set up SPI slave select pins
	      pinMode(RF_CS_PIN, OUTPUT);
	      pinMode(SD_CS_PIN, OUTPUT);
	      pinMode(ADS_CS_PIN, OUTPUT);
	      pinMode(RF_SHDN_PIN, OUTPUT);
	      pinMode(GPS_ENABLE, OUTPUT);
	      
	      Serial.print("*****Setting Chip Selects LOW*****");
	      digitalWrite(RF_SHDN_PIN, LOW);
	      digitalWrite(RF_CS_PIN, LOW);
	      digitalWrite(ADS_CS_PIN, HIGH);
	      digitalWrite(SD_CS_PIN, HIGH);
	      digitalWrite(GPS_ENABLE, HIGH);
	      
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
    while (!RF22.init())
    {
      Serial.println("rf22 init failed");
    }
    Serial.println("rf22 init success");  
	// Set transmission rate to 38.4 kbs
	if ( !RF22.setModemConfig(RF22::FSK_Rb38_4Fd19_6) ) {
		Serial.println("::setModemConfigFailed");
		return 2;
	}
	RF22.setTxPower(RF22_TXPOW_20DBM);
	RF22.setRetries(1);
	RF22.setTimeout(50);
	
	if(SD.exists(file_name)){
		SD.remove(file_name);
	}
	Serial.print("Creating file: ");
	Serial.println(file_name);
}

void loop()
{
	digitalWrite(RF_CS_PIN, LOW);
	digitalWrite(SD_CS_PIN, HIGH);
	uint8_t buf[RF22_MAX_MESSAGE_LEN];
	uint8_t len =sizeof(buf);
	if (RF22.recvfromAck(buf, &len, &from))
	{
		String str_buf((const char*)buf);
		Serial.print("recieved string: ");
		Serial.println(str_buf);
		String str_data((const char*)data);
		if (!str_buf.equals(str_data))
		{
			digitalWrite(RF_CS_PIN, HIGH);
			digitalWrite(SD_CS_PIN, LOW);
			File dataFile = SD.open("test.txt", FILE_WRITE);
			if (dataFile)
			{
				dataFile.println(str_buf);
				dataFile.close();
				Serial.println("Not equal to ready command - writing to file");
			}
			else{
				Serial.println("Failed to Enter SD card file");
			}
			digitalWrite(RF_CS_PIN, LOW);
			digitalWrite(SD_CS_PIN, HIGH);
		}
		else{
			Serial.println("WSN ready message recieved! Send Transfer command");
			int resp = 0;
			while(resp == 0)
			{
				if(RF22.sendtoWait(response, sizeof(response), SERVER_ADDRESS)!= RF22_ROUTER_ERROR_NONE)
				{
					Serial.println("sendtoWait failed - no Ack from WSN");
				}
				else
				{
					Serial.println("Waiting for WSN to resond to Transfer Command with data");
				}
				uint8_t buf[RF22_MAX_MESSAGE_LEN];
				uint8_t len =sizeof(buf);
				if (RF22.recvfromAckTimeout(buf, &len, timeout, &from)){
					String str_buf((const char*)buf);
					Serial.print("recieved string: ");
					Serial.println(str_buf);
					String str_data((const char*)data);
					if (!str_buf.equals(str_data))
					{
						resp = 1;
						digitalWrite(RF_CS_PIN, HIGH);
						digitalWrite(SD_CS_PIN, LOW);
						File dataFile = SD.open("test.txt", FILE_WRITE);
						Serial.println("Not equal to ready command - writing to file");
						if (dataFile)
						{
							dataFile.println(str_buf);
							dataFile.close();
							
						}
						else{
							Serial.println("Failed to Enter SD card file");
						}
						digitalWrite(RF_CS_PIN, LOW);
						digitalWrite(SD_CS_PIN, HIGH);
					}
				}
			}
		}
	}
}

