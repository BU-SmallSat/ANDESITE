#include "Arduino.h"
#include "Position.h"
#include <String.h>

//How to call and print in good format

//String GPS = "$GPGLL,4916.45,N,12311.12,W,225444,A,*1D";
//String latitude = Position().sortInfo(GPS, LATITUDE);
//String NS = Position().sortInfo(GPS, NORTHSOUTH);
//String longitude = Position().sortInfo(GPS, LONGITUDE);
//String EW = Position().sortInfo(GPS, EASTWEST);
//String time = Position().sortInfo(GPS, TIME);
//String active = Position().sortInfo(GPS, ACTIVE);
//String checksum = Position().sortInfo(GPS, CHECKSUM);
//Serial.println("Exact Latitude: " + latitude + " " + NS);
//Serial.println("Exact Longitude: " + longitude + " " + EW);
//Serial.println("Date and Time: " + time + " UTC");
//Serial.println("Data Active? " + active);
//Serial.println("Checksum: " + checksum);
//while (true);


String Position::sortInfo(String GPS, int value)
{
	int firstcomma = GPS.indexOf(',');
	String type = GPS.substring(0, firstcomma);

	int secondcomma = GPS.indexOf(',', firstcomma + 1);
	String latitude = GPS.substring(firstcomma + 1, secondcomma);

	int thirdcomma = GPS.indexOf(',', secondcomma + 1);
	String NS = GPS.substring(secondcomma + 1, thirdcomma);

	int fourthcomma = GPS.indexOf(',', thirdcomma + 1);
	String longitude = GPS.substring(thirdcomma + 1, fourthcomma);

	int fifthcomma = GPS.indexOf(',', fourthcomma + 1);
	String EW = GPS.substring(fourthcomma + 1, fifthcomma);

	int sixthcomma = GPS.indexOf(',', fifthcomma + 1);
	String time = GPS.substring(fifthcomma + 1, sixthcomma);

	int seventhcomma = GPS.indexOf(',', sixthcomma + 1);
	String active = GPS.substring(sixthcomma + 1, seventhcomma);

	int eigthcomma = GPS.indexOf(',', seventhcomma + 1);
	String checksum = GPS.substring(seventhcomma + 1, eigthcomma);

	switch (value)
	{
	case TYPE:
		return type;
	case LATITUDE:
		return latitude;
	case NORTHSOUTH:
		return NS;
	case LONGITUDE:
		return longitude;
	case EASTWEST:
		return EW;
	case TIME:
		return time;
	case ACTIVE:
		return active;
	case CHECKSUM:
		return checksum;
	}
}
