#include "Arduino.h"
#include "RxReceived.h"

RxReceived::RxReceived( uint8_t packet[], int size )
{

  for( int x = 0; x < size; x++ )
  {
    _packet[x] = packet[x]; //move elements from packet array to _packet array
  }
 
 _sh = findSection(shStartIndex, shEndIndex);
 _sl = findSection(slStartIndex, slEndIndex);
 _my = findSection(myStartIndex, myEndIndex);
 findData();
}

String RxReceived::findSection(int startIndex, int endIndex)
{
  String section = "";
  String transfer ="";

  for( int i = startIndex; i <= endIndex; i++ )
  {
    transfer = String( _packet[i], HEX);
	
	int length = transfer.length();
	
    if( length < 2 )
    {
      transfer = "0" + transfer;
    }

	else if ( length > 2 )
	{
		transfer = transfer.substring(length - 2);
	}

	section += transfer;
  }
    
  return section;
}

void RxReceived::findData()
{

  for( int x = dataStartIndex; x <= dataEndIndex; x++ )
  {
    _data[x - dataStartIndex] = _packet[x];
  }
}

uint8_t RxReceived::getPacketAt( int n )
{
  return _packet[n];
}

String RxReceived::getSH()
{
  return _sh;
}

String RxReceived::getSL()
{
  return _sl;
}

String RxReceived::getMY()
{
  return _my;
}

uint8_t RxReceived::getDataAt( int n )
{
  return _data[n];
}

