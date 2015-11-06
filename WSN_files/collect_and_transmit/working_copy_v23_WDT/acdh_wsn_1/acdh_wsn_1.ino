// ========
// Includes
// ========
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <RF22Mesh.h>
#include <SdFat.h>
#include <TinyGPS.h>

#include <EEPROM.h>
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


// ========
// Declares
// ========

// Create instance of classes for instruments
RF22Mesh RF22(ACDH_WSN1_ADDR);
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


int check = 0;
int count = 0;
int Science_Mode_State = 0;


// //////////////////////////////////////////////////
// ///// INITIALIZE INTERRUPTS FOR DATA SAMPLING/////
// //////////////////////////////////////////////////


ISR(TIMER1_COMPA_vect) {
    
    //WDT_RESET to guarantee we are reaching the interrupts
    //Serial.println("here");
    if(count >= 2){
      Science_Mode_State = 2;
      count = 0;
    }
    else {
      Science_Mode_State = 1;
      count++;
    }
};

ISR(WDT_vect){
   //interrupt code : do the job here
   Serial.println("WDT");
   //MCUSR &= ~(1 << WDRF);
}

//setup the watchdog to timeout every 4 seconds and make an interrupt and a reset
void setupWatchdog(){
    //README : must set the fuse WDTON to 0 to enable the watchdog
    Serial.println("Watchdog setup.");
    //disable interrupts
    cli();
    
    //make sure watchdod will be followed by a reset (must set this one to 0 because it resets the WDE bit)
    MCUSR &= ~(1 << WDRF);
    //set up WDT interrupt (from that point one have 4 cycle to modify WDTCSR)
    WDTCSR = (1<<WDCE)|(1<<WDE);
    //Start watchdog timer with 4s prescaller and interrupt then resest
    WDTCSR = (1<<WDIE)|(1<<WDE)|(1<<WDP3)|(1<<WDP0);
    //Enable global interrupts
    sei();
}

// ////////////////////////////////////
// ///// RUN WIRELESS SENSOR NODE /////
// ////////////////////////////////////

// Setup the sensor node
void setup() {
    
    // Set baud rate
    Serial.begin(ACDH_SERIAL_BAUD);
    Serial1.begin(ACDH_SERIAL_BAUD);
    
    Serial.println("here");
    Serial1.println("here1");
    //Setup WDT
    setupWatchdog();
    //wdt_reset(); //COMMENT OUT TO TEST WDT
    
    //set up interrupts for data sampling
    //noInterrupts();
    TCCR1A = 0x00; // normal operation page 148 (mode0);
    TCNT1= 0x0000; // 16bit counter register
    TCCR1B = 0x0B; // 16MHZ with 64 prescalar
    OCR1A = (25000-1);  //10Hz
    TIMSK1 &= !(1 << OCIE1A);
    //TIMSK1 &= !(1 << TOIE1);
    
    //interrupts();
    // Setup the wireless sensor node
//    if ( WSN.init() != 0 ) {
//        Serial.println(":: WSN initialization failed, fix errors and try again");
//        //while (1) {}
//    }
//    else {
//        Serial.println(":: WSN initialization succeeded");
//       // while (1) {}
//    }
    Serial.println("Done with main setup.");
    
    wdt_reset();  //COMMENT OUT TO TEST WDT
}


// Collect and send data to the Mule
void loop() {
  
    for (int i = 0; i < 20; i++)
    {
        EEPROM.write(i,i+10);
    }
    int value;
    for (int i = 0; i < 20; i++)
    {
      value = EEPROM.read(i);
      Serial.print("Address: ");
      Serial.print(i);
      Serial.print(", Value: ");
      Serial.println(value);
    }
    //Serial.println("loop");
  
    // Enter Science Mode
    if ( WSN.isScienceMode() ) {
        wdt_reset();
        WSN.scienceMode();
        wdt_reset();
    }
    
    // Enter Transfer Mode
    if ( WSN.isTransferMode() ) {
        wdt_reset();
        WSN.transferMode();
        wdt_reset();
    }
    
    WSN.wait();
}

