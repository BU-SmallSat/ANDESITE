#ifndef Potentiometer_h
#define Potentiometer_h

#include "Arduino.h"

class Potentiometer
{
  public:
    Potentiometer(int value, String timeStamp);
    int getValue();
    String getTimeStamp();
  private:
    int _value;
    String _timeStamp;
};
#endif