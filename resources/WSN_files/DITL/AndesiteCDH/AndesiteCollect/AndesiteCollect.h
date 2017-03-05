// -*-c++-*-
// **********************************************************************************
// 
// Name:    AndesiteCollect.h
// Class:   <AndesiteCollect>
// Author:  Gabriel Gonzalez
// 
// Description: The skeleton for the ANDESITE Satellite data collection method. 
//              
// Notes: None.
// 
// **********************************************************************************


// ============
// Header guard
// ============

#ifndef ANDESITECOLLECT_H
#define ANDESITECOLLECT_H

#include <SPI.h>

// =======
// Classes
// =======

class AndesiteCollect {
public:
    static void mag(SPISettings ADC_Settings);
    static void gyro();
    static void gps();
    static void temp();
};

#endif
