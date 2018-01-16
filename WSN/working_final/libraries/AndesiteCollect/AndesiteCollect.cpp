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
#include <DallasTemperature.h>



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
	//_handle.println(data); //didnt include parity
    
	/*File _handle = SD.open(_File._file.c_str(), O_CREAT | O_APPEND | O_WRITE);
	 
	 if ( !_handle ) {
		 Serial.println(":: File wr failed.");
		 _handle.close();
		 return;
	 }
	 _handle.println(data); //didnt include parity
	 _handle.close();
    data = "";*/
	return data;
}

String AndesiteCollect::temp() {
    String data; 
    Serial.println("CollectingTemp");   
	data= "T"+String(sensors.getTempC(Temp1))+",";
	data+= String(sensors.getTempC(Temp2))+",";
	data+= String(sensors.getTempC(Temp3))+",";
	data+= String(sensors.getTempC(Temp4));
    //data = "T100.00";
	//Serial.println(data);
    return data;
}


// Collect and store gyroscope data
String AndesiteCollect::gyro() {
    //DOF.readGyro();
    //Serial.println("collecting gyro");
    String data;
    char temp[10] = {0};
    /*
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
	*/
	data = "G400,500,600";
	//Serial.println(data);
    //_handle.println(data);
    return data;
}



// Collect and store GPS data
String AndesiteCollect::gps() {
    String data = "L";
	unsigned long timer_start = millis();
	while(1){
		if ( GPS.encode(Serial1.read()) ) {
				//Serial.print("Here:");
				data = "*" + String(GPS.location.lat());
				Serial.println(data);
				//_File.store(data);
				return data;
		}
		if ( (millis() - timer_start) >= GPS_SCIENCE_TIMEOUT ) { break; }
	}
	//data = "L100,200,300";
	data = GPS.location.lat();
	//Serial.println(data);
	//_handle.println(data);
    return data;
}
