#include "Arduino.h"
#include "Potentiometer.h"

Potentiometer::Potentiometer(int value, String timeStamp)
{
	_value = value;
    _timeStamp = timeStamp;
}

int Potentiometer::getValue()
{
	return _value;
}

String Potentiometer::getTimeStamp()
{
    return _timeStamp;
}