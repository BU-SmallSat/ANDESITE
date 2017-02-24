// -*-c++-*-
// **********************************************************************************
// 
// Name:    AndesiteOrbit.h
// Class:   <AndesiteOrbit>
// Author:  Gabriel Gonzalez
// 
// Description: The skeleton for the ANDESITE Satellite orbit information recorder. 
//              
// Notes: Make a function to return boolean when above 65 deg and below 65 deg.
// 
// **********************************************************************************


// Header guard
#ifndef ANDESITEORBIT_H
#define ANDESITEORBIT_H

// ========
// Includes
// ========

#include <SdFat.h>



// =======
// Classes
// =======

// I think everything can be static but i'm not sure
class AndesiteOrbit {
public:
    AndesiteOrbit();
    
    // Get latitude orbit information
    double           getLatitude()           { return _latitude; }
    int              getModeSwitchLatitude() { return _lat_mode_switch; }
    unsigned int     getOrbit()              { return _orbit; }
    unsigned int     getPosition()           { return _position; }
    void             setOrbit(int val)       { _orbit    = val; }
    void             setPosition(int val)    { _position = val; }
    void             setStatus(int val)      { _status   = val; }
    
    // Set the orbit latitude
    boolean          setLatitude();
    
    // Perform reads/writes to the header of the data file
    unsigned long    readHeader(String file, int field); 
    boolean          writeHeader(File handle); 
    boolean          update(String file); // Updates orbit/pos/stat
    
    // Wait for different modes in the orbit to finish
    boolean          waitOrbitFinish();
    boolean          waitScienceFinish();
    boolean          waitTransferStartCmd();
    
    // Utility to print latitude for testing purposes
    unsigned long    latIntervalPrint(unsigned long now, int interval); 
    
private:
    
    // Zero-pad the information in the header of the data file
    String           zeroPadFields(); 
    
    // Orbit latitude
    double           _latitude;
    
    // Latitude at which modes switch from Science to Transfer (and vice versa)
    int              _lat_mode_switch;
    
    // Data file header information
    unsigned int     _orbit;
    unsigned long    _position;
    unsigned int     _status;
    
    // Determine latitude increments in case of GPS fail
    double           _dl;      // [deg] (dv*dt/radius * 180/pi)
    unsigned long    _dt;      // [s] Time elapsed since last latitude update
    double           _dv;      // [km/s] Velocity of WSN
    double           _radius;  // [km/s] Radius of orbit (including radius of Earth)
    
    // Stuff
    unsigned int    _timeout;
    unsigned int    _update_fail;
};



// =======
// Globals
// =======

extern AndesiteOrbit _Orbit;

#endif
