// ========
// Includes
// ========

#include "AndesiteFile.h"



// ///////////////////////////////
// ///// ANDESITE FILE CLASS /////
// ///////////////////////////////

// Initialize file class
AndesiteFile::AndesiteFile() {
    _handle;
    _file  = "001.txt";
    _lines = 0;
    _limit = 500;
}



// Initialize the orbit file (not reading the header, so when power cycling, starts at Orbit 1)
boolean AndesiteFile::init() {
    
    // Read status field in data file header to check if a file has not sent all its 
    //   data. Loop will stop on an empty file or a file that has not finished sending
    //   all its data
    while ( _Orbit.readHeader(_file, 2) == 1 ) {
        Serial.print(":: File has data = ");
        Serial.println(_file);
        
        _Orbit._orbit = (_Orbit._orbit + 1);
        AndesiteFile::set();
    }
    
	// Do nothing if file already exists
    if ( SD.exists( _file.c_str() ) ) {
        Serial.print(":: File exists = ");
        Serial.println(_file.c_str());
        return true;
    }
    
    // Open up orbit info file
    File handle = SD.open(_file.c_str(), O_CREAT | O_WRITE);
    
	Serial.print("Setting starting orbit value...");
	if ( !handle ) { 
		Serial.println("Fail.");
        handle.close();
		return false;
	}
	Serial.println("Done.");
	
	// Set the initial orbit number 
    _Orbit.writeHeader(handle);
	handle.close(); 
    
    return true;
}



// //////////////////////////////////
// ///// SEND DATA FILE TO MULE /////
// //////////////////////////////////

// Read a file on the SD card and send it to the Mule
boolean AndesiteFile::send() {
    
    // Initialize string variables
    String line = "";
    char letter;
    unsigned long len = 0, 
        compr_len     = 0, 
        // Determine this mathematically, not sure why specifically this works
        len_limit     = 2 * (RF22_MESH_MAX_MESSAGE_LEN - 5),
        timer_start = millis();
	
    uint8_t convline[RF22_MESH_MAX_MESSAGE_LEN];
	const char *str;
    
    // Open SD card file for reading
	_handle = SD.open(_file.c_str(), O_RDWR);
    _lines  = 1;
    
    // Make sure SD card file was opened correctly
    if ( !_handle ) {
        Serial.print("ERROR: File '");
        Serial.print(_file);
        Serial.println("' was not found");
        
		// Done sending file to Mule (b/c couldn't be found)
		AndesiteFile::done();
        _handle.close();
        
		return false;
    }
    
    // Loop through every char in the file (check if reached end of file, 
	//     update some global variable to true/false, print data on orbit 
	//     to file regarding where file reading ended)
    unsigned long pos = _Orbit._position;
    Serial.println(":: Reading and sending file...");
    Serial.print(":: Seeking to position '");
    Serial.print(pos);
    Serial.print("'...");
    
    // if ( pos <= 13 ) {
    //     pos = 14;
    //     Serial.print("jk seeking to 14...");
    // }
    
    Serial.println(_handle.seek(pos));
    
    while ( _handle.available() > 0 ) {
        
        // Next char on the current file line
        letter = _handle.read();
		pos    = _handle.position();
		
        // Check for invalid chars and update GPS values
        if ( ((millis() - timer_start) >= 2000) || (letter <= 0) ) {
            if ( !_Orbit.setLat() || (_Orbit._latitude >= _Orbit._lat_limit) )
                break;
            Serial.println("File fail: Hi!");
			if ( letter <= 0 )
				continue;
			else
				timer_start = millis();
        }
		
		// Check for valid characters (not end of line chars)
		if ( (letter != '\n') && (letter != '\r') ) {
			line += letter; 
			++len;
		}
        
		// Send file line to server
        if ( (len == len_limit) || ((letter == '\n') && (len > 0)) ) {
            
            // File line
			str = line.c_str();
			compr_len = acdh_str_compress(convline, str, len, RF22_MESH_MAX_MESSAGE_LEN);
			
            Serial.print(F("Sending data -- "));
			Serial.print(str);
			Serial.print(" ");
			Serial.print(compr_len);
			Serial.print(" ");
			Serial.print(len);
            Serial.print(" | ");
            Serial.println(_Orbit._latitude);
			
			if ( ((compr_len % 2) == 1) && (compr_len < RF22_MESH_MAX_MESSAGE_LEN) )
				++compr_len;
			
            // Send file line to server 
            _Radio.send(convline, compr_len, ACDH_MULE_ADDR);
			
            // Reset variables
            _lines++;
            line = "";
            len = 0;
			timer_start = millis();
            
            // Update file position every 10th line sent 
            if ( (_lines % 10) == 0 ) {
                _lines = 0;
                _Orbit._position = pos;
                _Orbit.writeHeader(_handle);
                
                Serial.print(":: Seeking back to position '");
                Serial.print(pos);
                Serial.print("'...");
                Serial.println(_handle.seek(pos));
            }
            
            // Update orbit values
            if ( !_Orbit.setLat() || (_Orbit._latitude >= _Orbit._lat_limit) ) {
                Serial.println(":: NOT done sending file.");
                AndesiteFile::done();
                _handle.close();
                return false;
            }
        }
    }
	
	// Done sending file to Mule
	Serial.println(":: Done sending file.");
	AndesiteFile::done();
    _handle.close();
    
	return true;
}



// Tell Mule that file sending is done
void AndesiteFile::done() {
    
    // Send a finish statement at the end of each file read 
    _Radio.sendCommand(&_Radio._cmd_done, sizeof(uint8_t), ACDH_MULE_ADDR);
    
    // Set file position value once done sending file (not sure that I want this)
    // if ( _handle.size() == _Orbit._position )
    //     _Orbit.setPos(0);
    
    // Write information to file
    _Orbit.writeHeader(_handle);
    
	return;
}



// //////////////////////////////
// ///// WRITE TO DATA FILE /////
// //////////////////////////////

// Write data to file
void AndesiteFile::write(String data) {
    _handle.print(data);
    _lines++;
}



// Close the file
void AndesiteFile::close() {
    _handle.close();
    _lines = 0;
}



// Open file every # of line writes
boolean AndesiteFile::status() {
    
    // Limit number of file lines added to file per Open/Close cycle
    if ( (_lines % _limit) == 0 ) {
        if ( _lines ) 
            _handle.close();
        
        _handle = SD.open(_file.c_str(), O_CREAT | O_APPEND | O_WRITE);
        _lines = 0;
        
        // Could not open file
        if ( !_handle )
            return false;
    }
    
    return true;
}



// //////////////////////////
// ///// DATA FILE NAME /////
// //////////////////////////

// Return name of file
String AndesiteFile::name() {
    return _file;
}



// Return orbit number from file
unsigned int AndesiteFile::orbit() {
    int hun = _file.charAt(0) - '0';
    int ten = _file.charAt(1) - '0';
    int one = _file.charAt(2) - '0';
    
    return (100*hun + 10*ten + one);
}    




// Name the next file to write to
void AndesiteFile::set() {
    int orb = _Orbit._orbit;
    
    int hun = (orb/100),
        ten = ((orb/10) - (hun*10)),
        one = ((orb/1) - (hun*100) - (ten*10));
    
    _file.setCharAt(0, hun+'0');
    _file.setCharAt(1, ten+'0');
    _file.setCharAt(2, one+'0');
	
    // Serial.print(":: Setting file name from orbit: ");
    Serial.print(orb);
    Serial.print(" | Chars = (hun)");
    Serial.print(hun);
    Serial.print(" (ten) ");
    Serial.print(ten);
    Serial.print(" (one) ");
    Serial.print(one);
    Serial.print(" => ");
    Serial.println(_file);
    
	return;
}



// ////////////////////////////////
// ///// DATA FILE ATTRIBUTES /////
// ////////////////////////////////

// Return the number of lines written to the file
unsigned int AndesiteFile::getLines() {
    return _lines;
}




// Set the number of lines
void AndesiteFile::setLines(unsigned int num) {
    _lines = num;
}



unsigned long AndesiteFile::size() {
    _handle = SD.open(_file.c_str(), O_READ);
    unsigned long size = _handle.size();
    _handle.close();
    
    return size;
}
