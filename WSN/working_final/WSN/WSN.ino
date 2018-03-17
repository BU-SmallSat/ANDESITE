#include <DallasTemperature.h>
#include <OneWire.h>
// ========
// Includes
// ========
#include <avr/wdt.h>
#include <RF22ReliableDatagram.h>
#include <SdFat.h>
#include <TinyGPS.h>
#include <AndesiteWSN.h>
//#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "AndesiteRadio.h"
#include "libandesite.h"
#include "ADS1248.h"

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress Temp1,Temp2,Temp3,Temp4;
//**********************************************//
//************MODIFY FOR FLIGHT NODES***********//
int wsn_num = ACDH_WSN2_ADDR;
RF22ReliableDatagram RF22(wsn_num);
//**********************************************//
//**********************************************//

// ========
// Declares
// ========
// Create instance of classes for instruments
SdFat SD;
TinyGPSPlus GPS;


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
unsigned long lastInt = millis();
unsigned long now = millis();
ISR(TIMER1_COMPA_vect) {
	WSN._science_mode_state = 1;

	
	if(count == WSN._temp_timing){
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


//setup the watchdog to timeout after 8 seconds and reset
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

	TCCR1A = 0x00; // normal operation page 148 (mode0);
	TCNT1= 0x0000; // 16bit counter register
	
	TCCR1B = THIRTY_HZ_TCCRIB; // 16MHZ with 8 prescalar
	OCR1A = (THIRTY_HZ_OCRIA);  //30Hz
	TIMSK1 &= !(1 << OCIE1A);

	Serial.print("WSN number is: ");
	Serial.println(wsn_num);
	WSN.init();
	Serial.println("Done with main setup.");
}


// Collect and send data to the Mule
void loop() {
	
	// Enter Science Mode
	if ( WSN.isScienceMode() ) {
		//wdt_reset();
		WSN.scienceMode(false);
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
