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

#include "AndesiteData.h"
#include "AndesiteFile.h"
#include "ADS1248.h"
#include <SPI.h>
#include <DallasTemperature.h>

// =======
// Classes
// =======

class AndesiteCollect {
public:
    static String mag(unsigned long timer_diff);
    static String gyro();
    static String gps();
    static String temp();
	DeviceAddress TEMP_ONE = "285DD74809000007";
	DeviceAddress TEMP_TWO = "2817EA48090000D5";
	DeviceAddress TEMP_THREE = "2817EA48090000D5";
	DeviceAddress TEMP_FOUR = "2817EA48090000D5";
};

extern DallasTemperature sensors;
extern DeviceAddress Temp1,Temp2, Temp3, Temp4;

#endif
