// Header guard
#ifndef ANDESITEMULE_H
#define ANDESITEMULE_H

// ========
// Includes
// ========

#include "AndesiteRadio.h"
#include "libandesite.h"


// =======
// Classes
// =======

class AndesiteMule {
 public:
    AndesiteMule();
    int init();
    
    bool isAvailable(int addr);
    int  listen();
    int  request();
    
 private:
    uint8_t _all_wsn[ACDH_WSN_NUM];
    uint8_t _avail_wsn[ACDH_WSN_NUM];
    uint8_t _num_avail;
};

#endif
