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

// Initiate 9-axis IMU 
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW
LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM); 

AndesiteRadio _Radio;
AndesiteOrbit _Orbit;
AndesiteFile _File;
AndesiteWSN WSN;



// ////////////////////////////////////
// ///// RUN WIRELESS SENSOR NODE /////
// ////////////////////////////////////

// Setup the sensor node
void setup() {
    
    // Set baud rate
    Serial.begin(ACDH_SERIAL_BAUD);
    Serial1.begin(ACDH_SERIAL_BAUD);
    
    // Setup the wireless sensor node
    if ( WSN.init() != 0 ) {
        Serial.println(":: WSN initialization failed, fix errors and try again");
        while (1) {}
    }
    
    Serial.println("Done with main setup.");
}


// Collect and send data to the Mule
void loop() {
    
    // Enter Science Mode
    if ( WSN.isScienceMode() )
        WSN.scienceMode();
    
    // Enter Transfer Mode
    if ( WSN.isTransferMode() ) 
        WSN.transferMode();
    
    WSN.wait();
}

