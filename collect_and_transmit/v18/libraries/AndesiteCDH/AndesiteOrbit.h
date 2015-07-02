// Header guard
#ifndef ANDESITEORBIT_H
#define ANDESITEORBIT_H

// ========
// Includes
// ========

#include "AndesiteData.h"
#include "AndesiteFile.h"
#include "libandesite.h"



// =======
// Classes
// =======

class AndesiteOrbit {
 public:
    AndesiteOrbit();
    String        zeroPadFields();
    unsigned long readHeader(String file, int field);
    boolean       writeHeader(File handle);
    
    unsigned int  update(String file);
    
    unsigned long latIntervalPrint(unsigned long now, int interval);
    boolean       setLat();
    
    unsigned int  getFail();
    boolean       wait();
    boolean       waitScience();
    
    double        _latitude;
    double        _lat_limit;
    unsigned int  _orbit;
    unsigned long _position;
    unsigned int  _status;
    
 private:
    unsigned int  _timeout;
    unsigned int  _update_fail;
};



// =======
// Globals
// =======

extern AndesiteOrbit _Orbit;

#endif
