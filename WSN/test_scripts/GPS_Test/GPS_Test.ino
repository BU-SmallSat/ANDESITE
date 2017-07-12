// ========
// Includes
// ========
#include <avr/wdt.h>
#include <Wire.h>
#include <SFE_LSM9DS0.h>

#include "AndesiteWSN.h"
#include "libandesite.h"
#include "ADS1248.h"


// ========
// Declares
// ========

TinyGPSPlus GPS;

AndesiteOrbit _Orbit;


// ////////////////////////////////////
// ///// RUN WIRELESS SENSOR NODE /////
// ////////////////////////////////////

void setup() {
	// Set baud rate
	Serial.begin(ACDH_SERIAL_BAUD);
	Serial1.begin(9600);
	pinMode(13, OUTPUT);
	digitalWrite(13,HIGH);
	
	
	/*
	//	setup LED
	pinMode(LED_ONE_PIN, OUTPUT);
	pinMode(LED_TWO_PIN, OUTPUT);
	digitalWrite(LED_ONE_PIN, LOW);
	digitalWrite(LED_TWO_PIN, LOW);
	*/

	// Setup science instruments
	//DOF.begin();
	//Wire.begin();
	
	Serial.println("Done with main setup.");
	Serial.println("=====================");
	delay(1000);
	digitalWrite(13,LOW);
}


void loop() {
	digitalWrite(13,HIGH);
	Serial.println("collecting gps");
	Serial.print("GPS time is: ");
	Serial.print(GPS.time.hour());
	Serial.print(":");
	Serial.print(GPS.time.minute());
	Serial.print(":");
	Serial.println(GPS.time.second());

	Serial.print("GPS location is: ");
	Serial.print(GPS.location.lat());
	Serial.print(",");
	Serial.print(GPS.location.lng());
	Serial.print(",");
	Serial.println(GPS.altitude.meters());

	Serial.print("GPS Sentence With Fix: ");
	Serial.println(GPS.sentencesWithFix());
	
	//The following code displays a GPS sentence:
	/*
	delay(1000);
	
	uint8_t info[] = "You shouldn't be seeing this";
	int counter = 0;
	while (Serial1.available()) {
		char c = Serial1.read();
		//int cint = (int)c;
		//Serial.println(cint);
		Serial.print((char)c);
		counter++;
	}
	Serial.println(counter);
	Serial.print("Serial1: ");
	for(int i = 0; i < counter; i++) {
		
	}
	
	Serial.println("\n");
	Serial.print("End of serial1 output");
	*/
	delay(500); digitalWrite(13,LOW); delay(500);                                    //waiting a second
}
