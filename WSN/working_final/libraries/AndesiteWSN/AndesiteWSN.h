// -*-c++-*-
// **********************************************************************************
// 
// Name:    AndesiteOrbit.h
// Class:   <AndesiteOrbit>
// Author:  Gabriel Gonzalez
// 
// Description: The skeleton for the Wireless Sensore Nodes (WSN) that are part of 
//              the ANDESITE Satellite.
//              
// Notes: None.
// 
// **********************************************************************************


// Header guard
#ifndef ANDESITEWSN_H
#define ANDESITEWSN_H

// Includes
#include "Arduino.h"
#include "AndesiteData.h"
#include "AndesiteCollect.h"
#include "AndesiteRadio.h"
#include "AndesiteOrbit.h"
#include "AndesiteFile.h"
#include "libandesite.h"
#include "ADS1248.h"
#include <SPI.h>
#include <DallasTemperature.h>



// =======
// Classes
// =======

class AndesiteWSN {
public:
    AndesiteWSN();
    
    // Initialize the components on WSN
    int              init();
    
    // Check if it is ok to go into Science mode (collect data)
    boolean          isScienceMode();
    
    // Execute Science mode, collect magnetometer data at 30 Hz and GPS and Gyroscope
    //   data at 10 Hz
    int              scienceMode(bool isCalibration);
    
    // Check if it is ok to go into Transfer mode (send data)
    boolean          isTransferMode();
    
    // Execute Transfer mode, send the file of data that was collected in Science mode
    //   to the Mule
    int              transferMode();

	// Listen for a message from the mule to start transfer mode or execute other cmd.
	bool			 listenMuleMessage();

	// Send health beacon from WSNx to Mule
	void			 healthBeacon();

    // Wait for WSN orbit to finish
    void             wait();

	// Low Power Mode
	void			 lowPowerMode();

    int              _science_mode_state;
	int				_temp_timing;
	
	unsigned long	_transfer_start;
    
private:
	
    // Duration of Science mode
    unsigned long    _science; 
    
    // Duration of Transfer mode
    unsigned long    _transfer; 
    
    // Period of time in which to collect magnetometer, GPS, and gyroscope data
    unsigned long    _magfreq; 
    unsigned long    _ggfreq; 
    
    // Verify that the orbit has started
    boolean          _orb_start;

    //state of the WSN - either "s" for science mode or "t" for data transfer mode
    char             state;

    //percentage of battery voltage remaining
    int              batteryLevel;
};

extern DallasTemperature sensors;
extern DeviceAddress Temp1, Temp2, Temp3, Temp4;
#endif
