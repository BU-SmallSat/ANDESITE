#ifndef HEALTH_H
#define HEALTH_H

#include "Arduino.h"

class Health()
{
public:
	double getTemp(XbeeInfo & Router);
	double getBattery(XbeeInfo & Router);
	double getPower(XbeeInfo & Router);

private:
	double Temp;
	double Battery;
	double Power;

};

#endif