#include "Arduino.h"
#include "Position.h"
#include <String.h>

void Position::sortInfo(String GPS)
{
	
	//char delimiter = ",";
	//size_t pos = 0;
	byte coord[50];
	//int counter = 0; 
	//GPS.erase(0, 7); //erase $GPGLL,

	GPS.getBytes(coord, 50);
	for (int i = 0; i <= GPS.length(); i++)
	{
		
		if (coord[i] == ',')
		{
			coord.replace(',', ' ');
		}
		//coord[i] = GPS[i];
		//if ((coord[i] == delimiter)
		//{
		//	coord[counter] = GPS.substr(0, pos);
		//	GPS.erase(0, pos + delimiter.length());
		//	counter++;
		//}
	}
	//String checksum = GPS.substr(0);
	//String latitude = coord[0];
	//String NS = coord[1];
	//String longitude = coord[2];
	//String EW = coord[3];
	//String time = coord[4];
	//String active = coord[5];
}

