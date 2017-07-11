#ifndef ADS1248_H_
#define ADS1248_H_

#ifdef __cplusplus

#include <string.h>
#include <SPI.h>
#include "Arduino.h"
#include "libandesite.h"



//Commands
#define WAKEUP		 B00000000
#define SLEEP		 B00000010
#define SYNC		 B00000100
#define RESET		 B00000110
#define SYSOCAL		 B01100000
#define SYSGCAL		 B01000001
#define SELFOCAL	 B01100010
#define RDATA		 B00010010 //Read Data once
#define RDATAC		 B00010100 //Read data continuous
#define SDATAC		 B00010110 //Stop continuous read

//#define RREG			 B0010rrrr
#define RREG_MUX0		 B00100000
#define RREG_VBIAS		 B00100001
#define RREG_MUX1		 B00100010
#define RREG_SYS0		 B00100011
#define RREG_OFC0		 B00100100
#define RREG_OFC1		 B00100101
#define RREG_OFC2		 B00100110
#define RREG_FSC0		 B00100111
#define RREG_FSC1		 B00101000
#define RREG_FSC2		 B00101001
#define RREG_IDAC0		 B00101010
#define RREG_IDAC1		 B00101011
#define RREG_GPIOCFG	 B00101100
#define RREG_GPIODIR	 B00101101
#define RREG_GPIODAT	 B00101110

//#define WREG			 B0100rrrr
#define WREG_MUX0		 B01000000 //Set pairings
#define WREG_VBIAS		 B01000001  //Set bias from half supply
#define WREG_MUX1		 B01000010  //Set internal reference and calibration
#define WREG_SYS0		 B01000011  //Set PGA and Sampling rate
#define WREG_OFC0		 B01000100  //Write offset calibration values (3 bytes)
#define WREG_OFC1		 B01000101
#define WREG_OFC2		 B01000110
#define WREG_FSC0		 B01000111  //Write Full scale calibration values (3 bytes)
#define WREG_FSC1		 B01001000  // factory trimmed already
#define WREG_FSC2		 B01001001
#define WREG_IDAC0		 B01001010  //Current for sensor excitation (default is off)
#define WREG_IDAC1		 B01001011
#define WREG_GPIOCFG	 B01001100  //Set pins to GPIO state
#define WREG_GPIODIR	 B01001101	 //Set GPIO direction (default is output)
#define WREG_GPIODAT	 B01001110  //Set high or low (default is low)

// Register Values
//  Register Map table [Page 55]
#define MUX0  0x00
#define VBIAS  0x01
#define MUX1  0x02
#define SYS0  0x03
#define OFC0  0x04
#define OFC1  0x05
#define OFC2  0x06
#define FSC0  0x07
#define FSC1  0x08
#define FSC2  0x09
#define IDAC0 0x0A
#define IDAC1 0x0B
#define GPIOCFG 0x0C
#define GPIOCFG  0x0D
#define GPIODAT  0x0E


//MUX0 Values [Page 56]
// Select Channel Pairings
// [Burn-out Detect][ADC pos input][ADC neg input]
#define MUX_CH1_X  B00001000  // A0+ A1-
#define MUX_CH2_Y  B00100101  // A4+ A5-
#define MUX_CH3_Z  B00011010  // A3+ A2-
#define MUX_CH4_T  B00111110  // A7+ A6-

//VBIAS Values [Page 58]
// Apply (AVDD+AVSS)/2 voltage bias to inputs
#define VBIAS_DISABLE  B00000000 // Bias disabled
#define VBIAS_ENABLE   B11111111 // Mid supply applied to all A inputs

//MUX1 Values [Page 59]
// [CLKSTAT][VREF][REF SELECT][MUXCAL (pg 60)]
// [1 bit]	[2]	  [2]		  [3]
#define INTERNAL_REF    B00111000 //Internal Reference Always on; Selected and routed to REFP0 and REFN0
#define INTERNAL_REF_LP B01111000 //Internal Reference on when Converting; Selected and routed to REFP0 and REFN0
#define EXTERNAL_REF    B00001000 //Sets to default external reference to REFP1 and REFN1
#define TEMP_CONNECT	B00111011 //Same as INTERNAL_REF but connects temperature sense to inputs

//OFFSET Calibrate requires 0V diff applied to input pairs
//GAIN Calibrate requires full scale diff applied to input pairs
#define CALIBRATE_OFFSET  B00111001  //Using internal reference calibrate analog inputs
#define CALIBRATE_GAIN  B00111010

#define MONITOR_REFP  B00111100	//using internal reference measure reference
#define MONITOR_REFN  B00111101

//System Control Register 0
// Set PGA and Data output rate [Page 53]
// [0] [PGA] [DSPS]
#define SCR0_PGA1_SPS5       B00000000
#define SCR0_PGA2_SPS5       B00010000
#define SCR0_PGA4_SPS5       B00100000
#define SCR0_PGA8_SPS5       B00110000
#define SCR0_PGA16_SPS5      B01000000
#define SCR0_PGA32_SPS5      B01010000
#define SCR0_PGA64_SPS5      B01100000
#define SCR0_PGA128_SPS5     B01110000

#define SCR0_PGA1_SPS10      B00000001
#define SCR0_PGA2_SPS10      B00010001
#define SCR0_PGA4_SPS10      B00100001
#define SCR0_PGA8_SPS10      B00110001
#define SCR0_PGA16_SPS10     B01000001
#define SCR0_PGA32_SPS10     B01010001
#define SCR0_PGA64_SPS10     B01100001
#define SCR0_PGA128_SPS10    B01110001

#define SCR0_PGA1_SPS20      B00000010
#define SCR0_PGA2_SPS20      B00010010
#define SCR0_PGA4_SPS20      B00100010
#define SCR0_PGA8_SPS20      B00110010
#define SCR0_PGA16_SPS20     B01000010
#define SCR0_PGA32_SPS20     B01010010
#define SCR0_PGA64_SPS20     B01100010
#define SCR0_PGA128_SPS20    B01110010

#define SCR0_PGA1_SPS40      B00000011
#define SCR0_PGA2_SPS40      B00010011
#define SCR0_PGA4_SPS40      B00100011
#define SCR0_PGA8_SPS40      B00110011
#define SCR0_PGA16_SPS40     B01000011
#define SCR0_PGA32_SPS40     B01010011
#define SCR0_PGA64_SPS40     B01100011
#define SCR0_PGA128_SPS40    B01110011

#define SCR0_PGA1_SPS80      B00000100
#define SCR0_PGA2_SPS80      B00010100
#define SCR0_PGA4_SPS80      B00100100
#define SCR0_PGA8_SPS80      B00110100
#define SCR0_PGA16_SPS80     B01000100
#define SCR0_PGA32_SPS80     B01010100
#define SCR0_PGA64_SPS80     B01100100
#define SCR0_PGA128_SPS80    B01110100

#define SCR0_PGA1_SPS160     B00000101
#define SCR0_PGA2_SPS160     B00010101
#define SCR0_PGA4_SPS160     B00100101
#define SCR0_PGA8_SPS160     B00110101
#define SCR0_PGA16_SPS160    B01000101
#define SCR0_PGA32_SPS160    B01010101
#define SCR0_PGA64_SPS160    B01100101
#define SCR0_PGA128_SPS160   B01110101

#define SCR0_PGA1_SPS320     B00000110
#define SCR0_PGA2_SPS320     B00010110
#define SCR0_PGA4_SPS320     B00100110
#define SCR0_PGA8_SPS320     B00110110
#define SCR0_PGA16_SPS320    B01000110
#define SCR0_PGA32_SPS320    B01010110
#define SCR0_PGA64_SPS320    B01100110
#define SCR0_PGA128_SPS320   B01110110

#define SCR0_PGA1_SPS640     B00000111
#define SCR0_PGA2_SPS640     B00010111
#define SCR0_PGA4_SPS640     B00100111
#define SCR0_PGA8_SPS640     B00110111
#define SCR0_PGA16_SPS640    B01000111
#define SCR0_PGA32_SPS640    B01010111
#define SCR0_PGA64_SPS640    B01100111
#define SCR0_PGA128_SPS640   B01110111

#define SCR0_PGA1_SPS1000    B00001000
#define SCR0_PGA2_SPS1000    B00011000
#define SCR0_PGA4_SPS1000    B00101000
#define SCR0_PGA8_SPS1000    B00111000
#define SCR0_PGA16_SPS1000   B01001000
#define SCR0_PGA32_SPS1000   B01011000
#define SCR0_PGA64_SPS1000   B01101000
#define SCR0_PGA128_SPS1000  B01111000

#define SCR0_PGA1_SPS2000    B00001001
#define SCR0_PGA2_SPS2000    B00011001
#define SCR0_PGA4_SPS2000    B00101001
#define SCR0_PGA8_SPS2000    B00111001
#define SCR0_PGA16_SPS2000   B01001001
#define SCR0_PGA32_SPS2000   B01011001
#define SCR0_PGA64_SPS2000   B01101001
#define SCR0_PGA128_SPS2000  B01111001





class ADS1248 {

public:
	ADS1248();
    void initialize();
    String payloadMessage();
    unsigned long READ_DATA(byte address);
    unsigned long READ_TEMP();
	
	long Read_Register(byte waddress,byte raddress, byte value);
	void SetResetMag();
    void setupADC();
  
    ///////// Connections ////////////////////
    int START;
    int CS;
    int DRDY;
    int reset;
	int SetReset;
	SPISettings ADC_Settings;
  
};

extern ADS1248 _ADC;

#endif
#endif

