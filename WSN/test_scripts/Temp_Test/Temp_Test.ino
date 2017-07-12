#include <OneWire.h>
#include <DallasTemperature.h>

////////////has never been tested to work...Currently unknown if it is a software or hardware issue////////////////////////////
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 24

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

void setup(void)
{
	// start serial port
	Serial.begin(115200);
	pinMode(13,OUTPUT);
	pinMode(13,HIGH);
	Serial.println("Dallas Temperature IC Control Library Demo");

	// Start up the library
	sensors.begin(); // IC Default 9 bit. If you have troubles consider upping it 12. Ups the delay giving the IC more time to process the temperature measurement
	pinMode(13,LOW);
}


void loop(void)
{
	digitalWrite(13, HIGH);
	// call sensors.requestTemperatures() to issue a global temperature
	// request to all devices on the bus
	Serial.print("Requesting temperatures...");
	sensors.requestTemperatures(); // Send the command to get temperatures
	Serial.println("DONE");
	
	Serial.print("Temperature for Device 1 is: ");
	Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
	Serial.print("Temperature for Device 2 is: ");
	Serial.println(sensors.getTempCByIndex(1)); 
	Serial.print("Temperature for Device 3 is: ");
	Serial.println(sensors.getTempCByIndex(2)); 
	Serial.print("Temperature for Device 4 is: ");
	Serial.println(sensors.getTempCByIndex(3));
	digitalWrite(13, LOW);
	delay(1000);
}







////TMP36 Pin Variables
//int TEMP_SENSOR = 24; //the analog pin the TMP36's Vout (sense) pin is connected to
                        ////the resolution is 10 mV / degree centigrade with a
                        ////500 mV offset to allow for negative temperatures
     //
///*
    //* setup() - this function runs once when you turn your Arduino on
    //* We initialize the serial connection with the computer
    //*/
//
//void setup()
//{
    //Serial.begin(115200);  //Start the serial connection with the computer
                        ////to view the result open the serial monitor 
	//pinMode(13,OUTPUT);
	//
	//Serial.println("SETUP COMPLETE");
	//Serial.println("==============");
//}
     //
//void loop()                     // run over and over again
//{
    ////getting the voltage reading from the temperature sensor
    //int reading = analogRead(TEMP_SENSOR);  
     //
    //// converting that reading to voltage, for 3.3v arduino use 3.3
    //float voltage = reading * 5.0;
    //voltage /= 1024.0; 
     //
    //// print out the voltage
    //Serial.print(voltage); Serial.println(" volts");
     //
    //// now print out the temperature
    //float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                ////to degrees ((voltage - 500mV) times 100)
    //Serial.print(temperatureC); Serial.println(" degrees C");
     //
    //// now convert to Fahrenheit
    //float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
    //Serial.print(temperatureF); Serial.println(" degrees F");
	//delay(500); digitalWrite(13,LOW); delay(500);                                    //waiting a second
//}