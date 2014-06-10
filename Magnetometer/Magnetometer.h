/*
 * Magnetometer.h
 *
 *  Created on: Jun 10, 2014
 *      Author: danielv775
 */

#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

class Magnetometer {
public:
	Magnetometer();

	double get_xCoordVoltage();
	double get_yCoordVoltage();
	double get_zCoordVoltage();

	double setxyzVoltage(double xCoordVoltage, double yCoordVoltage, double zCoordVoltage);

private:
	double xCoordVoltage, yCoordVoltage, zCoordVoltage;

};

#endif /* MAGNETOMETER_H_ */
