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
#include "ADS1248.h"
#include <SPI.h>



// //////////////////////////////////
// ///// ANDESITE COLLECT CLASS /////
// //////////////////////////////////

// Collect and store magnetometer data
String AndesiteCollect::mag() {

    //Serial.println("collecting mag");
	
	String data;    
	long X_raw;
	long Y_raw;
	long Z_raw;
		
	float voltageX;
	float voltageY;
	float voltageZ;	
		
	X_raw = _ADC.READ_DATA(MUX_CH1_X);
	Y_raw = _ADC.READ_DATA(MUX_CH2_Y);
	Z_raw = _ADC.READ_DATA(MUX_CH3_Z);

	voltageX = ((5.0/8388608.0)*X_raw)/64;
	voltageY = ((5.0/8388608.0)*Y_raw)/64;
	voltageZ = ((5.0/8388608.0)*Z_raw)/64;
    
	//data = _ADC.payloadMessage() + ", " + String(timer_diff,6);
	data = "M"+String(voltageX,6)+","+String(voltageY,6)+","+String(voltageZ,6);
	return data;
	//Serial.println(data);
	//_File.store(data);
}

String AndesiteCollect::temp() {

    Serial.println("collecting temp");
    String data;    
	
	
	data = "T100.00";
	return data;
	//Serial.println(data);
	//_handle.println(data); //didnt include parity
    
	//_File.store(data);

}



// Collect and store gyroscope data
String AndesiteCollect::gyro() {
    //Serial.println("collecting gyro");
	DOF.readGyro();
    String data;
	/*
	    	DOF.readGyro();
	    	Serial.begin(ACDH_SERIAL_BAUD);
	    	Serial.println("collecting gyro");
	    	Serial.print("G:");
	    	Serial.print(DOF.calcGyro(DOF.gx));
	    	Serial.print(",");
	    	Serial.print(DOF.calcGyro(DOF.gy));
	    	Serial.print(",");
	    	Serial.println(DOF.calcGyro(DOF.gz));
	*/
   
    //data = "G400,500,600";
	data = "G"+String(DOF.calcGyro(DOF.gx),6)+","+String(DOF.calcGyro(DOF.gy),6)+","+String(DOF.calcGyro(DOF.gz),6);
	return data;
	
	//Serial.println(data);
	//_handle.println(data);
    //_File.store(data);

}



// Collect and store GPS data
String AndesiteCollect::gps() {
    String data;
    //Serial.println("collecting gps");
    /*
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
    */
    data = Serial2.readString();
	//Serial.println(data);
    return data;
	//_File.store(data);
    
}
