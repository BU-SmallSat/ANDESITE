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
	static String mag();
    static String gyro();
    static String gps();
    static String temp();
};

#endif
