// ========
// Includes
// ========

#include <RF22Mesh.h>
#include <SdFat.h>
#include <TinyGPS.h>

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

//Initiate 9-axis IMU 
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 

AndesiteRadio _Radio;
AndesiteOrbit _Orbit;
AndesiteFile _File;
AndesiteWSN WSN;
AndesiteCollect Tester;

int toggle;
int check;
//String data;
//char * temp;


// /////////////////////////////////////////////
// ///// INITIALIZE INTERRUPTS FOR TESTING /////
// /////////////////////////////////////////////


ISR(TIMER1_COMPA_vect) {
//digitalWrite(13, HIGH);
//delay(20); 
check = 1;
//digitalWrite(13, HIGH);
//delay(20);
};



// ////////////////////////////////////
// ///// RUN WIRELESS SENSOR NODE /////
// ////////////////////////////////////

// Setup the sensor node
void setup() {
    // Set baud rate
    Serial.begin(ACDH_SERIAL_BAUD);
    Serial1.begin(ACDH_SERIAL_BAUD);
    
    // Setup magnetometer
    DOF.begin();
    
    //setup SD card
    if ( !SD.begin(ACDH_SD_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
    }

    //setup interrupts
    noInterrupts();
    TCCR1A = 0x00; // normal operation page 148 (mode0);
    TCNT1= 0x0000; // 16bit counter register
    
    //FOR DATA RATES BETWEEN 10HZ and 30HZ USE THIS BLOCK AND UNCOMMENT FIRST LINE AND ANY DATA RATE IN THIS BLOCK
    TCCR1B = 0x0B; // 16MHZ with 64 prescalar for data rates between 10Hz and 30Hz
    OCR1A = (25000-1);  //10Hz
    //OCR1A = (16667-1);  //about 15Hz (15.0006)
    //OCR1A = (12500-1);  //20Hz
    //OCR1A = (10000-1);  //25Hz
    //OCR1A = (9615-1);  //about 26Hz (26.001)
    //OCR1A = (9259-1);  //about 27Hz (27.0006)
    //OCR1A = (8928-1);  //about 28Hz (28.0018)
    //OCR1A = (8621-1);  //about 29Hz (28.99896)
    //OCR1A = (8333-1);  //about 30Hz (30.0012)
    //END OF DATA RATES BETWEEN 10HZ AND 30HZ BLOCK
    
    
    
    //FOR DATA RATES BETWEEN 31HZ AND 50HZ USE THIS BLOCK AND UNCOMMENT FIRST LINE AND ANY DATA RATE IN THIS BLOCK
    //TCCR1B = 0x0A; // 16MHZ with 8 prescalar for data rates between 31Hz and 50Hz
    //OCR1A = (64516-1);  //about 31Hz (31.00006)
    //OCR1A = (62500-1);  //32Hz 
    //OCR1A = (60606-1);  //about 33Hz (33.00003)
    //OCR1A = (58823-1);  //about 34Hz (34.0003)
    //OCR1A = (57143-1);  //about 35Hz (34.999913)
    //OCR1A = (50000-1);  //40Hz
    //OCR1A = (44444-1);  //about 45Hz (45.00045)
    //OCR1A = (40000-1);  //50Hz
    //END OF DATA RATES BETWEEN 31HZ AND 50HZ BLOCK
    
    TIMSK1 |= (1 << OCIE1A);
    interrupts(); // enable all interrupts
    
}


// Collect and send data to the Mule
void loop() {
  while(check){
    Tester.magTest();
    check = 0;
  }
}

