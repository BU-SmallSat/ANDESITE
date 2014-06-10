#include "Arduino.h"
#include "Position.h"
#include <string.h>

void Position::sortInfo(string GPS)
{
	
	string delimiter = ",";
	size_t pos = 0;
	string coord[6];
	int counter = 0;
	GPS.erase(0, 7); //erase $GPGLL,
	while ((pos = GPS.find(delimiter)) != string::npos) //erase the commas and store the coordinate info
	{
		coord[counter] = GPS.substr(0, pos);
		GPS.erase(0, pos + delimiter.length());
		counter++;
	}
	string checksum = GPS.substr(0);
	string latitude = coord[0];
	string NS = coord[1];
	string longitude = coord[2];
	string EW = coord[3];
	string time = coord[4];
	string active = coord[5];
}

