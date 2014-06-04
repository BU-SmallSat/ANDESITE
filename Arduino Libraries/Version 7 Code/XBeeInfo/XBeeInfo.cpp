#include "Arduino.h"
#include "XBeeInfo.h"

XBeeInfo::XBeeInfo( long sh, long sl, String ni )
{
	_sl = sl;
	_sh = sh;
	_ni = ni;
	
	_my = 0xFFFE; // default MY address for all XBee modules; coordinator will give node a new MY once entered into the network
}

XBeeInfo::XBeeInfo(long sh, long sl, String ni, int my)
{
    _sl = sl;
    _sh = sh;
    _ni = ni;
    _my = my;
}

XBeeInfo::XBeeInfo(long sh, long sl, int my)
{
	_sh = sh;
	_sl = sl;
	_my = my;
}

long XBeeInfo::getSL()
{
	return _sl;
}

long XBeeInfo::getSH()
{
	return _sh;
}

int XBeeInfo::getMY()
{
	return _my;
}

String XBeeInfo::getNI()
{
	return _ni;
}

void XBeeInfo::setMY( int my )
{
	_my = my;
}

void XBeeInfo::setNI( String ni )
{	
	_ni = ni;
}


	
	