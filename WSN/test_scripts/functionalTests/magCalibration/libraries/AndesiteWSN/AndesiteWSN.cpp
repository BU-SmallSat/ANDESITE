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
	  
	Serial.print("*****Setting Chip Selects LOW*****");
    //digitalWrite(RF_SHDN_PIN, LOW);
	digitalWrite(RF_CS_PIN, HIGH);
	digitalWrite(ADS_CS_PIN, HIGH);
	digitalWrite(SD_CS_PIN, LOW);
    delay(1000);
	
	//setup LED
	pinMode(LED_ONE_PIN, OUTPUT);
	pinMode(LED_TWO_PIN, OUTPUT);
	digitalWrite(LED_ONE_PIN, LOW);
	digitalWrite(LED_TWO_PIN, LOW);
    
    // Initialize SD card
    if ( !SD.begin(SD_CS_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
        SD.initErrorHalt();
        delay(10000);
        resetFunc();
	}
	
    // Initialize radio 
    if ( _Radio.init() != 0 ) {
        Serial.println("ERROR: Radio setup failed.");
        delay(10000);
        resetFunc();
    }

	int val = analogRead(BATTERY_OUT);    // read the input pin
	Serial.print("READING BATTERY OUTPUT:  ");
	Serial.println(val);             // debug value
	
	

	SPI.begin();
	_ADC.initialize();
    delay(1000);
    // Setup science instruments
    //DOF.begin();
    //Wire.begin();
    
    // Setup LEDs
   // acdh_init_led();
	
    // Set number of the previous orbit completed to zero
    _File.init();
    
    // Set led for waiting
    //acdh_led_set(1);
    
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
	
    // Check for simulated GPS string-in from the computer
    if(!_orb_start){
		Serial.print(":: Waiting for GPS string from computer...");
		while ( !_Orbit.setLatitude() ) {
			if ( (millis() - timer_start) >= GPS_INIT_TIMEOUT ) { break; }
		}	
		_orb_start = true;
		
    }
    
    //make sure there is enough battery for science mode
    if(!batteryCheck()){
        return false;
    }
	
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
int AndesiteWSN::scienceMode() {
    /******************************************
    *****IMPORTANT:: TURN OFF RADIOS HERE******
    ******************************************/
	//digitalWrite(RF_SHDN_PIN, HIGH);
	digitalWrite(LED_ONE_PIN, HIGH);
	digitalWrite(LED_TWO_PIN, HIGH);
    _ADC.initialize();
    Serial.println(":: Entering Science mode...");
    Serial.print("Writing to file: ");
    Serial.println(_File.name());
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
					
	if ( !_handle ) {
		Serial.println(":: File wr failed.");
		_handle.close();
		return;
	}
    
    // ADC counter
    // uint8_t count_adc = 0;

    // Enter Science Mode when latitude is ABOVE a certain threshold	
    while ( !done) {
        switch(_science_mode_state){
            case 1:
				timer_previous = timer_now;
				timer_now		= millis();
				timer_diff = timer_now-timer_previous;
				               
				//collect magnetometer data
				data = AndesiteCollect::mag()+","+String(timer_diff,6);
				++count_mag;
				
				Serial.println(data);
				_handle.println(data);
                
                _science_mode_state = 0;
                break;
            
            case 2:
				timer_previous = timer_now;
				timer_now		= millis();		
				timer_diff = timer_now-timer_previous;		
                
				//collect magnetometer data
                data = AndesiteCollect::mag();
                ++count_mag; 

                // Collect gyroscope and gps data
                data += AndesiteCollect::gyro();
                //AndesiteCollect::gps();
                ++count_gg;

				data += ","+String(timer_diff,6);
				Serial.println(data);
				_handle.println(data); //didn't include parity

                _science_mode_state = 0;
                break;
				
            case 3:

				timer_previous = timer_now;
				timer_now		= millis();
				timer_diff = timer_now-timer_previous;
				
                //collect magnetometer data
				data = AndesiteCollect::mag();
                ++count_mag; 

				// Collect gyroscope and gps data
				data+= AndesiteCollect::gyro();
				//AndesiteCollect::gps();
				++count_gg;

				//collect temperature sensor data
				data += AndesiteCollect::temp();
				++count_temp;
				
				data += ","+String(timer_diff,6);
				Serial.println(data);
				_handle.println(data); //didn't include parity
				
                _science_mode_state = 0;
				break;
            default:
				//Serial.println("hello");
                //Serial.println(_science_mode_state); 
                //check latitude
				if(_Orbit._lock){
					if ( !_Orbit.setLatitude() ) { 
						Serial.println("No GPS found."); 
					}   
					if(_Orbit.getLatitude() <= _Orbit.getModeSwitchLatitude()){
						Serial.println(":: Done with Science Mode.");
						done = true;
					}
				}
				else{
					if ( (millis() - timer_start) >= SCIENCE_MODE_TIMEOUT ) { 
						done = true;
					}
				}
				break;
        }
	}
	TIMSK1 = 0;
	_handle.close();
    // Close SD card file after finished gathering data
    //Serial.println(":: Done with Science Mode.");
    
    //_File.store();
    if ( _File._ready_write_bool > 0 ){
        _File.write(done);
    }
    

    Serial.print("Science Mode: ");
	timer_run = (micros() - timer_run) / 1e6;
    Serial.print(timer_run);
    Serial.println(" sec");
	
	// Display sampling rate -- ONLY FOR TESTING. REMOVE IN REAL SIMULATION
	//double Hm  = (double) count_mag / timer_run;
	double Hgg = (double) count_gg  / timer_run;
	double Ht = (double) count_temp / timer_run;
	//Serial.print("Mag Sample Rate = ");
	//Serial.print(Hm,3);
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

    //check if there is sufficient battery for date transfer mode
    if(!batteryCheck()){
        return false;
    }

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
    // Send finished with Science mode message (had listen timeouts)
	_transfer_start = millis();
	Serial.println(":: Sending done collecting data message to Mule...");
	/*
	uint8_t data[]="::Ready";
	int sent = 0;
	while(!sent){
		if ( !_Radio.sendCommand(data, sizeof(data), ACDH_MULE_ADDR) ) {
			Serial.println("Send Fail.");
		}
		else{
			sent = 1;
		}
	}
	*/
	//digitalWrite(LED_ONE_PIN, LOW);
    //digitalWrite(LED_TWO_PIN, LOW);
	if(_Orbit.waitTransferStartCmd(_transfer_start)){
		return listenMuleMessage();
	} else{
		return false;
	}
}



// Start Transfer mode
int AndesiteWSN::transferMode() {
    Serial.println(":: Entering Transfer mode...");
    //acdh_led_set(4);
    double timer_start = millis();
    int num = 0;
    
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
	Serial.println(":: Waiting for message from Mule to begin Transfer mode...");
	bool scienceMode = false;
	while(!scienceMode){
		if (_Radio.listen() == ACDH_MULE_ADDR) {
			Serial.print(":: Message received from Mule - ");
			Serial.println(_Radio._message);
			//First char of message determines WSN action.
			uint8_t Transfer_cmd[]="Transfer";
			if(_Radio._message == Transfer_cmd){
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
				scienceMode = true;
			}
		}
	}
}



// Send health beacon to Mule
void AndesiteWSN::healthBeacon() {
	//Send battery level, state of instruments, state(Science/Transfer)
	char _transmit_message[RF22_MESH_MAX_MESSAGE_LEN];
	bool battLevel = batteryCheck();

	//global variable called state is either 's' or 't'


	//_Radio.send((uint8_t*)_transmit_message, (_message_size + 1), ACDH_MULE_ADDR);
}


// Wait for orbit to finish
void AndesiteWSN::wait() {
    
    // This function always returns true...(maybe doesn't need to be boolean?)
    if ( !_Orbit.waitOrbitFinish() ) return;
    
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
bool AndesiteWSN::batteryCheck(){
    Serial.println("in battery check");
    batteryLevel = 100;  //edit to get this value from actual WSN battery - EPS?
    switch (state){
        case 's':
            if(batteryLevel > 60){ //edit with percent battery required for science mode
                Serial.println("SUFFICIENT BATTERY FOR SCIENCE MODE");
                return true;
            } 
            else {
                return false;
            }
            break;
        case 't':
            if(batteryLevel > 75){  //edit with percent battery required for data transfer mode
                Serial.println("SUFFICIENT BATTERY FOR DATA TRANSFER MODE");
                return true;
            }
            else {
                return false;
            }
            break;
        default:
            //enter error checking case here
            return false;
            break;
    }
}

void AndesiteWSN::lowPowerMode() {
	//empty shell of a once great function 
    Serial.println("Entering Low Power Mode");
}
