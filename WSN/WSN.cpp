#include "Arduino.h"
#include "WSN.h"
#include <String.h>


WSN::WSN()
{
	xCoordVoltage = 0;
	yCoordVoltage = 0;
	zCoordVoltage = 0;
	xGyro = 0;
	yGyro = 0;
	zGyro = 0;
	PowerVoltage = 3.7;
	latitude = NULL;
}

String WSN::GPSsim(String latitude)
{
	Serial.println(latitude);
	int newlat = (latitude.toInt()) + 10;
	if (newlat >= 9000)
	{
		newlat = 0;
	}
	String lat = (String) newlat;
	return lat;
}


int WSN::OrbitCounter(int &OrbitCounter, String GPS)
{
	WSN().sortInfo(GPS);

	if (NS == "N")
	{
		if (latitude.toInt() >= 8900)
		{
			OrbitCounter++;
		}
	}
	return OrbitCounter;
}

void WSN::sortInfo(String GPS)//, int value)
{
	int firstcomma = GPS.indexOf(',');
	int secondcomma = GPS.indexOf(',', firstcomma + 1);
	int thirdcomma = GPS.indexOf(',', secondcomma + 1);
	int fourthcomma = GPS.indexOf(',', thirdcomma + 1);
	int fifthcomma = GPS.indexOf(',', fourthcomma + 1);
	int sixthcomma = GPS.indexOf(',', fifthcomma + 1);
	int seventhcomma = GPS.indexOf(',', sixthcomma + 1);
	int eigthcomma = GPS.indexOf(',', seventhcomma + 1);
	int ninthcomma = GPS.indexOf(',', eigthcomma + 1);
	int tenthcomma = GPS.indexOf(',', ninthcomma + 1);
	int eleventhcomma = GPS.indexOf(',', tenthcomma + 1);
	int twelthcomma = GPS.indexOf(',', eleventhcomma + 1);
	int thirteenthcomma = GPS.indexOf(',', twelthcomma + 1);
	int fourteenthcomma = GPS.indexOf(',', thirteenthcomma + 1);

	type = GPS.substring(0, firstcomma);
	time = GPS.substring(firstcomma + 1, secondcomma);
	latitude = GPS.substring(secondcomma + 1, thirdcomma);
	NS = GPS.substring(thirdcomma + 1, fourthcomma);
	longitude = GPS.substring(fourthcomma + 1, fifthcomma);
	EW = GPS.substring(fifthcomma + 1, sixthcomma);
	fixquality = GPS.substring(sixthcomma + 1, seventhcomma);
	numSat = GPS.substring(seventhcomma + 1, eigthcomma);
	dilution = GPS.substring(eigthcomma + 1, ninthcomma);
	altitude = GPS.substring(ninthcomma + 1, tenthcomma);
	altitudeUnits = GPS.substring(tenthcomma + 1, eleventhcomma);
	geoidOffset = GPS.substring(eleventhcomma + 1, twelthcomma);
	geoidOffsetUnits = GPS.substring(twelthcomma + 1, thirteenthcomma);
	checksum = GPS.substring(thirteenthcomma + 1, fourteenthcomma);
}

String WSN::getType()
{
	return type;
}

String WSN::getTime()
{
	return time;
}

String WSN::getLatitude()
{
	return latitude;
}

String WSN::getNS()
{
	return NS;
}

String WSN::getLongitude()
{
	return longitude;
}

String WSN::getEW()
{
	return EW;
}

String WSN::getFixQuality()
{
	return fixquality;
}

String WSN::getNumSat()
{
	return numSat;
}

String WSN::getDilution()
{
	return dilution;
}

String WSN::getAltitude()
{
	return altitude;
}

String WSN::getAltitudeUnits()
{
	return altitudeUnits;
}

String WSN::getOffset()
{
	return geoidOffset;
}

String WSN::getOffsetUnits()
{
	return geoidOffsetUnits;
}

String WSN::getChecksum()
{
	return checksum;
}

double WSN::get_xCoordVoltage()
{
	return xCoordVoltage;
}

double WSN::get_yCoordVoltage()
{
	return yCoordVoltage;
}

double WSN::get_zCoordVoltage()
{
	return zCoordVoltage;
}

//must store actual magnetometer data using pin information of magnetometer
void WSN::setxyzVoltage(double xCoordVoltage, double yCoordVoltage, double zCoordVoltage)
{
	this->xCoordVoltage = xCoordVoltage;
	this->yCoordVoltage = yCoordVoltage;
	this->zCoordVoltage = zCoordVoltage;
}


double WSN::get_xGyro()
{
	return xGyro;
}

double WSN::get_yGyro()
{
	return yGyro;
}

double WSN::get_zGyro()
{
	return zGyro;
}

void WSN::setxyzGyro(double xGyro, double yGyro, double zGyro)
{
	this->xGyro = xGyro;
	this->yGyro = yGyro;
	this->zGyro = zGyro;
}


double WSN::getPowerVoltage()
{
	//pin information needed so that Voltage can be set to the incoming information
	return PowerVoltage;
}
