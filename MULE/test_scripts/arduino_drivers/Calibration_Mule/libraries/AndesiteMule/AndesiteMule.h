// Header guard
#ifndef ANDESITEMULE_H
#define ANDESITEMULE_H

// ========
// Includes
// ========

#include "MuleCalibrationRadio.h"
#include "libandesite.h"


// =======
// Classes
// =======

class AndesiteMule {
 public:
    AndesiteMule();
    int radioInit();
    bool isAvailable(int addr);
    int  listen();
    int  request();
    int relayMessage(int nodeNumber, String message);
    int processMessage(String groundMessage);
    String message;
    char nextChar;
    
 private:
    uint8_t _all_wsn[ACDH_WSN_NUM];
    uint8_t _avail_wsn[ACDH_WSN_NUM];
    uint8_t _num_avail;
};

#endif
