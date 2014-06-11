#ifndef POSITION_H
#define POSITION_H

#include "Arduino.h"
#include <String.h>

class Position //using GLL type of NMEA strings
{
public:
	void sortInfo(String GPS);

private:
	String type; //always $GPGLL
	String latitude; //exact latitude
	String longitude; //exact longitude
	String NS; //Specifies NORTH or SOUTH
	String EW; //Specifies EAST or WEST
	String time; //time UTC
	String active; //check if data active or void
	String checksum; 
};

#endif
