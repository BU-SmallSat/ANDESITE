#include "Arduino.h"
#include "XbeeInfo.h"

//with default my
XbeeInfo::XbeeInfo( long sh, long sl, String ni )
{
	_sl = sl;
	_sh = sh;
	_ni = ni;
	
	_my = 0xFFFE; // default MY address for all XBee modules; coordinator will give node a new MY once entered into the network
}

//with explicit my
XbeeInfo::XbeeInfo(long sh, long sl, long my, String ni)
{
    _sl = sl;
    _sh = sh;
    _ni = ni;
    _my = my;
}

//with explicit my but no ni
XbeeInfo::XbeeInfo(long sh, long sl, long my)
{
	_sh = sh;
	_sl = sl;
	_my = my;
}

long XbeeInfo::getSl()
{
	return _sl;
}

long XbeeInfo::getSh()
{
	return _sh;
}

long XbeeInfo::getMy()
{
	return _my;
}

String XbeeInfo::getNi()
{
	return _ni;
}

void XbeeInfo::setMy( long my )
{
	_my = my;
}

void XbeeInfo::setNi( String ni )
{	
	_ni = ni;
}

void XbeeInfo::setSh(long sh)
{
    _sh = sh;
}

void XbeeInfo::setSl(long sl)
{
    _sl = sl;
}


	
	