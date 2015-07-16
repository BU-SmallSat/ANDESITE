// -*-c++-*-
// **********************************************************************************
// 
// Name:    AndesiteCollect.cpp
// Class:   <AndesiteCollect>
// Author:  Gabriel Gonzalez
// 
// Description: Provides the functionality to collect and store (not write!) data on
//              the ANDESITE Satellite.
//              
// Notes: Check if converting from double to string correctly.
// 
// **********************************************************************************


// ========
// Includes
// ========

#include "AndesiteCollect.h"
#include "AndesiteData.h"
#include "AndesiteFile.h"
#include <string.h>

#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW


// //////////////////////////////////
// ///// ANDESITE COLLECT CLASS /////
// //////////////////////////////////

// Collect and store magnetometer data
void AndesiteCollect::mag() {
    DOF.readMag();
    String data;    
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "M";
    //dtostrf(acdh_adc_readData(.....),1,4,temp);
    dtostrf(DOF.calcMag(DOF.mx), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcMag(DOF.my), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcMag(DOF.mz), 1, 4, temp);
    data += temp;
    
    _File.store(data);
}



// Collect and store gyroscope data
void AndesiteCollect::gyro() {
    DOF.readGyro();
    String data;
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "G";
    dtostrf(DOF.calcGyro(DOF.gx), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcGyro(DOF.gy), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(DOF.calcGyro(DOF.gz), 1, 4, temp);
    data += temp;
    
    _File.store(data);
}



// Collect and store GPS data
void AndesiteCollect::gps() {
    String data;
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "T";
    data += GPS.time.hour();
    data += ":";
    data += GPS.time.minute();
    data += ":";
    data += GPS.time.second();
    data += ".";
    data += GPS.time.centisecond();
    data += ",";
    
    dtostrf(GPS.location.lat(), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(GPS.location.lng(), 1, 4, temp);
    data += temp;
    data += ",";
    dtostrf(GPS.altitude.meters(), 1, 4, temp);
    data += temp;
    
    _File.store(data);
}

void AndesiteCollect::magTest() {
    //digitalWrite(13, HIGH);
    //delay(20);

    //LSM9DS0 DOF(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
    //DOF.begin();
    //Wire.begin(); 
    DOF.readMag();
    //digitalWrite(13, HIGH);
    //delay(20);
    //Serial.println("here");
    String data;    
    char temp[10] = {0};
    
    // Concatenate strings to write just one string to file
    data = "X:";
    //dtostrf(acdh_adc_readData(.....),1,4,temp);
    dtostrf(DOF.calcMag(DOF.mx), 1, 4, temp);
    data += temp;
    data += ",\tY:";
    dtostrf(DOF.calcMag(DOF.my), 1, 4, temp);
    data += temp;
    data += ",\tZ:";
    dtostrf(DOF.calcMag(DOF.mz), 1, 4, temp);
    data += temp;
    data += "\n";
    //digitalWrite(13, HIGH);
    //delay(20);
    _File.writeMagTest(data);
    Serial.println(data);
    //digitalWrite(13, HIGH);
    //delay(20);
}