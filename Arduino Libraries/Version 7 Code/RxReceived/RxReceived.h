#ifndef RxReceived_h
#define RxReceived_h
#include"Arduino.h"

class RxReceived
{
private:
#define shStartIndex 4 
#define shEndIndex 7
#define slStartIndex 8
#define slEndIndex 11
#define myStartIndex 12
#define myEndIndex 13
#define dataStartIndex 15
#define dataEndIndex 20
#define RX_RECEIVED 0x90
  uint8_t _packet[22];
  uint8_t _data[6];
  int holderLength;
  String _sl;
  String _sh;
  String _my;
  String holder;

public:
  RxReceived( uint8_t _packet[], int size );
  uint8_t getPacketAt( int n );
  uint8_t getDataAt( int n );
  String getSH();
  String getSL();
  String getMY();
  String findSection( int startIndex, int endIndex );
  void findData();
};
#endif