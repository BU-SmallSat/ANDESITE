#include "Arduino.h"
#include "XbeeInfo.h"

//sets the serial high, serial low, and my variables
XbeeInfo::XbeeInfo(long sh, long sl, unsigned int my)
{
	_sh = sh;
	_sl = sl;
	_my = my;
}

//default constructor that sets the serial high, serial low, and 16-bit address to broadcast
XbeeInfo::XbeeInfo()
{
    _sh = 0;
    _sl = 0xffff;
    _my = 0xfffe;
}

// returns the serial low address
long XbeeInfo::getSl()
{
	return _sl;
}

// returns the serial high address
long XbeeInfo::getSh()
{
	return _sh;
}

// returns the 16-bit address
unsigned int XbeeInfo::getMy()
{
	return _my;
}

// sets the serial high address
void XbeeInfo::setSh(long sh)
{
    _sh = sh;
}

void XbeeInfo::setSl(long sl)
{
    _sl = sl;
}

// sets the 16-bit address
void XbeeInfo::setMy( unsigned int my )
{
	_my = my;
}


	
	
