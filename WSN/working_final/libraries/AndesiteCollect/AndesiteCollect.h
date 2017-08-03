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
    static void mag(unsigned long timer_diff);
    static void gyro();
    static void gps();

    static void temp();
	  static void light();
};

#endif
