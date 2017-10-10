// Header guard
#ifndef ANDESITERADIO_H
#define ANDESITERADIO_H

// ========
// Includes
// ========

#include "libandesite.h"

#include <RF22Router.h>
#include <RF22Mesh.h>
#include <RF22.h>



// =======
// Classes
// =======

class AndesiteRadio {
  public:
    AndesiteRadio();
    int init();
    
    int send(uint8_t *message, uint8_t len, uint8_t addr);
    int sendCommand(uint8_t message[], uint8_t len, uint8_t addr);
    int listen();
    
    uint8_t getCommand(uint8_t num);
    
    uint8_t  _cmd_start;
    uint8_t  _cmd_done;

    const char * _message;
    uint8_t _message_len;
    
 protected:
    
    uint8_t  _listen_retries;
    uint8_t  _cmd_retries;
    uint8_t  _send_retries;
    
    uint16_t _listen_timeout;
    uint16_t _cmd_timeout;
    uint16_t _send_timeout;
};



// =======
// Globals
// =======

extern RF22ReliableDatagram RF22;
extern AndesiteRadio _Radio;

#endif
