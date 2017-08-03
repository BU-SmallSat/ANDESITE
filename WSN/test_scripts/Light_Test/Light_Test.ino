#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SFE_LSM9DS0.h>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
   
   Connections
   ===========
   Connect SCL to analog 5 //actually 4
   Connect SDA to analog 4 //actually 6
   Connect VDD to 3.3V DC
   Connect GROUND to common ground

   I2C Address
   ===========
   The address will be different depending on whether you leave
   the ADDR pin floating (addr 0x39), or tie it to ground or vcc. 
   The default addess is 0x39, which assumes the ADDR pin is floating
   (not connected to anything).  If you set the ADDR pin high
   or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
   (0x29) respectively.
    
   History
   =======
   2013/JAN/31  - First version (KTOWN)
*/

Adafruit_TSL2561_Unified tsl1 = Adafruit_TSL2561_Unified(TSL2561_ADDR_LOW, 12345);
Adafruit_TSL2561_Unified tsl2 = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 54321);
Adafruit_TSL2561_Unified tsl3 = Adafruit_TSL2561_Unified(TSL2561_ADDR_HIGH, 11111);

bool OneOn =   true;    // light sensor 1
bool TwoOn =   false;   // 3rd, non-existent light sensor (supported by library) 
bool ThreeOn = true;    // light sensor 2

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
	sensor_t sensor1;
	sensor_t sensor2;
	sensor_t sensor3;
	
	if(OneOn) {
	tsl1.getSensor(&sensor1);
	Serial.println("------------------------------------");
	Serial.print  ("Sensor:       "); Serial.println(sensor1.name);
	Serial.print  ("Driver Ver:   "); Serial.println(sensor1.version);
	Serial.print  ("Unique ID:    "); Serial.println(sensor1.sensor_id);
	Serial.print  ("Max Value:    "); Serial.print(sensor1.max_value); Serial.println(" lux");
	Serial.print  ("Min Value:    "); Serial.print(sensor1.min_value); Serial.println(" lux");
	Serial.print  ("Resolution:   "); Serial.print(sensor1.resolution); Serial.println(" lux");
	Serial.println("------------------------------------");
	Serial.println("");
	}

	if(TwoOn){
	tsl2.getSensor(&sensor2);
	Serial.println("------------------------------------");
	Serial.print  ("Sensor:       "); Serial.println(sensor2.name);
	Serial.print  ("Driver Ver:   "); Serial.println(sensor2.version);
	Serial.print  ("Unique ID:    "); Serial.println(sensor2.sensor_id);
	Serial.print  ("Max Value:    "); Serial.print(sensor2.max_value); Serial.println(" lux");
	Serial.print  ("Min Value:    "); Serial.print(sensor2.min_value); Serial.println(" lux");
	Serial.print  ("Resolution:   "); Serial.print(sensor2.resolution); Serial.println(" lux");
	Serial.println("------------------------------------");
	Serial.println("");
	}
	
	if(ThreeOn) {
	tsl3.getSensor(&sensor3);
	Serial.println("------------------------------------");
	Serial.print  ("Sensor:       "); Serial.println(sensor3.name);
	Serial.print  ("Driver Ver:   "); Serial.println(sensor3.version);
	Serial.print  ("Unique ID:    "); Serial.println(sensor3.sensor_id);
	Serial.print  ("Max Value:    "); Serial.print(sensor3.max_value); Serial.println(" lux");
	Serial.print  ("Min Value:    "); Serial.print(sensor3.min_value); Serial.println(" lux");
	Serial.print  ("Resolution:   "); Serial.print(sensor3.resolution); Serial.println(" lux");
	Serial.println("------------------------------------");
	Serial.println("");
	}
	
	delay(5);

}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  
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
	
	tsl3.enableAutoRange(true);
	tsl3.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);
	
	
  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void) {
	Serial.begin(115200);
	Serial.println("Light Sensor Test"); Serial.println("");
	pinMode(13,OUTPUT);
	
	digitalWrite(13,HIGH);
  
	/* Initialise the sensor */
	if(!tsl1.begin() && OneOn)
	{
		/* There was a problem detecting the TSL2561 ... check your connections */
		Serial.print("Ooops, no TSL2561 ==Version 1== detected ... Check your wiring or I2C ADDR!");
		while(1);	//hang
	}
	
	if(!tsl2.begin() && TwoOn)
	{
		/* There was a problem detecting the TSL2561 ... check your connections */
		Serial.print("Ooops, no TSL2561 ===Version 2== detected ... Check your wiring or I2C ADDR!");
		while(1);	//hang
	}
	
	if(!tsl3.begin() && ThreeOn)
	{
		/* There was a problem detecting the TSL2561 ... check your connections */
		Serial.print("Ooops, no TSL2561 ===Version 3== detected ... Check your wiring or I2C ADDR!");
		while(1);	//hang
	}
		
		
  
	/* Display some basic information on this sensor */
	displaySensorDetails();
  
	/* Setup the sensor gain and integration time */
	configureSensor();
  
	/* We're ready to go! */
	Serial.println("SETUP COMPLETE");
	Serial.println("==============");
	//Wire.begin();
	
		
	displaySensorDetails();
	digitalWrite(13,LOW);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) 
{  
	//displaySensorDetails();
	/* Get a new sensor event */ 
	sensors_event_t event1;
	sensors_event_t event2;
	sensors_event_t event3;
	digitalWrite(13,HIGH);
	if(OneOn) tsl1.getEvent(&event1);
	if(TwoOn) tsl2.getEvent(&event2);
 	if(ThreeOn) tsl3.getEvent(&event3);
	 
	/* Display the results (light is measured in lux) */
	if(OneOn) {
	if (event1.light) {
		Serial.print(event1.light); Serial.println(" lux");
	} else {
	/* If event.light = 0 lux the sensor is probably saturated
		and no reliable data could be generated! */
	Serial.println("Sensor 1 overload");
	}
	}
	
	
	if(TwoOn) {
	if (event2.light) {
		Serial.print(event2.light); Serial.println(" lux");
	} else {
	/* If event.light = 0 lux the sensor is probably saturated
		and no reliable data could be generated! */
	Serial.println("Sensor 2 overload");
	}
	}
	
	
	if(ThreeOn) {
	if (event3.light) {
		Serial.print(event3.light); Serial.println(" lux");
	} else {
	/* If event.light = 0 lux the sensor is probably saturated
	and no reliable data could be generated! */
		Serial.println("Sensor 3 overload");
	}
	}
	
	delay(500);
	digitalWrite(13,LOW);
	delay(500);
}