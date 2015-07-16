// ========
// Includes
// ========

#include "AndesiteMule.h"
#include "AndesiteComm.h"
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



// ////////////////////
// ///// RUN MULE ///// 
// ////////////////////

// Setup the Mule 
void setup() {
    acdh_init_mule();
}


// Receive messages from the wireless sensore nodes (WSN)
void loop() {
    
    // Determine which WSN are available 
    acdh_mule_avail_wsn();  // Has a return status
    
    // All WSN are done, receive data from them
    if ( AVAIL_WSN_NUM > 0 ) 
        acdh_mule_request_data(); // Has a return status
}
