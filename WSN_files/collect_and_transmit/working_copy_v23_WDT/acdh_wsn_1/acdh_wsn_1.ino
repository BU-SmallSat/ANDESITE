// ========
// Includes
// ========
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

unsigned long time;

// //////////////////////////////////////////////////
// ///// INITIALIZE INTERRUPTS FOR DATA SAMPLING/////
// //////////////////////////////////////////////////


ISR(TIMER1_COMPA_vect) {
  
    if(count >= 2){
      Science_Mode_State = 2;
      count = 0;
    }
    else {
      Science_Mode_State = 1;
      count++;
    }
};


//setup the watchdog to timeout every 8 seconds and reset
void setupWatchdog(){
  
    //README : must set the fuse WDTON to 0 to enable the watchdog
    Serial.println("Watchdog setup.");
    
    //disable interrupts
    cli();
    //MCUSR &= ~(1 << WDRF)
    //WDTCSR = (1 << WDCE)|(1 << WDE);
    wdt_enable(WDTO_8S);
    //Enable global interrupts
    sei();
}

// ////////////////////////////////////
// ///// RUN WIRELESS SENSOR NODE /////
// ////////////////////////////////////

// Setup the sensor node
void setup() {
    //Serial.println(millis());
    // Set baud rate
    Serial.begin(ACDH_SERIAL_BAUD);
    Serial1.begin(ACDH_SERIAL_BAUD);
    
    Serial.println("here");
    Serial1.println("here1");
    
    //Setup WDT
    setupWatchdog();
    
    //set up interrupts for data sampling
    //noInterrupts();
    TCCR1A = 0x00; // normal operation page 148 (mode0);
    TCNT1= 0x0000; // 16bit counter register
    TCCR1B = 0x0B; // 16MHZ with 64 prescalar
    OCR1A = (25000-1);  //10Hz
    TIMSK1 &= !(1 << OCIE1A);
    //TIMSK1 &= !(1 << TOIE1);
    
    //Setup EEPROM (Initialize all needed addresses to ZERO) 
    //Addr 0 -- First Initialization or not
    //Addr 1 -- GPS_fail
    //Addr 2 -- Gyro_fail
    if (EEPROM.read(0) == 0) {
        for(int i = 1; i < 5; i++) {
            EEPROM.write(i, 0);
        }
        EEPROM.write(0,1);
    }    
    wdt_reset();
    
    //interrupts();
    int init_Fail;
    int GPS_fail;
    int Gyro_fail;
    
    /* Setup the wireless sensor node
     * WSN.init returns : 0 -- Init successful
     *                    1 -- GPS Error
     *                    2 -- Gyro Error
     */
    if ( init_Fail = WSN.init() != 0 ) {
        switch(init_Fail) {
            case 1:
                //GPS FAIL
                GPS_fail = EEPROM.read(1);
                if (GPS_fail == 3) {      //TO DO: If you start to simulate GPS, this means its probably failed. Dont keep retrying if another part fails later.
                    Serial1.println("SIMULATE GPS HERE!");
                }
                else 
                    EEPROM.write(1, GPS_fail + 1);
                //while(1);
            case 2:
                //GYRO FAIL
                Gyro_fail = EEPROM.read(2);
                if (Gyro_fail == 3) {      
                    Serial1.println("Three gyro failures -- we're in trouble");
                }
                else 
                    EEPROM.write(2, Gyro_fail + 1);
                //while(1);
            default:
                break;
        }
        //Serial.println(":: WSN initialization failed, fix errors and try again");
        //while (1) {}
    }
    else {
        Serial.println(":: WSN initialization succeeded");
       // while (1) {}
    }
    
    Serial.println("Done with main setup.");
    
    wdt_reset();  //COMMENT OUT TO TEST WDT
}


// Collect and send data to the Mule
void loop() {
  
    // Enter Science Mode
    if ( WSN.isScienceMode() ) {
        wdt_reset();
        WSN.scienceMode(Science_Mode_State);
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
