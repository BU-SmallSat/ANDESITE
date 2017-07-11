// ========
// Includes
// ========

#include <SFE_LSM9DS0.h>
#include <Wire.h>
#include "AndesiteWSN.h"
#include "libandesite.h"

// ========
// Declares
// ========

// Initiate 9-axis IMU
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);

void setup() {
	// Set baud rate
	Serial.begin(ACDH_SERIAL_BAUD);
	pinMode(13, OUTPUT);
	digitalWrite(13,HIGH);

	
	// Setup science instruments
	DOF.begin();
	Wire.begin();
	digitalWrite(13,LOW);
	Serial.println("Done with main setup.");
	Serial.println("=====================");
}


void loop() {
	//testing the gyro
	digitalWrite(13,HIGH);
	DOF.readGyro();
	Serial.begin(ACDH_SERIAL_BAUD);
	Serial.println("collecting gyro");
	Serial.print("G:");
	Serial.print(DOF.calcGyro(DOF.gx));
	Serial.print(",");
	Serial.print(DOF.calcGyro(DOF.gy));
	Serial.print(",");
	Serial.println(DOF.calcGyro(DOF.gz));	
	Serial.print("One set of reading done!");
	delay(500); digitalWrite(13,LOW); delay(500);                                    //waiting a second
}