// ========
// Includes
// ========

#include "AndesiteMule.h"



// ///////////////////////////////
// ///// ANDESITE FILE CLASS /////
// ///////////////////////////////

// Initialize file class
AndesiteMule::AndesiteMule() {
    _num_avail = 0;
}



// ////////////////////////////
// ///// MULE BOARD SETUP /////
// ////////////////////////////

// Setup Mule
int AndesiteMule::radioInit() {
        
    // Turn on radios
    //pinMode(RF_CS_PIN, OUTPUT);
    pinMode(4, OUTPUT);    
    digitalWrite(4, LOW);
    
    
    // Initialize radio and set data rate
    if ( _Radio.init() != 0 ) {
        //Serial.println("ERROR: Radio setup failed.");
        return 1;
    }
    
    // Initialize WSN address arrays
    _all_wsn[0] = ACDH_WSN1_ADDR;
    _all_wsn[1] = ACDH_WSN2_ADDR;
    _all_wsn[2] = ACDH_WSN3_ADDR;
    _all_wsn[3] = ACDH_WSN4_ADDR;
    _all_wsn[4] = ACDH_WSN5_ADDR;
    _all_wsn[5] = ACDH_WSN6_ADDR;
    _all_wsn[6] = ACDH_WSN7_ADDR;
    _all_wsn[7] = ACDH_WSN8_ADDR;
    
    return 0;
}



// ////////////////////////////////////
// ///// MULE-TO-WSN INTERACTIONS /////
// ////////////////////////////////////

// Verify that the new address that was received is not already in the available address array
bool AndesiteMule::isAvailable(int addr) {
	int i;
    if (addr == 0){
        return false;
    }
	for ( i = 0; i < _num_avail; ++i)
		if ( _avail_wsn[i] == addr )
			return false;
	
	return true;
}



// Determine which WSN are available 
int AndesiteMule::listen() {
    
    // Wait for WSN to finish Science Mode
    int i, addr;
    unsigned long timer_start = 0,
		avail_timeout = 200;  
	
	// Clear available WSNs from memory
	for ( i = 0; i < _num_avail; ++i )
		_avail_wsn[i] = 0;
    
    _num_avail = 0;
	
    // Will still hang if no WSN ever communicates
	Serial.println(":: Waiting for WSNs to (re)enter Transfer Mode...");
	
    while ( _num_avail != ACDH_WSN_NUM ) {
        //Don't process messages in listen mode so that we are not interfering with messages
        if(Serial.available() > 0){
            message = "";
            message = Serial.readString();
            Serial.println(message);
            processMessage(message);
        }
        
        // Address of WSN (if it communicated)
        addr = _Radio.listen();
		
        if ( !timer_start && (addr <= 0) )	
			continue;
		if ( addr > 0 ) {
			Serial.print("::Detected WSN ");
			Serial.print(addr-1);
			Serial.println(".");
		}
		
        // Add address to available WSN array
		if ( AndesiteMule::isAvailable(addr) ) {
			_avail_wsn[_num_avail] = addr; // Add address and then increment
            if ( !_num_avail )
				timer_start = millis();
            
            ++_num_avail;
        }
        
		// Stop waiting for WSNs after timeout is reached
		if ( timer_start ) {
			if ( (millis() - timer_start) >= MULE_LISTEN_TIMEOUT) { 
				Serial.println(":: Timeout reached for waiting for WSNs to (re)enter Transfer Mode.");
				return 0;
			}
		}
    }
	
    // Make sure there are available nodes
    if ( _num_avail > 0 ) {
        return 0;
    } 
    
    return 1;
}


// Server sends a request to collect gathered data
int AndesiteMule::request() {
    
    // Request science file from each available WSN
	Serial.println(":: Requesting data from each WSN...");
	
	unsigned long timer_start,
		big_ass_num = 10000;
    int i, ret;
	
    for ( i = 0; i < _num_avail; ++i ) {
        
        // Send command from Mule to WSN to start executing
		Serial.print(":: Sending data request command..."); 
        uint8_t data[]="Transfer";
        if ( !_Radio.sendCommand(data, sizeof(data), _avail_wsn[i]) ) {
            ret = 0;
            continue;
        }
		
        // Receive data file from WSN
		Serial.println(":: Waiting for data from each WSN to be sent...");
		timer_start = millis();
		
        while ( (ret=_Radio.listen()) != -_Radio._cmd_done ) {
			//While loop needs a break out option - to ensure that it never gets stuck
      		// We want to only read full messages, so we want to read until a new line character
      		// Should the messages be a uniform length? Gets dangerous if messages get out of sync
      		// Should we keep waiting on a read until the full message is in? it shouldn't hold too long,
      		// But is it dangerous? 
            if(Serial.available() > 0){
                message = "";
                message = Serial.readString();
                processMessage(message);
            }
			// Another WSN sent a message, add it to the list of available WSN
			if ( ret && AndesiteMule::isAvailable(ret) ) {
				_avail_wsn[_num_avail] = ret;
				++_num_avail;
			}
		}
		
		Serial.print("::Done receiving data from WSN ");
        Serial.print(_avail_wsn[i]-1);
		Serial.println(".");
	}
	
	Serial.println(":: Done requesting data from WSNs.");
	
	return 1;
}

int AndesiteMule::relayMessage(int nodeNumber, String RFmessage){
    uint8_t* RFmessageArray = (uint8_t*)(RFmessage.c_str());
    if ( !_Radio.send(RFmessageArray, sizeof(RFmessageArray), nodeNumber) ) {
    }
}

int AndesiteMule::processMessage(String groundMessage){
	if(groundMessage == "updateNetwork"){
		listen();
        Serial.print("updatedNetwork:");
        for (int i = 0; i < 8;i++){
          Serial.print(_avail_wsn[i]);
        }
        Serial.print("\n");
	}
    else if(groundMessage.startsWith("newNetwork:",0)){
        for (int i = 0; i < 8;i++){
            _avail_wsn[i] = groundMessage[i+14] - 48;
        }
    }
    else if(groundMessage.startsWith("sensorNode",0)){
        Serial.println("sendingCommandToSensorNodes");
        int messageNode = groundMessage[12]-48;
        relayMessage(messageNode,groundMessage.substring(13));
    }
	else if(groundMessage == "radioInit"){
		Serial.print("radioInit");
	}
}
