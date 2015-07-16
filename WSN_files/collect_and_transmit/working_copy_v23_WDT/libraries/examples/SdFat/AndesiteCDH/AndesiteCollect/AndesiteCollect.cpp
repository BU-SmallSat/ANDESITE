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
