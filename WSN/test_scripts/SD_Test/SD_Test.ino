// ========
// Includes
// ========
#include <SdFat.h>
#include <SPI.h>
#include <Wire.h>
#include <libandesite.h>

// ========
// Declares
// ========
SdFat SD;


File myFile;

void setup() {
	// Set baud rate
	Serial.begin(115200);
	pinMode(13, OUTPUT);
	// set up SPI slave select pins
	pinMode(SD_CS_PIN, OUTPUT);
	pinMode(RF_CS_PIN, OUTPUT);
	pinMode(ADS_CS_PIN, OUTPUT);
	pinMode(RF_SHDN_PIN, OUTPUT);

	Serial.println("*****Setting Chip Selects LOW*****");
	digitalWrite(SD_CS_PIN, LOW);
	digitalWrite(RF_CS_PIN, HIGH);
	digitalWrite(ADS_CS_PIN, HIGH);
	digitalWrite(RF_SHDN_PIN, HIGH);
	digitalWrite(13,HIGH);
	delay(1000);

	Serial.print("Initializing SD card...");

	if (!SD.begin(SD_CS_PIN)) {
		Serial.println("initialization failed!");
		delay(60000);
		Serial.println("Done");
		return;
	}
	Serial.println("initialization done.");
	
	
	// open the file. note that only one file can be open at a time,
	// so you have to close this one before opening another.
	myFile = SD.open("test.txt", FILE_WRITE);

	// if the file opened okay, write to it:
	if (myFile) {
		Serial.print("Writing to test.txt...");
		myFile.println("Hello World!");
		// close the file:
		myFile.close();
		Serial.println("done.");
		} else {
		// if the file didn't open, print an error:
		Serial.println("error opening test.txt");
	}

	// re-open the file for reading:
	myFile = SD.open("test.txt");
	if (myFile) {
		Serial.println("test.txt:");

		// read from the file until there's nothing else in it:
		while (myFile.available()) {
			Serial.write(myFile.read());
		}
		// close the file:
		myFile.close();
		} else {
		// if the file didn't open, print an error:
		Serial.println("error opening test.txt");
	}
	
	Serial.println("Done with main setup.");
	Serial.println("=====================");
	delay(1000);
	digitalWrite(13,LOW);
	digitalWrite(RF_CS_PIN, HIGH);
	
}


void loop() {
	digitalWrite(SD_CS_PIN, HIGH);
	digitalWrite(13,!digitalRead(13));
	delay(1000);
}