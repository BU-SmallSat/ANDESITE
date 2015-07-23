// ========
// Includes
// ========

#include "AndesiteRadio.h"



// ///////////////////////
// ///// RADIO SETUP /////
// ///////////////////////

// Initialize the wireless sensor node class
AndesiteRadio::AndesiteRadio() {
    _cmd_start      = 0x2;
    _cmd_done       = 0x1;
    
    _listen_retries = 10 * sqrt(60.0 / 5.0);
    _cmd_retries    = 10 * sqrt(60.0 / 15.0);
    _send_retries   = 10 * sqrt(60.0 / 45.0);
    
    _listen_timeout = 10 * _listen_retries;
    _cmd_timeout    = 10 * _cmd_retries;
    _send_timeout   = 10 * _send_retries;
}


// Setup RF22 radio
int AndesiteRadio::init() {
    
    // Initialize RF22
    //     Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
    // Try reboot here?
    delay(500);
    if ( !RF22.init() ) {
        Serial.println("ERROR: RF22 init failed.");
        return 0;
    } else 
		Serial.println(F("RF22 succeeded!"));
    
    // Set transmission rate to 38.4 kbs   
    if ( !RF22.setModemConfig(RF22::FSK_Rb38_4Fd19_6) ) {
        Serial.println("ERROR: setModemConfig failed.");
		return 0;
	}
	
	// Set transmission power
	RF22.setTxPower(RF22_TXPOW_20DBM);
    
    if(!dummyDataTest()){
        return 0;
    }
	
    return 1;
}

int AndesiteRadio::dummyDataTest(){
    return 1;
}


// ////////////////////////////////
// ///// RADIO COMMUNICATIONS /////
// ////////////////////////////////

// Send message to the given radio address
int AndesiteRadio::send(uint8_t *message, uint8_t len, uint8_t addr) {
    
    // Set radio timeout and retries for sending a simple message
    RF22.setTimeout(_send_timeout);
    RF22.setRetries(_send_retries);
    
    // Send message
    if ( RF22.sendtoWait(message, len, addr) != RF22_ROUTER_ERROR_NONE )
        return 0;
    else
        return 1;
}



// Send message to the given radio address
int AndesiteRadio::sendCommand(uint8_t *message, uint8_t len, uint8_t addr) {
    
    // Set radio timeout and retries for sending a simple message
    RF22.setTimeout(_cmd_timeout);
    RF22.setRetries(_cmd_retries);
    
    // Send message 
    if ( RF22.sendtoWait(message, len, addr) != RF22_ROUTER_ERROR_NONE )
        return 0;
    else
        return 1;
}



// Acknowledgement message from the send address
int AndesiteRadio::listen() {
    
    // Acknowledgement message attributes
    uint8_t buf[RF22_MESH_MAX_MESSAGE_LEN] = {0};   
    uint8_t len = sizeof(buf);
    uint8_t from = 0;
    
    // Clear data in buffer
    int i;
    for ( i = 0; i < RF22_MESH_MAX_MESSAGE_LEN; i++ )
        buf[i] = 0;
    
    // Wait for a message addressed to the server from the client
    if ( !RF22.recvfromAck(buf, &len, &from) ) 
        return from;
    
	// Start key received
	if ( buf[0] == _cmd_start ) {
		Serial.print("Start key received. ");
		return from;
	}
	
    // EOF key received
    if ( buf[0] == _cmd_done ) {
		Serial.println("EOF reached.");
        return -_cmd_done;
	}
	
    // Message received 
    Serial.print("Receiving data from WSN ");
    Serial.print(from-1);
    Serial.print(" -- ");
	
	const int n = acdh_strlen(buf);
	const int sz = 2 * n;
	char line[sz];
	
	// Decompress data
	acdh_str_decompress(line, buf, n);
    
	// Print data char by char
	int q = 0;
	while ( (line[q] != 0) && (q < sz) ) {
		Serial.print(line[q]);
		++q;
	}
	Serial.print("\n");
	
    return from;
}



// ////////////////////
// ///// COMMANDS /////
// ////////////////////

// Return command
uint8_t AndesiteRadio::getCommand(uint8_t num) {
    if ( num == 0x2 )
        return _cmd_start;
    if ( num == 0x1 )
        return _cmd_done;
    return _cmd_start;
}
