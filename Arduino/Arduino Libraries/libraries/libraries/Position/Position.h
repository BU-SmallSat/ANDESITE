#ifndef POSITION_H
#define POSITION_H

#include "Arduino.h"
#include <String.h>

class Position //using GLL type of NMEA strings
{
public:
	//void sortInfo(string GPS);

private:
	double latitude; //exact latitude
	double longitude; //exact longitude
	char NS; //Specifies NORTH or SOUTH
	char EW; //Specifies EAST or WEST
	long time; //time UTC
	char active; //check if data active or void
	//string checksum; 
};

#endif
