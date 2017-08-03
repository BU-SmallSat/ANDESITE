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
#include <Onewire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SFE_LSM9DS0.h>


// //////////////////////////////////
// ///// ANDESITE COLLECT CLASS /////
// //////////////////////////////////


// Collect and store magnetometer data
void AndesiteCollect::mag(unsigned long timer_diff) {
    //DOF.readMag();
    //Serial.println("collecting mag");
    String data;    
    /*
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
    */
    
	data = _ADC.payloadMessage() + ", " + String(timer_diff,6);
    //Serial.println(data);
	/*File _handle = SD.open(_File._file.c_str(), O_CREAT | O_APPEND | O_WRITE);
	 
	 if ( !_handle ) {
		 Serial.println(":: File wr failed.");
		 _handle.close();
		 return;
	 }
	 _handle.println(data); //didnt include parity
	 _handle.close();
    data = "";*/
	_File.store(data);
	data = "";
}

void AndesiteCollect::temp() {
    //DOF.readMag();
    //Serial.println("collecting mag");
    String data;    
	DallasTemperature sensors(&oneWire);
	Serial.print("Requesting temperatures...");
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.println("DONE");
	                
	Serial.print("Temperature for Device 1 is: ");
	data += sensors.getTempCByIndex(0); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
	data += "\n";
	Serial.print("Temperature for Device 2 is: ");
	data += sensors.getTempCByIndex(1);
	Serial.print("Temperature for Device 3 is: ");
	data += "\n";
	data += sensors.getTempCByIndex(2);
	Serial.print("Temperature for Device 4 is: ");
	data += "\n";
	data += sensors.getTempCByIndex(3);
	
	//data +="\n";			
	
	//Serial.println(data);
    //Serial.println(data);
    //_File.store(data);
    data = "";
}



// Collect and store gyroscope data
void AndesiteCollect::gyro() {
    //DOF.readGyro();
    //Serial.println("collecting gyro");
    String data;
    /*
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
    */
    data = "G400,500,600";
	//Serial.println(data);
    _File.store(data);
    data = "";
}



// Collect and store GPS data
void AndesiteCollect::gps() {
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
    _File.store(data);
    data = "";
}

Adafruit_TSL2561_Unified tsl1 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 00001);
Adafruit_TSL2561_Unified tsl2 = Adafruit_TSL2561_Unified(TSL2561_ADDR_HIGH, 00002);

void light(void)
{
	//Initialization: use in AndesiteWSN Init----------------------------------------------------------

  
	/* Initialise the sensor */
	if(!tsl1.begin())
	{
		/* There was a problem detecting the TSL2561 ... check your connections */
		Serial.print("Light Sensor 1 not detected: Hardware or I2C Address problem");
		while(1);	//hang
	}
	
	if(!tsl2.begin())
	{
		/* There was a problem detecting the TSL2561 ... check your connections */
		Serial.print("Light Sensor 2 not detected: Hardware or I2C Address problem");
		while(1);	//hang
	}
	  
	  
	/*										COLLECTION
		tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);       // fast but low resolution 
		tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);      // medium resolution and speed   
		tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);      // 16-bit data but slowest conversions 
		/^ Changing the integration time gives you better sensor resolution (402ms = 16-bit data) ^/
	*/
  
	/*										  GAIN
		tsl.setGain(TSL2561_GAIN_1X);      // No gain ... use in bright light to avoid sensor saturation 
		tsl.setGain(TSL2561_GAIN_16X);     // 16x gain ... use in low light to boost sensitivity 
		tsl.enableAutoRange(true);         // Auto-gain ... switches automatically between 1x and 16x 
	*/
	tsl1.enableAutoRange(true);
	tsl1.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); 

	tsl2.enableAutoRange(true);
	tsl2.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
  
	/* We're ready to go! */
	Serial.println("SETUP COMPLETE");
	Serial.println("==============");
	//Setup Finished-------------------------------------------------------------------------------------


	//Light Sensor Data Collection ===========================================================================
	/* Get a new sensor event */ 
	sensors_event_t event1;
	sensors_event_t event2;

	tsl1.getEvent(&event1);
	tsl2.getEvent(&event2);

	 
	/* Display the results (light is measured in lux) */

	if (event1.light) {
		Serial.print(event1.light); Serial.println(" lux");
	} else {
	/* If event.light = 0 lux the sensor is probably saturated
		and no reliable data could be generated! */
	Serial.println("Sensor 1 overload");
	}
	

	if (event2.light) {
		Serial.print(event2.light); Serial.println(" lux");
	} else {
	/* If event.light = 0 lux the sensor is probably saturated
		and no reliable data could be generated! */
	Serial.println("Sensor 2 overload");
	}
	//end of data collection ================================================================================
	
}