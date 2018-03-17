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
    // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
    delay(500);
    if ( !RF22.init() ) {
        Serial.println("::RF22failed");
        return 1;
    } else 
		Serial.println("::RF22success");
    
    // Set transmission rate to 38.4 kbs   
    if ( !RF22.setModemConfig(RF22::FSK_Rb38_4Fd19_6) ) {
        Serial.println("::setModemConfigFailed");
		return 2;
	}
	
	// Set transmission power
	RF22.setTxPower(RF22_TXPOW_20DBM);
    RF22.setTimeout(50);
	return 0;
}



// ////////////////////////////////
// ///// RADIO COMMUNICATIONS /////
// ////////////////////////////////

// Send message to the given radio address
int AndesiteRadio::send(uint8_t *message, uint8_t len, uint8_t addr) {
    digitalWrite(RF_CS_PIN, LOW);
    digitalWrite(ADS_CS_PIN, HIGH);
    digitalWrite(SD_CS_PIN, HIGH);
    String str_mes((const char*)message);
    Serial.print("sending string: ");
    Serial.print(str_mes);
	Serial.print(" | ");
    Serial.println(len);
    // Send message
    if ( RF22.sendtoWait(message, len, addr) != RF22_ROUTER_ERROR_NONE ){
        digitalWrite(RF_CS_PIN, HIGH);
		return 0;
	}
    else{
		digitalWrite(RF_CS_PIN, HIGH);
        return 1;
	}
}



// Send message to the given radio address
int AndesiteRadio::sendCommand(uint8_t message[], uint8_t len, uint8_t addr) {
    digitalWrite(RF_CS_PIN, LOW);
    digitalWrite(ADS_CS_PIN, HIGH);
    digitalWrite(SD_CS_PIN, HIGH);

    if ( RF22.sendtoWait(message, len, addr) != RF22_ROUTER_ERROR_NONE ){
        digitalWrite(RF_CS_PIN, HIGH);
		return 0;
	}
    else{
		digitalWrite(RF_CS_PIN, HIGH);
        return 1;
	}
		
}



// Acknowledgment message from the send address
int AndesiteRadio::listen() {
	uint16_t timeout = 10;
    digitalWrite(RF_CS_PIN, LOW);
    digitalWrite(ADS_CS_PIN, HIGH);
    digitalWrite(SD_CS_PIN, HIGH);
    // Acknowledgment message attributes
    uint8_t buf[RF22_MESH_MAX_MESSAGE_LEN] = {0};
    //_message = {0};  
    uint8_t len =RF22_MESH_MAX_MESSAGE_LEN;
    uint8_t from =0;
    
    // Clear data in buffer
    int i;
    for ( i = 0; i < RF22_MESH_MAX_MESSAGE_LEN; i++ )
        buf[i] = 0;

    // Wait for a message addressed to the server from the client
    if ( !RF22.recvfromAckTimeout(buf, &len, timeout, &from) ){ 
        //Serial.println("listen failed");
		digitalWrite(RF_CS_PIN, HIGH);
        return from;
	}
    //Serial.println("LISTEN FUNCTION:: successful recvfromAck");
    _message = (const char*)buf;
    _message_len = len;

    // EOF key received
    if ( _message[0] == _cmd_done ) {
		Serial.println("::EOF reached.");
		digitalWrite(RF_CS_PIN, HIGH);
        return -_cmd_done;
	}
	
    for (i= 0;i<_message_len-1;i++){
        Serial.print(_message[i]);
    }
    Serial.println(_message[_message_len-1]);
	digitalWrite(RF_CS_PIN, HIGH);
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
