#ifndef Common_h
#define Common_h

#include"Arduino.h"

class Common 
{ 
  public:
    int getChecksum( byte packet[], int length );
    int getLSB( int packetLength );
    void toByteArray(int val, byte b[2]);
    int toInt(byte b[2]);
};
#endif
