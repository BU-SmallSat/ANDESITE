#ifndef ADS1248_H_
#define ADS1248_H_

#ifdef __cplusplus

#include <string.h>
#include <SPI.h>
#include "Arduino.h"
#include "libandesite.h"


class ADS1248 {

public:
	ADS1248();
    void initialize(SPISettings ADC_Settings);
    String payloadMessage(SPISettings ADC_Settings);
    unsigned long READ_DATA(byte address, unsigned long offset,SPISettings ADC_Settings);
    unsigned long READ_TEMP(long offset,SPISettings ADC_Settings);

    void setupADC(SPISettings ADC_Settings);
  

    long OFCX,OFCY,OFCZ,OFCT,OFCT_int;

    ///////// Connections ////////////////////
    int STARTPin;
    int CSPin;
    int DRDYPin;
    //int MISO;
    //int MOSI;
    //int SCK;
    int resetPin;
	//SPI settings
	
  
};

extern ADS1248 _ADC;

#endif
#endif

