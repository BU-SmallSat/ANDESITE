#include <RF22Router.h>
#include <Wire.h>
#include <SPI.h>
#include "libandesite.h"
#include "AndesiteRadio.h"
#include "AndesiteMule.h"

String message = "";
char nextChar = ' ';
int network_array[8];
    
// Setup the Mule 
void setup() {
    Serial.begin(115200);
    Serial.flush();
    delay(500);
    while(message != "initSuccess"){
      if(Serial.available() > 0){
        nextChar = Serial.read();
        while(nextChar != '\n' ){
          //Serial.println(nextChar);
          message += nextChar;
          nextChar = Serial.read();
        }
      }
      delay(100);
    } 
    Serial.println("initSuccess");
    for (int i = 0; i < 8;i++){
      network_array[i] = 0;
      //Serial.print(network_array[i]);
    }
}


// Receive messages from the wireless sensore nodes (WSN)
void loop() {
    /*
    Serial.println("newNode:");
    Serial.println("WSN1orbit1");
    Serial.println("line1 of data");
    Serial.println("line2ofData");
    Serial.println("line:3:of:data:");
    Serial.println("newNode:");
    Serial.println("WSN2orbit1");
    Serial.println("node2s first line of data");
    Serial.println("2: second line of data");
    Serial.println("2: finalLineOfData ");
    Serial.println("newNode:");
    Serial.println("WSN1orbit2");
    Serial.println("line1 of data");
    Serial.println("line2ofData");
    Serial.println("line:3:of:data:");
    Serial.println("newNode:");
    Serial.println("WSN2orbit2");
    Serial.println("node2s first line of data");
    Serial.println("2: second line of data");
    Serial.println("2: finalLineOfData ");
    */
    message = "";
    if(Serial.available() > 0){
      while(nextChar != '\n' ){
        //Serial.println(nextChar);
        message += nextChar;
        nextChar = Serial.read();
      }
      nextChar = Serial.read();
      if (nextChar == '\n'){
        nextChar = Serial.read();
      }
      if(message.startsWith("updateNetwork:",0)){
        for (int i = 0; i < 8;i++){
          network_array[i] = message[i+14] - 48;
        }
        Serial.print("updatedNetwork:");
        for (int i = 0; i < 8;i++){
          Serial.print(network_array[i]);
        }
        Serial.print("\n");
      }
    }
    delay(500);
}
