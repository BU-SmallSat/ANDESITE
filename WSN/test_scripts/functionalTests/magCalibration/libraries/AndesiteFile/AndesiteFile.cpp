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
#include "RF22Mesh.h" 
//#include <bitset>
//#include <string>



// ///////////////////////////////
// ///// ANDESITE FILE CLASS /////
// ///////////////////////////////

// Initialize file class
AndesiteFile::AndesiteFile() {
    _handle;
    _file                   = "001.txt";
    _collected_data         = "";
    _writing_data           = "";
    //_transmit_message       = "";
    _str_parity             = "";
    _int_parity             = 0;
    _file_open_bool         = 0;
    _ready_write_bool       = 0;
    _num_files              = 0;
    _num_unfinished_files   = 0;
    _last_orbit             = 0;
    _start_orbit            = 1;
    _failure                = 0;
    _data_limit             = 500;
    _header_size            = 20;
    _unfinished_files_size  = 5; // If you change this, change array size in header too
    
    // C++ has issues with arrays in constructor, this should make things better
    _unfinished_files_arr[0]= 1;
    _unfinished_files_arr[1]= 0;
    _unfinished_files_arr[2]= 0;
    _unfinished_files_arr[3]= 0;
    _unfinished_files_arr[4]= 0;
}



// Initialize the orbit file, starting the data file on the correct orbit, regardless
//   of system poweroff
// Tested on 5/14/15: Works fine when on. If you turn off and turn back on, it fails 
//                    on "Setting starting orbit value"
boolean AndesiteFile::init() {
    // _num_files = getNumberOfFiles();
    setUnfinishedFiles();
    
    // Put last orbit in first element
    _start_orbit = 1;
    Serial.print("first element in _unfinised_files_arr");
    Serial.println(_unfinished_files_arr[0]);
    Serial.print("Number of Files: ");
    Serial.println(_num_files);
    Serial.print("Start Orbit: ");
    Serial.println(_start_orbit);
    Serial.print("Last Orbit: ");
    Serial.println(_last_orbit);
    Serial.println("IF ( _start_orbit <= last_orbit )");
    Serial.println("\tSetting Last_Orbit + 1");
    Serial.println("ELSE");
    Serial.println("\tSetting _start_orbit");
    
    if ( _start_orbit <= _last_orbit )
        _Orbit.setOrbit( _last_orbit + 1 );
    else
        _Orbit.setOrbit( _start_orbit );
    
    AndesiteFile::set();
    
    // Open up orbit info file
    _handle = SD.open(_file.c_str(), O_CREAT | O_WRITE);
    
	Serial.print("Setting starting orbit value...");
	if ( !_handle ) { 
		Serial.println("Fail.");
        ++_failure;
        _handle.close();
		return false;
	}
	Serial.println("Done.");
	
	// Set the initial orbit number 
    _Orbit.writeHeader(_handle);
	_handle.close(); 
    
    return true;  
}



// //////////////////////////////////
// ///// SEND DATA FILE TO MULE /////
// //////////////////////////////////

// Read a file on the SD card and send it to the Mule
boolean AndesiteFile::send() {
    _file_open_bool = 0;
    _message_size = 0;

    // Open SD card file for reading
    ifstream sdin(_file.c_str());
    _file_position = _Orbit.getPosition();
    //Serial.print(":: File size = ");
    //Serial.println(_handle.size());
    Serial.println(":: Reading and sending file...");
    Serial.print(":: Seeking to position '");
    Serial.println(_file_position);

    sdin.seekg(_file_position);

        // Loop through every char in the file (check if reached end of file, 
    //     update some global variable to true/false, print data on orbit 
    //     to file regarding where file reading ended)

    while (sdin.getline(_transmit_message, RF22_MESH_MAX_MESSAGE_LEN) || sdin.gcount()) {
        _file_open_bool = 1;
        _message_size = sdin.gcount();
        if (sdin.fail()) {
            sdin.clear(sdin.rdstate() & ~ios_base::failbit);
            //Serial.println("extracted partial message!");
        } 
        //Serial.print(" (");
        //Serial.print(_message_size);
        //Serial.print(" chars): ");
        //Serial.println(_transmit_message);
        
        //IMPLEMENT PARITY CHECK OF DATA RETRIEVED FROM FILE
        _int_parity = 0;
    	_str_parity = "";
    	String strMessage = (String)_transmit_message;
    	//Serial.println((String)_transmit_message);
    	int letter; 

      	//convert each ascii letter into 8-bit binary version
      	for (int ii = 0; ii < strMessage.length(); ++ii) 
      	{
      	  	letter = (int)strMessage[ii];
      	  	while(letter > 1){
      	    	if(letter%2 != 0){
      	    		_int_parity++;
      	    	}
      	    	letter = letter/2;
      	  	}
  	    }
  	    
  	  	//pad parity with zeros so that the size of data written is const. (Max parity value ~= 400)
   		if (_int_parity < 10)
			_str_parity = "00" + (String)_int_parity;
	   	else if (_int_parity < 100) 
  			_str_parity = "0" + (String)_int_parity;
    	else 
  			_str_parity = (String)_int_parity;

  		_int_parity = 0;
  		Serial.println(strMessage.substring(strMessage.length() - 3));

   		if (_str_parity == strMessage.substring(strMessage.length() - 3)) {
			Serial.println("Error, parity bit does not match");
    	   // strMessage = strMessage.substring(0,strMessage.length()-3);
    	    //_transmit_message[RF22_MESH_MAX_MESSAGE_LEN-1] = 0;
    	}
    	else {
			Serial.println(strMessage.substring(strMessage.length() - 3));
			//Serial.println("-- Message is clean");
        	//strMessage = strMessage.substring(0,strMessage.length()-3);
        	//_transmit_message[RF22_MESH_MAX_MESSAGE_LEN-1] = 1;
    	}
    	Serial.println(strMessage);
    
//TRASNMIT MESSAGE WITHOUT PARITY

        // Send file line to server 
        Serial.print(F("Sending data -- "));
        Serial.print(_transmit_message);
        Serial.print(" ");
        Serial.print(_message_size+1);
        Serial.print(" | ");
        Serial.println(_Orbit.getLatitude());
        _Radio.send((uint8_t*)_transmit_message, _message_size, ACDH_MULE_ADDR);

        //update file position
        _file_position = sdin.tellg();
        _Orbit.setPosition(_file_position);
        _Orbit.writeHeader(_handle);
        sdin.seekg(_file_position);
            
        // Update orbit values
        _Orbit.setLatitude();
        if ( _Orbit.getLatitude() >= _Orbit.getModeSwitchLatitude() ) {
            Serial.println(":: NOT done sending file.");
            _handle.close();
            AndesiteFile::done();
            return false;
        }
    } 

    // Make sure SD card file was opened correctly
    if ( !_file_open_bool ) {
        Serial.print("ERROR: File '");
        Serial.print(_file);
        Serial.println("' was not found");
        ++_failure;
        
        // Done sending file to Mule (b/c couldn't be found)
        _handle.close();
        AndesiteFile::done();
        
        return false;
    }
   
    // Done sending file to Mule
    Serial.println(":: Done sending file.");
    _handle.close();
    AndesiteFile::done();
    
    return true;
}



// Tell Mule that file sending is done
void AndesiteFile::done() {
    _Radio.sendCommand(&_Radio._cmd_done, sizeof(uint8_t), ACDH_MULE_ADDR);
    // Write header in here?
    // _Orbit.writeHeader(_handle)?
}


 void AndesiteFile::store(String str) { 
    _collected_data += str; 
    _ready_write_bool = 0;
    while(_collected_data.length() > RF22_MESH_MAX_MESSAGE_LEN){
        _writing_data += _collected_data.substring(0,RF22_MESH_MAX_MESSAGE_LEN);
        _collected_data = _collected_data.substring(RF22_MESH_MAX_MESSAGE_LEN);
        _ready_write_bool++;
    }          
    return;
}


// //////////////////////////////
// ///// WRITE TO DATA FILE /////
// //////////////////////////////


// Write the stored data to the file
void AndesiteFile::write(bool done) {

    if(done){
        while(_collected_data.length() > RF22_MESH_MAX_MESSAGE_LEN-4){
            _writing_data += _collected_data.substring(0,RF22_MESH_MAX_MESSAGE_LEN-4);
            _collected_data = _collected_data.substring(RF22_MESH_MAX_MESSAGE_LEN-4);
            _ready_write_bool++;
        }   
    }
    
    _handle = SD.open(_file.c_str(), O_CREAT | O_APPEND | O_WRITE);
    
    if ( !_handle ) {
        ++_failure;
        Serial.println(":: File wr failed.");
        _handle.close();
        return;
    }
    
    int letter;
    
    while(_ready_write_bool > 0){
        //even parity bit (count the 1's)
        _int_parity = 0;
        _str_parity = ""; 

        //Serial.print("writing: ");
        //Serial.println(_writing_data);

	   //convert each ascii letter into 8-bit binary version
	   for (int ii = 0; ii < _writing_data.length(); ++ii) 
	   {
		    letter = (int)_writing_data[ii];
        	while(letter > 0){
       			if(letter%2 != 0){
            		_int_parity++;
          		}
          		letter = letter/2;
        	}
	   }

	   //pad parity with zeros so that the size of data written is const. (Max parity value ~= 400)
	   if (_int_parity < 10)
		  _str_parity = "00" + (String)_int_parity;
	   else if (_int_parity < 100) 
		  _str_parity = "0" + (String)_int_parity;
	   else 
		  _str_parity = (String)_int_parity;

	   //write to SD card
        _handle.println((_file.substring(0,3)+":"+_writing_data.substring(0,RF22_MESH_MAX_MESSAGE_LEN-4))); //didnt include parity
        _writing_data = _writing_data.substring(RF22_MESH_MAX_MESSAGE_LEN-4);
        _ready_write_bool--;
    }

    _handle.close();

    //reset values
    _writing_data = "";
    _ready_write_bool = 0;
}



// Check storage status to see if it has reached its max length
boolean AndesiteFile::status() {
    if ( _writing_data.length() >= RF22_MESH_MAX_MESSAGE_LEN ) 
        return true;
    else
        return false;
}

/*

void AndesiteFile::writeMagTest(String str){
    //digitalWrite(13, HIGH);
    //delay(20);
    File dataFile = SD.open("magData.txt", FILE_WRITE);
    if(SD.exists("magData.txt")){
        //digitalWrite(13, HIGH);
        //delay(20);
        dataFile.println(str);
        dataFile.close();
        //Serial.println(data);
    }
}

*/
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
    _handle = SD.open(_file.c_str(), O_READ);
    
    if ( !_handle ) {
        ++_failure;
        _handle.close();
        return -1;
    }
    
    unsigned long size = _handle.size();
    _handle.close();
    
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
    _num_unfinished_files  = 0;
    while( SD.exists( strOrbitFile(orb).c_str() ) ) {
        if(_Orbit.readHeader( strOrbitFile(orb), 2) == 0) {
            Serial.print("GetNumberofUnfinishedFiles: ");
            Serial.println(strOrbitFile(orb));
            ++_num_unfinished_files;
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
            index = _num_unfinished_files - count;
            if(index < _unfinished_files_size) {
                i = _unfinished_files_arr[index];
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
