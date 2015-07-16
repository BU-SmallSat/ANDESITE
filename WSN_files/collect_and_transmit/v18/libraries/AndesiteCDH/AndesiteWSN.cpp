// ========
// Includes
// ========

#include "AndesiteWSN.h"



// //////////////////////////////
// ///// ANDESITE WSN CLASS /////
// //////////////////////////////

// void(* resetFunc) (void) = 0;

// Initialize the wireless sensor node class
AndesiteWSN::AndesiteWSN() {
    _science  = 20 * 60;    // minutes
    _transfer = 70 * 60;    // minutes
    _magfreq  = 30;  // microseconds
    _ggfreq   = 100; // microseconds
}



// Setup wireless sensor nodes
int AndesiteWSN::init() {
	
    // Turn on radios
    pinMode(ACDH_SS_PIN, OUTPUT);
    pinMode(4, OUTPUT);    
    digitalWrite(4, LOW);
    delay(2000);
    
    // Setup ADC
    // acdh_adc_setup();
    
    // Initialize SD card
    if ( !SD.begin(ACDH_SD_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
        delay(6000);
        // resetFunc();
	}
	
    // Initialize radio 
    if ( _Radio.init() != 0 ) {
        Serial.println("ERROR: Radio setup failed.");
        delay(6000);
        // resetFunc();
    }
	
    // Setup science instruments
    DOF.begin();
    Wire.begin();
    
    // Setup LEDs
    acdh_init_led();
	
    // Set number of the previous orbit completed to zero
    _File.init();
    
    // Set led for waiting
    acdh_led_set(1);
    
	return 0;
}



// // /////////////////////////
// // ///// MISSION MODES /////
// // /////////////////////////

// Check if science mode should be entered
boolean AndesiteWSN::isScienceMode() {
    
	// Print stuff
    Serial.print("\nOrbit #");
    Serial.println(_Orbit._orbit);
	
	// Check for simulated GPS string-in from the computer
    if ( !_Orbit.setLat() ) {
        Serial.print(":: Waiting for GPS string from computer...");
        while ( !_Orbit.setLat() ) {}
        Serial.println("Done.");
    }
    
    // Make sure latitude is above the 65th, and that the file position is 0
    bool orb_check  = (_Orbit._latitude >= _Orbit._lat_limit);
    
    // Check that file does not already have data in it (true if empty)
    bool file_check = true;
    
    if ( SD.exists(_File.name().c_str()) ) {
        File handle = SD.open(_File.name().c_str(), O_READ);
        
        if ( !handle ) 
            return true;
        
        file_check = !(handle.size() > 20);
        
        Serial.print("Le File Size: ");
        Serial.println(handle.size());
        Serial.print("Le Bool: ");
        Serial.println(file_check);
        
        handle.close();
    }
    
    Serial.print("Le Both: ");
    Serial.println((orb_check && file_check));
    
    return ( orb_check && file_check );
}



// Start Science mode
int AndesiteWSN::scienceMode() {
    
	// Update the data file name
	// _File.set();
    // _File.setLines(0);
	
	// Display current mode
	acdh_led_set(2);
	
    // Timer variables 
    double timer_run = micros();    
    unsigned long timer_mag   = 0, 
        timer_gg              = 0,
        timer_now             = 0,
        timer_lat             = millis(), 
        timer_start           = millis();
	
    // ADC counter
    uint8_t count_adc = 0;
	uint32_t count_mag = 0,
			 count_gg = 0;
    
    // Enter Science Mode when latitude is ABOVE a certain threshold	
	Serial.println(":: Entering Science mode...");
    
    while ( _Orbit._latitude >= _Orbit._lat_limit ) {
        
		// chip select and slave select
        if ( !count_adc ) { 
            // acdh_adc_setup();
            count_adc = 1;
        }
        
        // Enter low power mode when in Science Mode for 30 min
        if ( (millis() - timer_start) >= ((unsigned long)_science*1000) ) {
            Serial.println("Timeout.");
            delay(2000);
            _File.write("\n");
            _File.close();
            return 1;
        }
        
        // Collect magnetometer data
        if ( ((timer_now=micros()) - timer_mag) >= (_magfreq*1000) ) {
            AndesiteCollect::mag();
            timer_mag = timer_now;
			++count_mag;
        }
        
        // Collect gyroscope and gps data
        if ( ((timer_now=micros()) - timer_gg) >= (_ggfreq*1000) ) {
            AndesiteCollect::gyro();
            AndesiteCollect::gps();
            timer_gg = timer_now;
			++count_gg;
        }
		
        // Read in GPS coordinates
        if ( (millis() - timer_lat) >= 1000 ) {
            timer_lat = millis();
            
            if ( !_Orbit.setLat() ) {
                Serial.println("No GPS found.");
                _File.write("\n");
                _File.close();
                return 1;
            }
        }
    }
	
    // Close SD card file after finished gathering data
    Serial.println(":: Done with Science Mode.");
    _File.write("\n");
    _File.close();
    
    Serial.print("Science Mode: ");
	timer_run = (micros() - timer_run) / 1e6;
    Serial.print(timer_run);
    Serial.println(" sec");
	
	// Display sampling rate
	double Hm  = (double) count_mag / timer_run;
	double Hgg = (double) count_gg  / timer_run;
	
	Serial.print("Mag Sample Rate = ");
	Serial.print(Hm);
	Serial.println(" Hz");
	Serial.print("GG Sample Rate = ");
	Serial.print(Hgg);
	Serial.println(" Hz");
	
    return 0;
}



// Send confirmation
boolean AndesiteWSN::isTransferMode() {
    
    // Check current position (latitude)
    if ( _Orbit._latitude >= _Orbit._lat_limit ) {
        Serial.println("Latitude is still in science mode.");
        _Orbit.waitScience();
        return false;
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
    
    // Send finished with Science mode message (had listen timeouts)
    Serial.print(":: Sending done collecting data message to Mule...");
	
    if ( !_Radio.send(&_Radio._cmd_start, sizeof(uint8_t), ACDH_MULE_ADDR) ) {
        Serial.println("Send Fail.");
        return false;
    } else
        Serial.println("Done.");
	
	// Display current mode
	acdh_led_set(3);
    
    // Enter Data Transfer Mode when latitude is BELOW a certain threshold	
	Serial.println(":: Waiting for message from Mule...");
	unsigned long timer_start = millis();
    
    while ( _Orbit._latitude < _Orbit._lat_limit ) {  
		
        // Wait for command from Mule to start sending data to it
        if ( _Radio.listen() == ACDH_MULE_ADDR ) {
            Serial.println("Done.");
            return true;
        }
        
		timer_start = _Orbit.latIntervalPrint(timer_start, 1000);
        
		// GPS string poll (maybe add a timer here if necessary)
		if ( !_Orbit.setLat() ) {
			Serial.println("GPS Fail.");
			return false;
		}
    }
	
    return false;
}



// Start Transfer mode
int AndesiteWSN::transferMode() {
	
	// Display current mode
	acdh_led_set(4);
	
	// Send data from WSN to Mule
	Serial.println(":: Entering Transfer mode...");
    double timer_start = millis();
    
    if ( _File.send() )
        _Orbit._status = 1;
    else
        _Orbit._status = 0;
    
	Serial.print("File Transfer: ");
	timer_start = (millis() - timer_start) / (double) 1000;
	Serial.print(timer_start);
	Serial.print(" sec");
	Serial.print(" | ");
	Serial.println(_Orbit._status);
    
    File handle = SD.open(_File.name().c_str(), O_WRITE);
    _Orbit.writeHeader(handle);
    handle.close();
    
    return 0;
}



// Wait for orbit to finish
void AndesiteWSN::wait() {
    
    if ( _Orbit.wait() ) {
        Serial.print("Using info file: ");
        Serial.println(_File.name());
        Serial.print("Orbit is: ");
        Serial.println(_Orbit._orbit);
        
        // Update orbit values
        if ( _Orbit.update(_File.name()) == 0 ) {
            
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
        
        _File.setLines(0);
        
    }
    
}
