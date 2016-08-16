
#include <SdFat.h>
SdFat SD;
//File handle;
#define ACDH_SD_PIN               9


void setup() {
  Serial.begin(115200);
  // Setup SD card
  while ( !SD.begin(ACDH_SD_PIN) ) {
      Serial.println("ERROR: SD card initialization failed.");
  }
  Serial.println("SD success");
 
    
}

String dummy = "M0.2142,0.4352,0.1293M0.3456,0.5432,0.4735";
const int line_buffer_size = 50;
char message[line_buffer_size];

int line_number = 0;
int _position = 0;


String write() {
    File handle = SD.open("Test.txt", FILE_WRITE);
    if(SD.exists("Test.txt")){
    
      //even parity bit (count the 1's)
      int parity = 0;
      String strParity; 
      int letter;

      //convert each ascii letter into 8-bit binary version
      for (int ii = 0; ii < dummy.length(); ++ii) 
      {
        letter = (int)dummy[ii];
        while(letter > 0){
          if(letter%2 != 0){
            parity++;
          }
          letter = letter/2;
        }
      }
      
      //pad parity with zeros so that the size of data written is const. (Max parity value ~= 400)
      if (parity < 10)
	strParity = "00" + (String)parity;
      else if (parity < 100) 
	strParity = "0" + (String)parity;
      else 
       	strParity = (String)parity;

      //write to SD card
      handle.println((dummy + strParity));
      handle.close();
      //reset values
    parity = 0;
    return strParity;
  }
}


void send() {
    ifstream sdin("Test.txt");
    
    sdin.getline(message, line_buffer_size); 
    //IMPLEMENT PARITY CHECK OF DATA RETRIEVED FROM FILE
    int parity = 0;
    String strParity;
    String strMessage;
   
    strMessage = (String)message;
    Serial.println(strMessage);
      int letter;

      //convert each ascii letter into 8-bit binary version
      for (int ii = 0; ii < strMessage.length(); ++ii) 
      {
        letter = (int)strMessage[ii];
        while(letter > 1){
          if(letter%2 != 0){
            parity++;
          }
          letter = letter/2;
        }
      }
      
    //pad parity with zeros so that the size of data written is const. (Max parity value ~= 400)
    if (parity < 10)
	strParity = "00" + (String)parity;
    else if (parity < 100) 
  	strParity = "0" + (String)parity;
    else 
  	strParity = (String)parity;
  parity = 0;
  Serial.println(strMessage.substring(strMessage.length() - 3));

    if (strParity == strMessage.substring(strMessage.length() - 3)) {
	Serial.println("Error, parity bit does not match");
         strMessage = strMessage.substring(0,strMessage.length()-3);
        strMessage += 0;
    }
    else {
	Serial.println(strMessage.substring(strMessage.length() - 3));
	Serial.println("-- Message is clean");
        strMessage = strMessage.substring(0,strMessage.length()-3);
        strMessage += 1;
    }
    Serial.println(strMessage);
}

void loop() {
  // put your main code here, to run repeatedly:
  write();
  send();
  while(1){}
}
