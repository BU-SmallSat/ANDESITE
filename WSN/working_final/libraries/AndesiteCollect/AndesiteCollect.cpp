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

// //////////////////////////////////
// ///// ANDESITE COLLECT CLASS /////
// //////////////////////////////////

// Collect and store magnetometer data
String AndesiteCollect::mag(unsigned long timer_diff) {
    //Serial.println("collecting mag");
    String data;    
	long X_raw = _ADC.READ_DATA(MUX_CH1_X);
	long Y_raw = _ADC.READ_DATA(MUX_CH2_Y);
	long Z_raw = _ADC.READ_DATA(MUX_CH3_Z);

	float voltageX = ((5.0/8388608.0)*X_raw)/64;
	float voltageY = ((5.0/8388608.0)*Y_raw)/64;
	float voltageZ = ((5.0/8388608.0)*Z_raw)/64;
					
	data = "M"+String(voltageX,6)+","+String(voltageY,6)+","+String(voltageZ,6)+ ","+String(timer_diff,6);
	//Serial.println(data);
	return data;
}

String AndesiteCollect::temp() {
    String data; 
	sensors.requestTemperatures();
    //Serial.println("collecting temp");   
	data= "T"+String(sensors.getTempC(Temp1))+",";	
	data+= String(sensors.getTempC(Temp2))+",";
	data+= String(sensors.getTempC(Temp3))+",";
	data+= String(sensors.getTempC(Temp4));
	//Serial.println(data);
	
    return data;
}


// Collect and store gyroscope data
String AndesiteCollect::gyro() {
    String data;
    char temp[10] = {0};
	data = "G400,500,600";
    return data;
}



// Collect and store GPS data
String AndesiteCollect::gps() {
    String data = "L";
	unsigned long timer_start = millis();
	while(1){
		if ( GPS.encode(Serial1.read()) ) {
				data = "*" + String(GPS.location.lat());
				Serial.println(data);
				//_File.store(data);
				return data;
		}
		if ( (millis() - timer_start) >= GPS_SCIENCE_TIMEOUT ) { break; }
	}
	data = GPS.location.lat();
	//Serial.println(data);
    return data;
}
