// Header guard
#ifndef LIBANDESITE_H
#define LIBANDESITE_H

// ========
// Includes
// ========

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SPI.h>
#include <Wire.h>


// =======
// Defines
// =======

// Serial communications
#define ACDH_SERIAL_BAUD          (unsigned long) 115200
#define ACDH_GPS_BAUD             (unsigned long) 115200

// Pins
#define ACDH_SS_PIN               53
#define ACDH_SD_PIN               9
#define ACDH_ADC_RESET_PIN		  45
#define ACDH_ADC_SYNC_PIN		  46
#define ACDH_ADC_XSS_PIN		  47
#define ACDH_ADC_YSS_PIN		  48
#define ACDH_ADC_ZSS_PIN		  49

// Addresses
#define ACDH_WSN_NUM      8

#define ACDH_MULE_ADDR    1
#define ACDH_WSN1_ADDR    2
#define ACDH_WSN2_ADDR    3
#define ACDH_WSN3_ADDR    4
#define ACDH_WSN4_ADDR    5
#define ACDH_WSN5_ADDR    6
#define ACDH_WSN6_ADDR    7
#define ACDH_WSN7_ADDR    8
#define ACDH_WSN8_ADDR    9

// Osi pins for power
#define eps_shutdown          29 // set to high for on
#define eps_inverter_shutdown 28 // set to low for on


//Command codes for ADC 
extern const byte DataByte0;  //NOTE: Transfer a single byte all the time
extern const byte DataByte1;
extern const byte DataByte2;
extern const byte ReadControlByte0;
extern const byte ReadControlByte1;
extern const byte ReadControlByte2;
extern const byte WriteControlByte0;
extern const byte WriteControlByte1;
extern const byte WriteControlByte2;

extern const byte Standby;  //*note: gain will need to be reset after exiting standby
extern const byte Gain1;
extern const byte Gain2;
extern const byte Gain4;
extern const byte Gain8;
extern const byte Gain16;
extern const byte Gain32;
extern const byte Gain64;
extern const byte Gain128;  

extern const byte C1Filter1000; //Control Byte 1
extern const byte C1Filter500;
extern const byte C1Filter200;
extern const byte C1Filter100;
extern const byte C1Filter50;
extern const byte C1Filter30;


extern const byte writeControl;
extern const byte C0test;
extern const byte C1test; //
extern const byte C2test;

extern const byte C2Filter1000; //Control Byte 2
extern const byte C2Filter500;
extern const byte C2Filter200;
extern const byte C2Filter100;
extern const byte C2Filter50;
extern const byte C2Filter30;

extern const byte conversionStart;

//Digital Potentiometer control bytes
extern const byte Xaddress; 
extern const byte Yaddress;
extern const byte Zaddress; 
extern const byte CommandTapA;  //just change wipper A 
extern const byte CommandTapB;  //just change wipper B
extern const byte CommandTapAB;  //change both wipper A and B



// ================
// Public functions
// ================

// LED setup
void acdh_init_led();

// Compression/decompression
int acdh_strlen(uint8_t *str);
int acdh_str_compress(uint8_t *hex, const char *str, int n, int nmax);
void acdh_str_decompress(char *str, uint8_t *hex, int n);

// Instrument drivers
void acdh_adc_setup();
unsigned int acdh_adc_readData(int ChipSelect, int SyncPin, int ResetPin);

// Digital potentiometer control
void acdh_digipot_setup(byte Address);
void acdh_digipot_gainset(byte Address, int Gain);

// Setup visual aid LEDs
void acdh_led_set(int mode);


#endif
