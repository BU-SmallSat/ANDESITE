// ========
// Includes
// ========

#include "AndesiteOrbit.h"



// ////////////////////////////////
// ///// ANDESITE ORBIT CLASS /////
// ////////////////////////////////

// Initialize Andesite Orbit class
AndesiteOrbit::AndesiteOrbit() {
    _latitude    = 0;
    _lat_limit   = 65;
    _orbit       = 1;
    _status      = 0;
    _timeout     = 1000;
    _update_fail = 0;
}



// //////////////////////////////////////////
// ///// READ/WRITE TO DATA FILE HEADER /////
// //////////////////////////////////////////

// Zero pad the data file header fields
String AndesiteOrbit::zeroPadFields() {
    
    // Data fields
    String part1 = String(_orbit),
        part2    = String(_position),
        part3    = String(_status),
        allparts = "";
    
    // Set initial values (on first run)
    unsigned int  orb = _orbit;
    unsigned long pos = _position;
    
    if ( orb == 0 )
        orb = 1;
    
    if ( pos == 0 )
        pos = 1;
    
    // Zero pad the fields
    while ( (orb=orb*10) < 1e3 ) 
        part1 = "0" + part1;
    
    while ( (pos=pos*10) < 1e7 ) 
        part2 = "0" + part2;
    
    // Return the zero padded string
    allparts = part1 + "," + part2 + "," + part3 + "|";
    
    return allparts;
}



// Return specified field in orbit information file
unsigned long AndesiteOrbit::readHeader(String file, int field) {
    
    // File attributes
    int loc    = 0;
    char sep   = ',',
        end    = '|',
        letter = '\0';
    
    // Check that the file opened
    File handle = SD.open(file.c_str(), O_READ);
    
    if ( !handle ) {
        Serial.println("Orbit READ was unsuccesful!");
        handle.close();
        return -1;
    }
    
    // Get the specified field from the file
    String str = "";
    
    while ( (handle.available() > 0) && (loc <= field) ) {
        letter = handle.read();
        
        if ( (letter == end) || (str.length() >= 7) )
            break;
        
        if ( letter == sep ) {
            ++loc;
            continue;
        }
        
        if ( (letter <= 0) || (letter == '\r') || (letter == '\n') )
          continue;
        
        if ( (field == loc) )
            str += letter;
    }
    
    // Close file
    handle.close();
    
    return (long) str.toInt();
}



// Set the header of the orbit data file
boolean AndesiteOrbit::writeHeader(File handle) {
    Serial.print("Orbit Info: ");
	Serial.print(_orbit);
    Serial.print(",");
    Serial.print(_position);
    Serial.print(",");
    Serial.println(_status);
    
	if ( !handle ) {
        Serial.println("Orbit WRITE was unsuccesful!");
        return false;
    }
    
    // Write the header of the orbit file
    String buf = AndesiteOrbit::zeroPadFields();
    Serial.print("Writing to orbit file, info: ");
    Serial.println(buf);
    
    handle.seek(0);
    handle.print(buf);
    
    return true;
}



// /////////////////////////////////////
// ///// UPDATE HEADER EVERY ORBIT /////
// /////////////////////////////////////

// Update orbit file containing value of current orbit
unsigned int AndesiteOrbit::update(String file) {
	
    // Open up orbit info file (check for file open fails globally)
    // File handle;
    // if ( _update_fail > 5 ) 
    //     handle = SD.open(file.c_str(), O_CREAT | O_TRUNC | O_WRITE);
    // else
    //     handle = SD.open(file.c_str(), O_CREAT | O_WRITE);
    
    // // Check status of opened file
	// if ( !handle ) {
    //     Serial.println(":: Unable to update file.");
        
    //     // Stop trying this orbit and go to next orbit
    //     if ( _update_fail > 10 )
    //         ++_orbit;
        
    //     // Opening file has failed, keep track of this        
    //     ++_update_fail;
    //     handle.close();
        
    //     return 0;
    // } else {
        
    //     // Opening file has failed many times, write fresh data to file and be done with it
    //     if ( _update_fail ) {
    //         if ( _update_fail > 5 )
    //             AndesiteOrbit::writeHeader(handle);
    //         _update_fail = 0;
    //         header.close();
    //         return 2;
    //     }
    //     header.close();
    // } 
    
    // Get data file header information
	unsigned int orb  = AndesiteOrbit::readHeader(file, 0);
    unsigned long pos = AndesiteOrbit::readHeader(file, 1);
    unsigned int stat = AndesiteOrbit::readHeader(file, 2);
    
    // Serial.print("Info SIZE = ");
    // Serial.println(handle.size());
    Serial.print("Info Orb = ");
    Serial.println(orb);
    Serial.print("Info Pos = ");
    Serial.println(pos);
    Serial.print("Info Stat = ");
    Serial.println(stat);
    Serial.println("Done.");
    
	// Setup file for next orbit Write value to file, file contains current orbit number
    if ( stat ) {
        ++_orbit;
        _position = 0;
        _status = 0;
        return 0;
    } 
    
    return 1;
}



// //////////////////////////
// ///// ORBIT LATITUDE /////
// //////////////////////////

// Print the latitude at the given time interval
unsigned long AndesiteOrbit::latIntervalPrint(unsigned long now, int interval) {
    if ( (millis() - now) >= interval ) {
        Serial.print("Latitude = ");
        Serial.println(_latitude);
        now = millis();
    }
    
    return now;
}



// Set the orbit latitude
boolean AndesiteOrbit::setLat() {
	
    // Function timeout reference
    unsigned long timer_start = millis();
	
	// Read in data from the serial port
	while (1) {
        
		while ( Serial1.available() > 0 ) {
			char c = Serial1.read();
            
			if ( c > -1 ) {
				if ( GPS.encode(c) ) {
					_latitude = GPS.location.lat();
					return true;
				}
			}
			
			if ( (millis() - timer_start) >= _timeout )
				return false;
        }
        
		if ( (millis() - timer_start) >= _timeout )
			return false;
    }
	
	return false;
}



// ///////////////////////
// ///// GPS TESTING /////
// ///////////////////////

// Count how many times opening a file has failed
unsigned int AndesiteOrbit::getFail() {
    if(!SD.open()){
        _update_fail++;
    }
    return _update_fail;
}



// Wait for orbit to finish
boolean AndesiteOrbit::wait() {
	
	// Display current mode
	acdh_led_set(1);
	
	// Wait for orbit to complete
	Serial.println(":: Waiting for orbit to finish...");
    unsigned long timer_start = millis();
    
	while ( _latitude < _lat_limit ) {
      
        timer_start = latIntervalPrint(timer_start, 1000);
        
		if ( !AndesiteOrbit::setLat() ) {
			Serial.println("GPS Fail.");
			return false;
		}
	}
	
	Serial.println("Done.");
	
	// Orbit number peaked and is too big, do nothing
    if ( _orbit >= 999 ) {
        Serial.print("Orbit number is at its max value: ");
        Serial.println(_orbit);
        while ( _orbit >= 999 ) {}
    }
    
    return true;
}



// Wait for Science mode to finish
boolean AndesiteOrbit::waitScience() {
	
	// Display current mode
	acdh_led_set(1);
	
	// Wait for orbit to complete
	Serial.println(":: Waiting for Science Mode to finish...");
    unsigned long timer_start = millis();
    
	while ( _latitude >= _lat_limit ) {
      
        timer_start = latIntervalPrint(timer_start, 1000);
        
		if ( !AndesiteOrbit::setLat() ) {
			Serial.println("GPS Fail.");
			return false;
		}
	}
	
	Serial.println("Done.");
	
	// Orbit number peaked and is too big, do nothing
    if ( _orbit >= 999 ) {
        Serial.print("Orbit number is at its max value: ");
        Serial.println(_orbit);
        while ( _orbit >= 999 ) {}
    }
    
    return true;
}
