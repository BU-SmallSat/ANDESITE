// **********************************************************************************
// 
// Name:    AndesiteOrbit.h
// Class:   <AndesiteOrbit>
// Author:  Gabriel Gonzalez
// 
// Description: The skeleton for the Wireless Sensor Nodes (WSN) that are part of 
//              the ANDESITE Satellite.
//              
// Notes: None.
// 
// **********************************************************************************


// ========
// Includes
// ========

// Includes
#include "AndesiteWSN.h"
#include "AndesiteData.h"
#include "AndesiteCollect.h"
#include "AndesiteRadio.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "libandesite.h"
#include "ADS1248.h"
#include <SPI.h>
#include <DallasTemperature.h>



// //////////////////////////////
// ///// ANDESITE WSN CLASS /////
// //////////////////////////////

void(* resetFunc) (void) = 0;

// Initialize the wireless sensor node class
AndesiteWSN::AndesiteWSN() {
    _science   = (unsigned long) 20 * 60 * 1000; // [min]
    _transfer  = (unsigned long) 70 * 60 * 1000; // [min]
    _magfreq   = (unsigned long) 33 * 1000;      // [us] microsec
    _ggfreq    = (unsigned long)100 * 1000;      // [us] microsec
    _orb_start = false;
    _science_mode_state = 0;
}



// Setup wireless sensor nodes
int AndesiteWSN::init() {
	
    // set up SPI slave select pins
    pinMode(RF_CS_PIN, OUTPUT);
    pinMode(SD_CS_PIN, OUTPUT); 
	pinMode(ADS_CS_PIN, OUTPUT);
	pinMode(RF_SHDN_PIN, OUTPUT);	
	pinMode(GPS_ENABLE, OUTPUT);
	  
	Serial.print("*****Setting Chip Selects LOW*****");
    digitalWrite(RF_SHDN_PIN, LOW);
	digitalWrite(RF_CS_PIN, HIGH);
	digitalWrite(ADS_CS_PIN, HIGH);
	digitalWrite(SD_CS_PIN, HIGH);
	digitalWrite(GPS_ENABLE, HIGH);
    delay(1000);
	
	//setup LED
	pinMode(LED_ONE_PIN, OUTPUT);
	pinMode(LED_TWO_PIN, OUTPUT);
	digitalWrite(LED_ONE_PIN, HIGH);
	digitalWrite(LED_TWO_PIN, HIGH);
	delay(200);
	digitalWrite(LED_ONE_PIN, LOW);
	digitalWrite(LED_TWO_PIN, LOW);
	
    digitalWrite(SD_CS_PIN, LOW);
    // Initialize SD card
    while( !SD.begin(SD_CS_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
        SD.initErrorHalt();
        delay(50);
        //resetFunc();
	}
	Serial.println("SD card Initialized");
	
	// Set number of the previous orbit completed to zero
	_File.init();
	digitalWrite(SD_CS_PIN, HIGH);
    // Initialize radio 
	digitalWrite(RF_CS_PIN, LOW);
    while ( _Radio.init() != 0 ) {
        Serial.println("ERROR: Radio setup failed.");
        delay(50);
        //resetFunc();
    }	
	
	//Setup science instruments
	/*
	Wire.begin();
	uint16_t stat = DOF.begin();
	while (stat != 0x49D4){
		Serial.print("Status: ");
		Serial.println(stat,HEX);
		delay(50);
		stat = DOF.begin();
	}
	Serial.println("Gyro Initialized");
	*/
	//setup GPS
	Serial1.begin(ACDH_GPS_BAUD);
	
	SPI.begin();
	//_ADC.initialize();
    delay(1000);
	
	sensors.begin();
    if (!sensors.getAddress(Temp1, 0)) Serial.println("Unable to find address for Device 1");
    if (!sensors.getAddress(Temp2, 1)) Serial.println("Unable to find address for Device 2");
    if (!sensors.getAddress(Temp3, 2)) Serial.println("Unable to find address for Device 3");
    if (!sensors.getAddress(Temp4, 3)) Serial.println("Unable to find address for Device 4");
    
    // Setup LEDs
   // acdh_init_led();
	
    
    // Set led for waiting
    //acdh_led_set(1);
	    // Check for simulated GPS string-in from the computer
	unsigned long timer_start = millis();
	if(!_orb_start){
		Serial.print(":: Waiting for GPS string from computer...");
		while ( !_Orbit.setLatitude() ) {
			if ( (millis() - timer_start) >= GPS_INIT_TIMEOUT ) { break; }
		}
		_orb_start = true;
		    
	}
	
	scienceMode(true);
	listenMuleMessage();
    digitalWrite(LED_ONE_PIN, HIGH);
    digitalWrite(LED_TWO_PIN, HIGH);
	return 0;
}



// // /////////////////////////
// // ///// MISSION MODES /////
// // /////////////////////////

// Check if science mode should be entered
boolean AndesiteWSN::isScienceMode() {

	
    //set state to science mode
    state = 's';

	// Print stuff
    Serial.print("\nOrbit #");
    Serial.println(_Orbit.getOrbit());
	unsigned long timer_start = millis();
	
	
    bool orb_check;
	if(_Orbit._lock){
		// Make sure latitude is above the 65th, and that the file position is 0
		orb_check  = (_Orbit.getLatitude() >= _Orbit.getModeSwitchLatitude());
	}
	else{
		 orb_check = true;
	}
	
    // Check that file does not already have data in it (true if empty)
    bool file_check = true;
        /*
    if ( SD.exists(_File.name().c_str()) ) {
        File handle = SD.open(_File.name().c_str(), O_READ);
         
        if ( !handle ) 
            return true;
        
        file_check = !(handle.size() > 20);
     /*   
        Serial.print("File Size: ");
        Serial.println(handle.size());
        Serial.print("File Check Bool: ");
        Serial.println(file_check);
        Serial.print("Orb Check Bool: ");
        Serial.println(orb_check);
        handle.close();
    }
    */
    Serial.print("Le Both: ");
    Serial.println((orb_check && file_check));
    
    return (orb_check);
}



// Start Science mode
int AndesiteWSN::scienceMode(bool isCalibration) {
    /******************************************
    *****IMPORTANT:: TURN OFF RADIOS HERE******
    ******************************************/

	if(isCalibration){
		_temp_timing = SCIENCE_TEMP_TIMING;
	}
	else{
		_temp_timing = CALIBRATION_TEMP_TIMING;
	}
	//digitalWrite(RF_SHDN_PIN, HIGH);
	RF22.setTimeout(50);
    _ADC.initialize();
    Serial.println(":: Entering Science mode...");
	File _handle;
	String data;
    //acdh_led_set(2);
    double timer_run			= micros();    
    unsigned long timer_now		= millis();
    unsigned long timer_start	= millis();
	unsigned long timer_previous= millis();
	unsigned long timer_diff	= 0;
	uint32_t count_mag			= 0;
    uint32_t count_gg			= 0;
    uint32_t count_temp			= 0;
    boolean recently_wrote		= false;
    boolean done				= false;
	
    TIMSK1 |= (1 << OCIE1A);
	_handle = SD.open(_File._file.c_str(), O_CREAT | O_APPEND | O_WRITE);
	Serial.print("Writing to file: ");
	Serial.println(_File.name());
					
	if ( !_handle ) {
		Serial.println(":: File wr failed.");
		_handle.close();
		return;
	}
    // ADC counter
    // uint8_t count_adc = 0;

    // Enter Science Mode when latitude is ABOVE a certain threshold	
    while ( !done) {
		// // chip select and slave select
        // if ( !count_adc ) { 
        //     // acdh_adc_setup();
        //     count_adc = 1;
        // }
        
        /* 
        // Enter low power mode when in Science Mode for 30 min
        if ( (millis() - timer_start) >= _science ) {
            Serial.println("Science Timeout.");
            delay(2000);
            _File.write();
            _File.write("\n");
            return 1;
        }
        */
		
        switch(_science_mode_state){
            case 1:
				timer_previous = timer_now;
				timer_now		= millis();
				timer_diff = timer_now-timer_previous;
				
                //collect magnetometer data
                data = AndesiteCollect::mag(timer_diff);
                ++count_mag; 
				_handle.println(data);
                _science_mode_state = 0;
                break;
            
            case 2:
				timer_previous = timer_now;
				timer_now		= millis();
				timer_diff = timer_now-timer_previous;				
                //collect magnetometer data
                data = AndesiteCollect::mag(timer_diff);
                ++count_mag; 

                // Collect gyroscope and gps data
                //data += AndesiteCollect::gyro();
				if(_Orbit._lock){
					data += AndesiteCollect::gps();
				}
				++count_gg;
                _handle.println(data);
				_science_mode_state = 0;
                break;
            case 3:
				timer_previous = timer_now;
				timer_now		= millis();
				timer_diff = timer_now-timer_previous;
				Serial.println("TEMP");
				//collect magnetometer data
				data = AndesiteCollect::mag(timer_diff);
				++count_mag;
                //collect temperature sensor data
                data += AndesiteCollect::temp();
                ++count_temp;
				_handle.println(data);
                _science_mode_state = 0;
				break;

            default:
				//Serial.println("hello");
                //Serial.println(_science_mode_state); 
                //check latitude
				if(isCalibration){
					if ( (millis() - timer_start) >= CALIBRATION_TIMEOUT) {
						done = true;
					}
				}
				else{
					if(_Orbit._lock){
						if ( !_Orbit.setLatitude() ) {
							Serial.println("No GPS found.");
						}
						if(_Orbit.getLatitude() <= _Orbit.getModeSwitchLatitude()){
							Serial.println(":: Science Mode.");
							done = true;
						}
					}
					else{
						if ( (millis() - timer_start) >= SCIENCE_MODE_TIMEOUT) {
							done = true;
						}
					}
				}
				_handle.close();
				_handle = SD.open(_File._file.c_str(), O_CREAT | O_APPEND | O_WRITE);
				/*
				//check for file overfow 
				if ( _File._ready_write_bool > 0 ) {
					_File.write(done);

				} 
				*/
				break;
        }
	}
	TIMSK1 = 0;
	_handle.close();
    // Close SD card file after finished gathering data
    //Serial.println(":: Done with Science Mode.");
    
    //_File.store();
	/*
    if ( _File._ready_write_bool > 0 ){
        _File.write(done);
    }
*/
    Serial.print("Science Mode: ");
	timer_run = (micros() - timer_run) / 1e6;
    Serial.print(timer_run);
    Serial.println(" sec");
	
	// Display sampling rate -- ONLY FOR TESTING. REMOVE IN REAL SIMULATION
	double Hm  = (double) count_mag / timer_run;
	double Hgg = (double) count_gg  / timer_run;
	double Ht = (double) count_temp / timer_run;
	Serial.print("Mag Sample Rate = ");
	Serial.print(Hm,3);
	Serial.println(" Hz");
	Serial.print("GG Sample Rate = ");
	Serial.print(Hgg,3);
	Serial.println(" Hz");
    Serial.print("Temp Sample Rate = ");
    Serial.print(Ht,3);
    Serial.print(" Hz");
	
    return 0;
}



// Send confirmation
boolean AndesiteWSN::isTransferMode() {
    
    //set state to data transfer mode
    state = 't';

	if(_Orbit._lock){
		// Check current position (latitude)
		if ( _Orbit.getLatitude() >= _Orbit.getModeSwitchLatitude() ) {
			Serial.print("Latitude is still in science mode");
			_Orbit.waitScienceFinish();
			//return false;
		}
	}
	
    // Check data file sent status
    if ( _Orbit.readHeader(_File.name(), 2) == 1 ) {
        Serial.print("File has already been completed: ");
        Serial.println(_File.name());
        return false;
    }
    
    // Check file size
    if ( _File.size() < 20 ) {
        Serial.print("File size is too small to send: ");
        Serial.println(_File.size());
        return false;
    }
    //digitalWrite(RF_SHDN_PIN, LOW);
	// Initialize radio
	/*if ( _Radio.init() != 0 ) {
		Serial.println("ERROR: Radio setup failed.");
		delay(10000);
		resetFunc();
	}*/
	
	//digitalWrite(LED_ONE_PIN, LOW);
    //digitalWrite(LED_TWO_PIN, LOW);
	if(_Orbit.waitTransferStartCmd(_transfer_start)){
		return true;
	} else{
		return false;
	}
}



// Start Transfer mode
int AndesiteWSN::transferMode() {
    Serial.println(":: Entering Transfer mode...");
    //acdh_led_set(4);
    double timer_start = millis();
    int num;
    if ( _File.send() ) {
	    num = 1;
	    _Orbit.setStatus(num);
	} else {
        num = 0;
        _Orbit.setStatus(num);
    }
    
	Serial.print("File Transfer: ");
	timer_start = (millis() - timer_start) / (double) 1000;
	Serial.print(timer_start);
	Serial.print(" sec");
	Serial.print(" | ");
	Serial.println(num);
    

	File handle = SD.open(_File.name().c_str(), O_WRITE);
	_Orbit.writeHeader(handle);
	handle.close();

    return 0;
}

//In place of data transfer mode. Instead of going straight to Transfer Mode,
//listen for a message coming from the mule.
bool AndesiteWSN::listenMuleMessage() {
	digitalWrite(RF_SHDN_PIN, LOW);
	unsigned long _transfer_start	= millis();
	// Initialize radio
	if ( _Radio.init() != 0 ) {
		Serial.println("ERROR: Radio setup failed.");
		delay(10000);
		//resetFunc();
	}
	// Send finished with Science mode message (had listen timeouts)
	bool scienceMode = false;
	Serial.println(":: Sending done collecting data message to Mule...");
	    
	uint8_t data[]="::ReadyTesting";
	int sent = 0;
	while(!sent && !scienceMode){
		if ( !_Radio.sendCommand(data, sizeof(data), ACDH_MULE_ADDR) ) {
			Serial.println("Send Fail.");
		}
		else{
			Serial.println("Recieved Ack from Mule");
			sent = 1;
		}
		if(_Orbit._lock){
			if ( !_Orbit.setLatitude() ) {
				Serial.println("No GPS found.");
			}
			if(_Orbit.getLatitude() <= _Orbit.getModeSwitchLatitude()){
				Serial.println(":: Done waiting for Mule - Data Transfer mode finished, Progressing to Science Mode");
				scienceMode = true;
			}
		}
		else{
			if ( (millis() - _transfer_start) >= TRANSFER_MODE_TIMEOUT ) {
				Serial.println(":: Done waiting for Mule - Data Transfer mode finished, Progressing to Science Mode");
				scienceMode = true;
			}
		}
	}
	Serial.println(":: Waiting for message from Mule to begin Transfer mode...");
	while(!scienceMode){
		if (_Radio.listen() == ACDH_MULE_ADDR) {
			//First char of message determines WSN action.
			uint8_t Transfer_cmd[]="TransferReady";
			String str_buf((const char*)_Radio._message);
			Serial.print("recieved string: ");
			Serial.println(str_buf);
			String str_data((const char*)Transfer_cmd);
			if (str_buf.equals(str_data))
			{
				transferMode();	
			}
			else{
				Serial.println("::Couldn't parse message received from Mule");
			}
			
			/*
			switch (_Radio._message[0]) {
				case 'T': //switch to data transfer mode
					transferMode();
					break;
				case 'L': //switch to Low Power Mode
					lowPowerMode();
					break;
				case 'R': //Reset WSN components
					//send line[1] to EPS to restart Mag, SD card, Power Com, 
					//Gyro, GPS, or Temp sensor.
					break;
				case 'F': //Retransmit file
					break;
				default:
					break;
			}
			*/
		}
		if(_Orbit._lock){
			if ( !_Orbit.setLatitude() ) {
				Serial.println("No GPS found.");
			}
			if(_Orbit.getLatitude() <= _Orbit.getModeSwitchLatitude()){
				Serial.println(":: Done waiting for Mule - Data Transfer mode finished, Progressing to Science Mode");
				scienceMode = true;
			}
		}
		else{
			if ( (millis() - _transfer_start) >= TRANSFER_MODE_TIMEOUT ) {
				Serial.println(":: Done waiting for Mule - Data Transfer mode finished, Progressing to Science Mode");
				scienceMode = true;
			}
		}
	}
}



// Send health beacon to Mule
void AndesiteWSN::healthBeacon() {
	//state of instruments, state(Science/Transfer)
	char _transmit_message[RF22_MESH_MAX_MESSAGE_LEN];

	//global variable called state is either 's' or 't'


	//_Radio.send((uint8_t*)_transmit_message, (_message_size + 1), ACDH_MULE_ADDR);
}


// Wait for orbit to finish
void AndesiteWSN::wait() {
    
    // This function always returns true...(maybe doesn't need to be boolean?)
    _Orbit.waitOrbitFinish();
    
    Serial.print("Using info file: ");
    Serial.println(_File.name());
    Serial.print("Orbit is: ");
    Serial.println(_Orbit.getOrbit());
     
    // Update orbit values
    if ( _Orbit.update(_File.name()) ) {
        
        // Update file name
        Serial.print("Before file name: ");
        Serial.println(_File.name());
        
        _File.set();
        
        Serial.print("After file name: ");
        Serial.println(_File.name());
        
        // Write new header to new file
        File handle = SD.open(_File.name().c_str(), O_CREAT | O_WRITE);
        _Orbit.writeHeader(handle);
        handle.close();
    }
    
    // _File.setLines(0);
}

void AndesiteWSN::lowPowerMode() {
	//empty shell of a once great function 
    Serial.println("Entering Low Power Mode");
}
