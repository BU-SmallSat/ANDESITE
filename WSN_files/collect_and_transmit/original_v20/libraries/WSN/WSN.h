#ifndef WSN_H
#define WSN_H

#include "Arduino.h"


class WSN
{
public:
	WSN();

	//GPS
	String GPSsim(String GPS);
	void sortInfo(String GPS);
	String getType();
	String getTime();
	String getLatitude();
	String getNS();
	String getLongitude();
	String getEW();
	String getFixQuality();
	String getNumSat();
	String getDilution();
	String getAltitude();
	String getAltitudeUnits();
	String getOffset();
	String getOffsetUnits();
	String getChecksum();

	//Orbit Counter
	int OrbitCounter(int &OrbitCounter, String GPS);

private:
	String type; //always $GPGGA
	String time; //time UTC
	String latitude; //exact latitude
	String longitude; //exact longitude
	String NS; //Specifies NORTH or SOUTH
	String EW; //Specifies EAST or WEST
	String fixquality;
	String numSat;
	String dilution;
	String altitude;
	String altitudeUnits;
	String geoidOffset;
	String geoidOffsetUnits;
	String checksum;

#endif
