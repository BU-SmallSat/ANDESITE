// ========
// Includes
// ========

#include "AndesiteMule.h"
#include "AndesiteRadio.h"
#include "libandesite.h"

#include <SPI.h>
#include <Wire.h>
#include <RF22Mesh.h>
#include <SdFat.h>
#include <SFE_LSM9DS0.h> 
#include <TinyGPS.h>


// ========
// Declares
// ========

// Create instance of Radio and Sd card
RF22Mesh RF22(ACDH_MULE_ADDR, 9);
SdFat SD;

AndesiteRadio _Radio;
AndesiteMule _Mule;



// ////////////////////
// ///// RUN MULE ///// 
// ////////////////////

// Setup the Mule 
void setup() {
    if ( _Mule.init() != 0 ) {
        Serial.println(":: Mule initialization failed, fix errors and try again");
        while (1) {}
    }
    
    Serial.println("Done with main setup.");
}


// Receive messages from the wireless sensore nodes (WSN)
void loop() {
    
    // Request data from available WSNs
    if ( _Mule.listen() == 0 ) 
        _Mule.request(); // Has a return status
}
