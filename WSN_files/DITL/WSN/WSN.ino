
// ========
// Includes
// ========
#include <avr/wdt.h>
#include <RF22Mesh.h>
#include <SdFat.h>
#include <TinyGPS.h>


//#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <SFE_LSM9DS0.h>

#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "AndesiteRadio.h"
#include "AndesiteWSN.h"
#include "libandesite.h"
#include "ADS1248.h"


// ========
// Declares
// ========

// Create instance of classes for instruments
RF22Mesh RF22(ACDH_WSN1_ADDR);
SdFat SD;
TinyGPSPlus GPS;
SPISettings ADC_Settings(4000000, MSBFIRST, SPI_MODE1); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0

// Initiate 9-axis IMU
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);

AndesiteRadio _Radio;
AndesiteOrbit _Orbit;
AndesiteFile _File;
AndesiteWSN WSN;
ADS1248 _ADC;


int check = 0;
int count = 0;

//unsigned long time;

// //////////////////////////////////////////////////
// ///// INITIALIZE INTERRUPTS FOR DATA SAMPLING/////
// //////////////////////////////////////////////////
int lastInt = 0;

ISR(TIMER1_COMPA_vect) {
	int now = millis();
	//Serial.println(WSN._science_mode_state);
	lastInt = now;
	if(count == 3000){
		WSN._science_mode_state = 3;
		count = 0;
	}
	else if(count%3 == 0){
		WSN._science_mode_state = 2;
		count++;
	}
	else {
		WSN._science_mode_state = 1;
		count++;
	}
};


//setup the watchdog to timeout every 8 seconds and reset
void setupWatchdog(){
	Serial.println("Watchdog setup.");
	
	cli();  //disable interrupts
	wdt_enable(WDTO_8S);
	sei();  //Enable global interrupts
}

// ////////////////////////////////////
// ///// RUN WIRELESS SENSOR NODE /////
// ////////////////////////////////////

// Setup the sensor node
void setup() {
	// Set baud rate
	Serial.begin(ACDH_SERIAL_BAUD);
	Serial1.begin(ACDH_SERIAL_BAUD);
	
	
	//Serial.println("here");
	//Serial1.println("here1");
	
	//Setup WDT
	//setupWatchdog();
	
	//set up interrupts for data sampling
	//noInterrupts();
	TCCR1A = 0x00; // normal operation page 148 (mode0);
	TCNT1= 0x0000; // 16bit counter register
	TCCR1B = 0x0B; // 16MHZ with 64 prescalar
	OCR1A = (8333-1);  //10Hz
	//OCR1A = (25000-1);  //10Hz
	TIMSK1 &= !(1 << OCIE1A);
	//TIMSK1 &= !(1 << TOIE1);
	
	//wdt_reset();
	

	//delay(500);
	//interrupts();
	// Setup the wireless sensor node
	if ( WSN.init() != 0 ) {
		Serial.println(":: WSN initialization failed, fix errors and try again");
		while (1) {}
	}
	else {
		Serial.println(":: WSN initialization succeeded");
		// while (1) {}
	}

	Serial.println("Done with main setup.");
	
	//wdt_reset();  //COMMENT OUT TO TEST WDT
	//WSN.healthBeacon();
}


// Collect and send data to the Mule
void loop() {
	
	// Enter Science Mode
	if ( WSN.isScienceMode() ) {
		//wdt_reset();
		WSN.scienceMode(ADC_Settings);
		//wdt_reset();
	}
	//WSN.healthBeacon();
	
	// Enter Transfer Mode
	if ( WSN.isTransferMode() ) {
		//wdt_reset();
		WSN.listenMuleMessage();
		//wdt_reset();
	}
	//WSN.healthBeacon();
	
	WSN.wait();
}