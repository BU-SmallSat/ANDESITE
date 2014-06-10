/*
 * Magnetometer.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: danielv775
 */

#include "Magnetometer.h"

Magnetometer::Magnetometer() {
	xCoordVoltage = 0;
	yCoordVoltage = 0;
	zCoordVoltage = 0;
}

double Magnetometer::get_xCoordVoltage()
{
	return xCoordVoltage;
}

double Magnetometer::get_yCoordVoltage()
{
	return yCoordVoltage;
}

double Magnetometer::get_zCoordVoltage()
{
	return zCoordVoltage;
}

//must store actual magnetometer data using pin information of magnetometer
double Magnetometer::setxyzVoltage(double xCoordVoltage, double yCoordVoltage, double zCoordVoltage)
{
	this->xCoordVoltage = xCoordVoltage;
	this->yCoordVoltage = yCoordVoltage;
	this->zCoordVoltage = zCoordVoltage;
}
