#ifndef MC33926_Arduino_FAST_PWM_h
#define MC33926_Arduino_FAST_PWM_h

#include <Arduino.h>

class MC33926_Arduino_FAST_PWM
{
  public:  
    // Constructor
    MC33926_Arduino_FAST_PWM(int D1, int INV, int FB, int notSF, int EN);
    // member functions
    void init(); // Initialize TIMER 1, set the PWM to 20kHZ. 
    void setSpeed(int speed); // Set speed for M1.
    unsigned int getCurrent(); // Get current reading for M1. 
    unsigned char getStatusFlag(); // Get fault reading.
    
  private:
    int _D1;
    int _INV;
    int _FB;
    int _notSF;
    int _EN;
    int MAX_SPEED;
};

#endif