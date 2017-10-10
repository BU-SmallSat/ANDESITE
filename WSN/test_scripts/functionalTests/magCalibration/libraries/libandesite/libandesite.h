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

//*************************************************//
//************CONFIGURABLE VARIABLES***************//
//*************************************************//
//GPS Timeout Variables
#define GPS_READ_TIMEOUT			1000
#define GPS_INIT_TIMEOUT			3000


//No Lock GPS Timeout Variables
#define SCIENCE_MODE_TIMEOUT		30000
#define TRANSFER_MODE_TIMEOUT		30000

#define MULE_LISTEN_TIMEOUT			30000

//Data sample rate
#define SIXTY_HZ_OCRIA					33332
#define SIXTY_HZ_TCCRIB					10


#define FIFTY_HZ_OCRIA					39999
#define FIFTY_HZ_TCCRIB					10


#define FOUTY_HZ_OCRIA					49999
#define FOURTY_HZ_TCCRIB				10

#define THIRTY_HZ_OCRIA					8332
#define THIRTY_HZ_TCCRIB				11

#define TEN_HZ_OCRIA					24999
#define TEN_HZ_TCCRIB					11





//*************************************************//
//****************STATIC REFERENCES****************//
//*************************************************//
//Battery Info
#define BATTERY_OUT				4

// Serial communications
#define ACDH_SERIAL_BAUD          (unsigned long) 115200
#define ACDH_GPS_BAUD             (unsigned long) 115200

// Pins
#define SD_CS_PIN               9		//low enable (select on low)
#define ADS_CS_PIN				6		//low enable (select on low
#define RF_CS_PIN				53		//low enable (select on low)
#define RF_SHDN_PIN				4		//high enable (shutdown on high)
#define ACDH_ADC_RESET_PIN		45
#define ACDH_ADC_SYNC_PIN		46
#define ACDH_ADC_XSS_PIN		47
#define ACDH_ADC_YSS_PIN		48
#define ACDH_ADC_ZSS_PIN		49


#define LED_ONE_PIN				13
#define LED_TWO_PIN				12
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



//************************************************************//
//***************NEED TO EDIT TO ONLY ESSENTIALS**************//
//************************************************************//


// ================
// Public functions
// ================

// LED setup
//void acdh_init_led();

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
//void acdh_led_set(int mode);


#endif
