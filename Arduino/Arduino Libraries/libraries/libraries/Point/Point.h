#ifndef POINT_H
#define POINT_H

#include "Arduino.h"

class Point
{
public:
	void Pointing(XbeeInfo & Router, int pointType);

private:
	#define AUTO_POINT 1 //Point solar panels towards the sun
	#define MANUAL_POINT 2 //Manual for WSN ejection & misc pointing needs
};

#endif