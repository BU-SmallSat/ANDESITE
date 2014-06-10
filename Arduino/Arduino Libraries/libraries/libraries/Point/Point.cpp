#include "Arduino.h"
#include "Point.h"
#include "XbeeInfo.h"

void Point::Pointing(XbeeInfo & Router, int pointType)
{
	if (pointType == 1)
	{
		//point specific side of Router towards sun
	}
	
	else if (pointType == 2)
	{
		//Manual point??
	}
	
	else
	{
		//Error check
	}
}