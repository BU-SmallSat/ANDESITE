/*
#include "AndesiteWSN.h"
#include "AndesiteData.h"
#include "AndesiteCollect.h"
#include "AndesiteRadio.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "libandesite.h"

#include <SPI.h>
*/
#include <SdFat.h>
SdFat SD;
//File handle;
#define ACDH_SD_PIN               9


void setup() {
  Serial.begin(115200);
 
 
 char c = 'c';
 int i = (int)c;
 byte letter = (byte)i;
 while(1){}
 /*
  // Setup SD card
  if ( !SD.begin(ACDH_SD_PIN) ) {
      Serial.println("ERROR: SD card initialization failed.");
  }
  else{
    Serial.println("SD success");
  }
   */ 
}
/*
String dummy = "M0.2142,0.4352,0.1293M0.3456,0.5432,0.4735"
const int line_buffer_size = dummy.size() + 3;
char message[line_buffer_size];

int line_number = 0;
int _position = 0;


void write() {
    File handle = SD.open("parityCheck.txt", FILE_WRITE);
    if(SD.exists("parityCheck.txt")){
    
      //even parity bit (count the 1's)
      int parity = 0;
      string strParity; 

      //convert each ascii letter into 8-bit binary version
      for (int ii = 0; ii < dummy.size(); ++ii) 
      {
	bitset<8> letter(dummy.c_str()[ii]);
	parity += letter.count();
      }

      //pad parity with zeros so that the size of data written is const. (Max parity value ~= 400)
      if (parity < 10)
	strParity = "00" + to_string(parity);
      else if (parity < 100) 
	strParity = "0" + to_string(parity);
      else 
       	strParity = to_string(parity);

      //write to SD card
      handle.println((dummy + strParity));
      handle.close();
    }
    //reset values
    _write = "";
    parity = 0;
    readyWrite = 0;
}


void send() {
    ifstream sdin("parityCheck.txt");
    
    sdin.getline(message, line_buffer_size); 
    //IMPLEMENT PARITY CHECK OF DATA RETRIEVED FROM FILE
    int parity = 0;
    string strParity; 
    //convert each ascii letter into 8-bit binary version
    for (int ii = 0; ii < (message.size() - 3); ++ii) 
    {
       	bitset<8> letter(message.c_str()[ii]);
	parity += letter.count();
    }
    //pad parity with zeros so that the size of data written is const. (Max parity value ~= 400)
    if (parity < 10)
	strParity = "00" + to_string(parity);
    else if (parity < 100) 
  	strParity = "0" + to_string(parity);
    else 
  	strParity = to_string(parity);

    if (strParity.compare(message.substring(message.size() - 3)) != 0) {
	Serial.println("Error, parity bit does not match");
    }
    else {
	Serial.println(message.substring(message.size() - 3));
	Serial.println("-- Message is clean");
    }
}
*/
void loop() {
  // put your main code here, to run repeatedly:
  //write();
  //send();
}
