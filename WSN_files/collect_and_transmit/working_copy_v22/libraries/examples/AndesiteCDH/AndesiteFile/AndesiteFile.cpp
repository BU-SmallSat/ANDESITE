// **********************************************************************************
// 
// Name:    AndesiteFile.cpp
// Class:   <AndesiteFile>
// Author:  Gabriel Gonzalez
// 
// Description: The interface for writing and storing data on the ANDESITE Satellite.
//              
// Notes: None.
// 
// **********************************************************************************


// ========
// Includes
// ========

#include "AndesiteFile.h"
#include "libandesite.h"
#include "AndesiteData.h"
#include "AndesiteOrbit.h"
#include "AndesiteRadio.h"



// ///////////////////////////////
// ///// ANDESITE FILE CLASS /////
// ///////////////////////////////

// Initialize file class
AndesiteFile::AndesiteFile() {
    _file            = "001.txt";
    _data            = "";
    _num_files       = 0;
    _num_unfinished  = 0;
    _last_orbit      = 0;
    _failure         = 0;
    _data_limit      = 500;
    _header_size     = 20;
    _unfinished_size = 5; // If you change this, change array size in header too
    
    // C++ has issues with arrays in constructor, this should make things better
    _unfinished_arr[0] = 1;
    _unfinished_arr[1] = 0;
    _unfinished_arr[2] = 0;
    _unfinished_arr[3] = 0;
    _unfinished_arr[4] = 0;
}



// Initialize the orbit file, starting the data file on the correct orbit, regardless
//   of system poweroff
// Tested on 5/14/15: Works fine when on. If you turn off and turn back on, it fails 
//                    on "Setting starting orbit value"
boolean AndesiteFile::init() {
    // _num_files = getNumberOfFiles();
    setUnfinishedFiles();
    
    // Put last orbit in first element
    unsigned int start_orbit = _unfinished_arr[0];
    
    Serial.print("Number of Files: ");
    Serial.println(_num_files);
    Serial.print("Start Orbit: ");
    Serial.println(start_orbit);
    Serial.print("Last Orbit: ");
    Serial.println(_last_orbit);
    Serial.println("IF ( start_orbit < last_orbit )");
    Serial.println("\tSetting Last_Orbit + 1");
    Serial.println("ELSE");
    Serial.println("\tSetting Start_Orbit");
    
    if ( start_orbit < _last_orbit )
        _Orbit.setOrbit( _last_orbit + 1 );
    else
        _Orbit.setOrbit( start_orbit );
    
    AndesiteFile::set();
    
    // Open up orbit info file
    File handle = SD.open(_file.c_str(), O_CREAT | O_WRITE);
    
	Serial.print("Setting starting orbit value...");
	if ( !handle ) { 
		Serial.println("Fail.");
        ++_failure;
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
    String line               = "";
    unsigned int len          = 0;
    unsigned int compr_len    = 0;
    unsigned long len_limit   = 2 * (RF22_MESH_MAX_MESSAGE_LEN - 5); // Determine why this limit works mathematically
    unsigned int num_sent     = 0;
    unsigned int num_bad      = 0;
    unsigned long timer_start = millis();
    
    
    uint8_t convline[RF22_MESH_MAX_MESSAGE_LEN];
	const char *str;
    char letter;
    
    // Open SD card file for reading
	File handle = SD.open(_file.c_str(), O_RDWR);
    
    // Make sure SD card file was opened correctly
    if ( !handle ) {
        Serial.print("ERROR: File '");
        Serial.print(_file);
        Serial.println("' was not found");
        ++_failure;
        
		// Done sending file to Mule (b/c couldn't be found)
        handle.close();
		AndesiteFile::done();
        
		return false;
    }
    
    // Loop through every char in the file (check if reached end of file, 
	//     update some global variable to true/false, print data on orbit 
	//     to file regarding where file reading ended)
    unsigned long pos = _Orbit.getPosition();
    Serial.print(":: File size = ");
    Serial.println(handle.size());
    Serial.println(":: Reading and sending file...");
    Serial.print(":: Seeking to position '");
    Serial.print(pos);
    Serial.print("'...");
    Serial.println(handle.seek(pos));
    
    while ( handle.available() > 0 ) {
        
        // Next char on the current file line
        letter = handle.read();
		pos    = handle.position();
		
        // Check for invalid chars and update GPS values
        if ( ((millis() - timer_start) >= 2000) || (letter <= 0) ) {
            _Orbit.setLatitude();
            
            if ( (_Orbit.getLatitude() >= _Orbit.getModeSwitchLatitude()) || (num_bad >= 10) ) {
                Serial.println("File fail: Hi!");
                break;
			}
            
            if ( letter <= 0 ) {
                Serial.println("File letter negative");
                ++num_bad;
				continue;
            }
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
			compr_len = acdh_str_compress(convline, 
                                          line.c_str(), 
                                          len, 
                                          RF22_MESH_MAX_MESSAGE_LEN);
			
            Serial.print(F("Sending data -- "));
			Serial.print(line.c_str());
			Serial.print(" ");
			Serial.print(compr_len);
			Serial.print(" ");
			Serial.print(len);
            Serial.print(" | ");
            Serial.println(_Orbit.getLatitude());
			
			if ( ((compr_len % 2) == 1) && (compr_len < RF22_MESH_MAX_MESSAGE_LEN) )
				++compr_len;
			
            // Send file line to server 
            _Radio.send(convline, compr_len, ACDH_MULE_ADDR);
			
            // Reset variables
            line        = "";
            len         = 0;
			timer_start = millis();
            ++num_sent;
            
            // Update file position every 10th line sent 
            if ( (num_sent % 10) == 0 ) {
                num_sent = 0;
                _Orbit.setPosition(pos);
                _Orbit.writeHeader(handle);
                
                Serial.print(":: Seeking back to position '");
                Serial.print(pos);
                Serial.print("'...");
                Serial.println(handle.seek(pos));
            }
            
            // Update orbit values
            _Orbit.setLatitude();
            if ( _Orbit.getLatitude() >= _Orbit.getModeSwitchLatitude() ) {
                Serial.println(":: NOT done sending file.");
                handle.close();
                AndesiteFile::done();
                return false;
            }
        }
    }
	
	// Done sending file to Mule
	Serial.println(":: Done sending file.");
    handle.close();
	AndesiteFile::done();
    
	return true;
}



// Tell Mule that file sending is done
void AndesiteFile::done() {
    _Radio.sendCommand(&_Radio._cmd_done, sizeof(uint8_t), ACDH_MULE_ADDR);
    // Write header in here?
    // _Orbit.writeHeader(handle)?
}



// //////////////////////////////
// ///// WRITE TO DATA FILE /////
// //////////////////////////////

// Write the stored data to the file
void AndesiteFile::write() {
    File handle = SD.open(_file.c_str(), O_CREAT | O_APPEND | O_WRITE);
    
    if ( !handle ) {
        ++_failure;
        Serial.println(":: File wr failed.");
        handle.close();
        return;
    }
    
    handle.print(_data);
    handle.close();
    _data = "";
}



// Check storage status to see if it has reached its max length
boolean AndesiteFile::status() {
    if ( _data.length() >= _data_limit ) 
        return true;
    else
        return false;
}



// //////////////////////////
// ///// DATA FILE NAME /////
// //////////////////////////

// Name the next file to write to
void AndesiteFile::set() {
    int orb = _Orbit.getOrbit();
    int hun = (orb/100);
    int ten = ((orb/10) - (hun*10));
    int one = ((orb/1) - (hun*100) - (ten*10));
    
    _file.setCharAt(0, hun+'0');
    _file.setCharAt(1, ten+'0');
    _file.setCharAt(2, one+'0');
	
    Serial.print(":: Setting file name from orbit: ");
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



// Return the size of the data file
unsigned long AndesiteFile::size() {
    File handle        = SD.open(_file.c_str(), O_READ);
    
    if ( !handle ) {
        ++_failure;
        handle.close();
        return -1;
    }
    
    unsigned long size = handle.size();
    handle.close();
    
    return size;
}



// Return orbit number from file
unsigned int AndesiteFile::orbit() {
    int hun = _file.charAt(0) - '0';
    int ten = _file.charAt(1) - '0';
    int one = _file.charAt(2) - '0';
    
    return (100*hun + 10*ten + one);
}    


// Return orbit number from file
String AndesiteFile::strOrbitFile(unsigned int orb) {
    String str;
    int hun = (orb/100);
    int ten = ((orb/10) - (hun*10));
    int one = ((orb/1) - (hun*100) - (ten*10));
    
    str = String(hun) + String(ten) + String(one) + ".txt";
    
    return str;
}    



// ////////////////////////////////
// ///// DATA FILE ATTRIBUTES /////
// ////////////////////////////////

// Search throught the SD card root directory and count the number of files 
//   (where files equates to orbits)
void AndesiteFile::setUnfinishedFiles() {
    int orb = 1;
    _num_unfinished  = 0;
    while( SD.exists( strOrbitFile(orb).c_str() ) ) {
        if(_Orbit.readHeader( strOrbitFile(orb), 2) == 0) {
            Serial.print("GetNumberofUnfinishedFiles: ");
            Serial.println(strOrbitFile(orb));
            ++_num_unfinished;
        }        
        _last_orbit = orb;
        ++orb;
    }
    _num_files = _last_orbit;
    unsigned int i = 1;
    int count = 0;
    int index = 0;
    while(i <= _num_files){
        if(_Orbit.readHeader( strOrbitFile(i), 2 ) ==0) {
            ++count;
            index = _num_unfinished - count;
            if(index < _unfinished_size) {
                i = _unfinished_arr[index];
                Serial.println(i);
            }
        }
        ++i; 
    }

 
        // if ( (_Orbit.readHeader(_file, 2) == 0) && (entry.size() >= _header_size) ) {
        //     Serial.print(entry.name());
        //     Serial.print(" | Count = ");
        //     Serial.println(count);
        //     // Hopefully extension (.txt) does not mess up conversion
        //     // Count can go over unfinished size, fix this
        //     int index = _unfinished_size - count;
        //     int orb   = String(entry.name()).toInt(); 
            
        //     if ( _unfinished_arr[index] != 0 ) {
        //         int j;
        //         Serial.println("Moving around array elements...");
        //         for ( j = 0; j < (count-1); ++j ) {
        //             _unfinished_arr[j] = _unfinished_arr[j+1];
        //             Serial.println(_unfinished_arr[j]);
        //         }
        //     }
            
        //     _unfinished_arr[index] = orb;
        //     Serial.println(_unfinished_arr[index]);
        //     Serial.println("Done.");
        //     if ( count < _unfinished_size )
        //         ++count;
        // }
}



// NOT FINISHED IMPLEMENTING! DOES NOT WORK AS EXECPTED
// Find the last files that did not finish sending their data to the Mule
// void AndesiteFile::setUnfinishedFiles() {
//     File root          = SD.open("/");
//     unsigned int i     = 0;
//     unsigned int count = 0;
//     int sub            = (int)_num_files - _unfinished_size;
    
//     if ( _num_files <= _unfinished_size ) 
//         sub = _num_files; 
    
//     while ( i < _num_files ) {
//         File entry = root.openNextFile();
        
//         if ( !entry ) { continue; }
        
//         if ( (_Orbit.readHeader(_file, 2) == 0) && (entry.size() >= _header_size) ) {
//             Serial.print(entry.name());
//             Serial.print(" | Count = ");
//             Serial.println(count);
//             // Hopefully extension (.txt) does not mess up conversion
//             // Count can go over unfinished size, fix this
//             int index = _unfinished_size - count;
//             int orb   = String(entry.name()).toInt(); 
            
//             if ( _unfinished_arr[index] != 0 ) {
//                 int j;
//                 Serial.println("Moving around array elements...");
//                 for ( j = 0; j < (count-1); ++j ) {
//                     _unfinished_arr[j] = _unfinished_arr[j+1];
//                     Serial.println(_unfinished_arr[j]);
//                 }
//             }
            
//             _unfinished_arr[index] = orb;
//             Serial.println(_unfinished_arr[index]);
//             Serial.println("Done.");
//             if ( count < _unfinished_size )
//                 ++count;
//         }
        
//         ++i;
//     }
    
//     root.close();
// }
