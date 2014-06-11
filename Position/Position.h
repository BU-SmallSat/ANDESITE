#ifndef POSITION_H
#define POSITION_H

#include "Arduino.h"
#include <String.h>

class Position //using GLL type of NMEA strings
{
public:
	String sortInfo(String GPS, int value);

private:
	String type; //always $GPGLL
	String latitude; //exact latitude
	String longitude; //exact longitude
	String NS; //Specifies NORTH or SOUTH
	String EW; //Specifies EAST or WEST
	String time; //time UTC
	String active; //check if data active or void
	String checksum;

#define TYPE 0
#define LATITUDE 1
#define NORTHSOUTH 2
#define LONGITUDE 3
#define EASTWEST 4
#define TIME 5
#define ACTIVE 6
#define CHECKSUM 7

};

#endif
