#ifndef GLOBALSTAR_H

#include <string.h>
#define MBUFSIZE 128;
#define FNSBUFFSIZE 3;
#define FNSIZE 100;
#define FSSIZE 6;
#define FSIZE 32000;
#define CRCSIZE;

class GlobalStar
{
public:
	//constructors
    GlobalStar();

    //member functions
    void SendFile(string fileName);

};

#define GLOBALSTAR_H
#endif