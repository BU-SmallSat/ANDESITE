// Header guard
#ifndef ANDESITECOLLECT_H
#define ANDESITECOLLECT_H

// ========
// Includes
// ========

#include "AndesiteData.h"
#include "AndesiteFile.h"



// =======
// Classes
// =======



class AndesiteCollect {
 public:
    static void mag();
    static void gyro();
    static void gps();
};

#endif
