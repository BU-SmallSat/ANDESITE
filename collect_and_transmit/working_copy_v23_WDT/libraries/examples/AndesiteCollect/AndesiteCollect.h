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


// =======
// Classes
// =======

class AndesiteCollect {
public:
    static void mag();
    static void gyro();
    static void gps();
    static void magTest();
};

#endif
