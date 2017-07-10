/*
ADS1248 Code
http://www.ti.com/product/ads1248?qgpn=ads1248

Osi Van Dessel
12/15/2016


Woot 500 lines
*/

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
#define RREG_MUX0		 B00100001
#define RREG_VBIAS		 B00100010
#define RREG_MUX1		 B00100011
#define RREG_SYS0		 B00100100
#define RREG_OFC0		 B00100101
#define RREG_OFC1		 B00100110
#define RREG_OFC2		 B00100111
#define RREG_FSC0		 B00101000
#define RREG_FSC1		 B00101001
#define RREG_FSC2		 B00101010
#define RREG_IDAC0		 B00101011
#define RREG_IDAC1		 B00101100
#define RREG_GPIOCFG	 B00101101
#define RREG_GPIOCFG	 B00101110
#define RREG_GPIODAT	 B00101111

//#define WREG			 B0100rrrr
#define WREG_MUX0		 B01000001  //Set pairings
#define WREG_VBIAS		 B01000010  //Set bias from half supply
#define WREG_MUX1		 B01000011  //Set internal reference and calibration
#define WREG_SYS0		 B01000100  //Set PGA and Sampling rate
#define WREG_OFC0		 B01000101  //Write offset calibration values (3 bytes)
#define WREG_OFC1		 B01000110
#define WREG_OFC2		 B01000111
#define WREG_FSC0		 B01001000  //Write Full scale calibration values (3 bytes)
#define WREG_FSC1		 B01001001  // factory trimmed already
#define WREG_FSC2		 B01001010
#define WREG_IDAC0		 B01001011  //Current for sensor excitation (default is off)
#define WREG_IDAC1		 B01001100
#define WREG_GPIOCFG	 B01001101  //Set pins to GPIO state
#define WREG_GPIOCFG	 B01001110	 //Set GPIO direction (default is output)
#define WREG_GPIODAT	 B01001111  //Set high or low (default is low)

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



////////////////////////////////////////////////////////////
////////////////////// Connections /////////////////////////
////////////////////////////////////////////////////////////

const int START = 16;
const int CS	= 6;
const int DRDY	= 7;
const int SetReset = A0;
//const int MISO	= 50;
//const int MOSI	= 51;
//const int SCK	= 52;
const int reset	= 8;

////////////////////////////////////////////////////////////
//////////////////////// Libraries /////////////////////////
////////////////////////////////////////////////////////////
#include <Arduino.h>
#include <SPI.h>

//SPI settings
SPISettings ADC_Settings(4000000, MSBFIRST, SPI_MODE1); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
//SPI mode 0 as denoted on [Page 42]
//Max SPI setting is AVR is capable is 20MHz [20000000]
//According to Datasheet internal fclk is 4.096MHz


////////////////////////////////////////////////////////////
////////////////////////// NOTES ///////////////////////////
////////////////////////////////////////////////////////////
/*
Currently at PGA = 4 and SPS = 320 ENOB is 18.8 (16) [Page 22]
with MUX settling time of ~6.24ms (note that 30Hz is ~33ms)
*/


//Global Variables
long OFCX,OFCY,OFCZ,OFCT,OFCT_int;
void setup()
{
	//Set pin modes
	pinMode(START,OUTPUT);
	pinMode(CS,OUTPUT);
	pinMode(DRDY,INPUT);
	pinMode(reset,OUTPUT);
	pinMode(SetReset,OUTPUT);
	
	pinMode(13,OUTPUT); //LED

	
	pinMode(9,OUTPUT); //RADIO CS
	
	
	
	
	//Set initial outputs
	digitalWrite(START,HIGH);
	digitalWrite(CS,HIGH);
	digitalWrite(9,HIGH);
	digitalWrite(RESET,HIGH);
	digitalWrite(SetReset,HIGH);
	
	//SetResetMag();
	
	//Start Serial Terminal 
	Serial.begin(115200);
	
	//Start SPI Library
	SPI.begin();
	
	delay(50); //Allow start up time
	
	//Setup ADC 
//	while(1){
	setupADC(); //Setup ADC configuration 
				//(resets, internal reference, vbias, self calibration, disable continuous stream)
//	delay(1);
//	}
//	while(1){
	
//	delay(10);
//	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
	digitalWrite(13,!digitalRead(13));


		
	// Read ADC inputs (requires MUX switching)
//	while(1){Read_Register(WREG_MUX0,RREG_MUX0,MUX_CH1_X);}
		
	long X_raw = READ_DATA(MUX_CH1_X, OFCX);
	long Y_raw = READ_DATA(MUX_CH2_Y, OFCY);
	long Z_raw = READ_DATA(MUX_CH3_Z, OFCZ);
		
//		while(1){Read_Register(WREG_MUX0,RREG_MUX0,MUX_CH2_Y);}
//	unsigned long T_raw = READ_DATA(MUX_CH4_T, OFCT);
//	unsigned long T_int = READ_TEMP(OFCT_int);			//page 35 188mv @ 25*C & 405uV/*C
												//Note that function doesn't change the PGA so multiply by 4
	

	
	// Print out Raw ADC outputs
	float voltageX = ((5.0/8388608.0)*X_raw)/8.0 ; /////////////////////////////////////////////////////////////////// divided by 8 because scaling?
	float voltageY = ((5.0/8388608.0)*Y_raw)/8.0 ;
	float voltageZ = ((5.0/8388608.0)*Z_raw)/8.0 ;
	Serial.println(voltageX,6);
	Serial.println(voltageY,6);
	Serial.println(voltageZ,6);
//	Serial.println(Y_raw);
//	Serial.println(Z_raw);
	

	/*
	Serial.print("  ,  ");
	Serial.print(Y_raw,BIN);
	Serial.print("  |  ");
	Serial.print(Z_raw,BIN);
	Serial.print("  |  ");
	Serial.print(T_raw,BIN);
	Serial.print("  |  ");
	Serial.print(T_int,BIN);
	Serial.print("\n");
	Serial.print(X_raw);
	Serial.print("  |  ");
	Serial.print(Y_float,2);
	Serial.print("  |  ");
	Serial.print(Z_float,2);
	Serial.print("  |  ");
	Serial.print(T_float,2);
	Serial.print("  |  ");
	Serial.print(Tint_float,2);
*/
	
	//Serial.println("    ");
	
	//delay(10);
	
}



void setupADC(){
	
	///////////////////////////////////////////////////////////////////
	//Hardware Reset
	//Trigger pin RESET
	digitalWrite(reset,HIGH);
	delayMicroseconds(100000);
	digitalWrite(reset,LOW);		//Active LOW reset
	delayMicroseconds(100000);
	digitalWrite(reset,HIGH);
	delayMicroseconds(100000);						//Allow ADC to settle (0.6ms page 36)
	
	///////////////////////////////////////////////////////////////////
	//Enable SPI communications
	digitalWrite(CS,LOW);
	SPI.beginTransaction(ADC_Settings);
	
	///////////////////////////////////////////////////////////////////
	//Software Reset
	SPI.transfer(RESET);			//Allow ADC to settle (0.6ms page 46)
	delayMicroseconds(10000);
	
	///////////////////////////////////////////////////////////////////
	//Disable continuous Data stream
	SPI.transfer(SDATAC);
	delayMicroseconds(10000);
	
	///////////////////////////////////////////////////////////////////
	//Set internal reference
	SPI.transfer(WREG_MUX1);
	SPI.transfer(0x00);
	SPI.transfer(INTERNAL_REF);		//Set to internal reference and internally connect to ref pins
									//EXTERNAL_REF
	delayMicroseconds(10000);

	SPI.transfer(RREG_MUX1);
	SPI.transfer(0x00);
	SPI.transfer(0xFF);
	delayMicroseconds(10000);

	///////////////////////////////////////////////////////////////////
	//Set PGA and sample rate 
	//while(1){
	SPI.transfer(WREG_SYS0);
	SPI.transfer(0x00);
	SPI.transfer(SCR0_PGA16_SPS640);  
	delayMicroseconds(10000);
	
	
//	SPI.transfer(RREG_SYS0);
//	SPI.transfer(0x00);
//	SPI.transfer(0xFF);
//	delayMicroseconds(10000);
//	}
	
	///////////////////////////////////////////////////////////////////
	//Self Calibrate. Change MUX inputs, self calibrate, record calibration values
	//Set Analog Positive and Negative pairings (probably not necessary as this will be switching a lot of times)
	//X-axis
//	SPI.transfer(WREG_MUX0);	//1st command byte: WREG Command
//	SPI.transfer(0x00);	//2nd command byte: # of bytes
//	SPI.transfer(MUX_CH1_X);	//Data Byte
//	delayMicroseconds(20000);					//MUX settle time
//	SPI.transfer(SELFOCAL);		//Send self calibration command for DC offset
//	delayMicroseconds(100000);					//Allow settling time. Table can be found: [Page 41]
	/*
	SPI.transfer(RREG_OFC0);			//Send Read Command for Offset byte 1
	SPI.transfer(0x00);			//# of bytes expected	
	byte OFC0X = SPI.transfer(0x00);	//Clock out Byte	
	SPI.transfer(RREG_OFC1);			//Repeat for other 2 bytes
	SPI.transfer(0x00);
	byte OFC1X = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC2);
	SPI.transfer(0x00);
	byte OFC2X = SPI.transfer(0x00);	
	
	////////////////////////////////////////
	//Y-axis
	SPI.transfer(WREG_MUX0);	
	SPI.transfer(0x00);	
	SPI.transfer(MUX_CH2_Y);	
	delay(20);
	SPI.transfer(SELFOCAL);		
	delay(100);		
	
	SPI.transfer(RREG_OFC0);
	SPI.transfer(0x00);
	byte OFC0Y = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC1);
	SPI.transfer(0x00);
	byte OFC1Y = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC2);
	SPI.transfer(0x00);
	byte OFC2Y = SPI.transfer(0x00);			
	
	////////////////////////////////////////		
	//Z-axis
	SPI.transfer(WREG_MUX0);
	SPI.transfer(0x00);
	SPI.transfer(MUX_CH3_Z);
	delay(20);
	SPI.transfer(SELFOCAL);
	delay(100);
	
	SPI.transfer(RREG_OFC0);
	SPI.transfer(0x00);
	byte OFC0Z = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC1);
	SPI.transfer(0x00);
	byte OFC1Z = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC2);
	SPI.transfer(0x00);
	byte OFC2Z = SPI.transfer(0x00);
	
	////////////////////////////////////////
	//External temperature
	SPI.transfer(WREG_MUX0);
	SPI.transfer(0x00);
	SPI.transfer(MUX_CH4_T);
	delay(20);
	SPI.transfer(SELFOCAL);
	delay(100);
	
	SPI.transfer(RREG_OFC0);
	SPI.transfer(0x00);
	byte OFC0T = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC1);
	SPI.transfer(0x00);
	byte OFC1T = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC2);
	SPI.transfer(0x00);
	byte OFC2T = SPI.transfer(0x00);	
	
	////////////////////////////////////////
	//Internal temperature
	SPI.transfer(WREG_MUX1);
	SPI.transfer(0x00);
	SPI.transfer(TEMP_CONNECT);
	delay(20);
	SPI.transfer(SELFOCAL);
	delay(100);
	
	SPI.transfer(RREG_OFC0);
	SPI.transfer(0x00);
	byte OFC0T_int = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC1);
	SPI.transfer(0x00);
	byte OFC1T_int = SPI.transfer(0x00);
	SPI.transfer(RREG_OFC2);
	SPI.transfer(0x00);
	byte OFC2T_int = SPI.transfer(0x00);
	
	////////////////////////////////////////
	//COMPILE offset calibration into Global variable
	OFCX = OFC0X << 16 | OFC1X << 8 | OFC2X ;
	OFCY = OFC0Y << 16 | OFC1Y << 8 | OFC2Y ;
	OFCZ = OFC0Z << 16 | OFC1Z << 8 | OFC2Z ;
	OFCT = OFC0T << 16 | OFC1T << 8 | OFC2T ;
	OFCT_int = OFC0T_int << 16 | OFC1T_int << 8 | OFC2T_int;*/

	//Set mux to ch1
//	SPI.transfer(WREG_MUX0);	//1st command byte: WREG Command
//	SPI.transfer(0x00);	//2nd command byte: # of bytes
//	SPI.transfer(MUX_CH1_X);	//Data Byte
//	delay(20);					//MUX settle time

	///////////////////////////////////////////////////////////////////
	//End SPI communications
	SPI.endTransaction();
	digitalWrite(CS,HIGH);
	
}

long Read_Register(byte waddress,byte raddress, byte value){
	
	///////////////////////////////////////////////////////////////////
	//Enable SPI communications
	digitalWrite(CS,LOW);
	SPI.beginTransaction(ADC_Settings);
	
	SPI.transfer(waddress);
	SPI.transfer(0x00);
	SPI.transfer(value);  //Gain of 4 @ 320sps
	delayMicroseconds(1);
		
		
	SPI.transfer(raddress);
	SPI.transfer(0x00);
	SPI.transfer(0xFF);
	delayMicroseconds(2);
	
	///////////////////////////////////////////////////////////////////
	//End SPI communications
	SPI.endTransaction();
	digitalWrite(CS,HIGH);
	
	/*
	//MUX1 Values [Page 59]
	// [CLKSTAT][VREF][REF SELECT][MUXCAL (pg 60)]
	// [1 bit]	[2]	  [2]		  [3]
	#define INTERNAL_REF    B 0 01 11 000 //Internal Reference Always on; Selected and routed to REFP0 and REFN0
	#define INTERNAL_REF_LP B01111000 //Internal Reference on when Converting; Selected and routed to REFP0 and REFN0
	#define EXTERNAL_REF    B00001000 //Sets to default external reference to REFP1 and REFN1
	#define TEMP_CONNECT	B00111011 //Same as INTERNAL_REF but connects temperature sense to inputs
	
	#define RREG_MUX1		 B00100011
	*/
	
}

long READ_DATA(byte address, unsigned long offset){
	//Address can be as follows: MUX_CH1_X, MUX_CH2_Y, MUX_CH3_Z, MUX_CH4_T
	
	///////////////////////////////////////////////////////////////////
	//Enable the SPI bus
	digitalWrite(CS,LOW);
	SPI.beginTransaction(ADC_Settings);
	
/*	///////////////////////////////////////////////////////////////////
	//Write Offset Calibration values
	SPI.transfer(WREG_OFC0);
	SPI.transfer(0x00);
	SPI.transfer(offset >> 16 & 0xFF);
	SPI.transfer(WREG_OFC1);
	SPI.transfer(0x00);
	SPI.transfer(offset >> 8 & 0xFF);
	SPI.transfer(WREG_OFC2);
	SPI.transfer(0x00);
	SPI.transfer(offset & 0xFF);*/

	
	///////////////////////////////////////////////////////////////////
	//Set MUX to correct channel
	SPI.transfer(WREG_MUX0);	//1st command byte: WREG Command
	SPI.transfer(0x00);			//2nd command byte: # of bytes-1
	SPI.transfer(address);		//Data Byte
	delayMicroseconds(10000);
	//delayMicroseconds(100);
		
	///////////////////////////////////////////////////////////////////
	//Send Read data command and transfer 24 bits
	SPI.transfer(RDATA);
	
	unsigned long MSB  = SPI.transfer(0x00);	//Most significant byte
	unsigned long MiSB = SPI.transfer(0x00);	//Middle significant byte
	unsigned long LSB  = SPI.transfer(0x00);	//Least significant byte
	
	//50
	//00110010
	
	//-467467
	//11111000 11011101 11110101
	//B11111000; //
	//B11011101;//
	//B11110101;//
	
	// -50
	//B11111111;
	//B11111111; //
	//B11001110; //

	///////////////////////////////////////////////////////////////////
	//End SPI transaction
	SPI.endTransaction();
	digitalWrite(CS,HIGH);
		
	
	unsigned long highlong		= ((MSB << 16) & 0x00FF0000);
	unsigned long middlelong	= (MiSB << 8)  & 0x0000FF00;
	unsigned long lowlong		= (LSB) & 0x000000FF;
	unsigned long highest		= 0x00;
	unsigned long retVal		= 0;
	
	if ((MSB & 0x80) == 0x80) highest = 0xFF000000;
	
	retVal	=	highest | highlong | middlelong | lowlong;
	
	
	//long DataSum = MSB*65536 + MiSB*256 + LSB; //((0x00 << 24) | (MSB << 16) | (MiSB << 8) | LSB) ; //Compile data bytes together 


	
	///////////////////////////////////////////////////////////////////
	//Return Data as long (MSB is B00000000)
	return retVal;
	
}

long READ_TEMP(long offset){
		///////////////////////////////////////////////////////////////////
	//Enable the SPI bus
	digitalWrite(CS,LOW);
	SPI.beginTransaction(ADC_Settings);
	
	///////////////////////////////////////////////////////////////////
	//Set MUX to correct channel
	SPI.transfer(WREG_MUX1);	//1st command byte: WREG Command
	SPI.transfer(0x00);	//2nd command byte: # of bytes
	SPI.transfer(TEMP_CONNECT);		//Data Byte
	delay(5);
/*	
	///////////////////////////////////////////////////////////////////
	//Write Offset Calibration values
	SPI.transfer(WREG_OFC0);
	SPI.transfer(0x00);
	//SPI.transfer(0x00);
	SPI.transfer(offset >> 16 & 0xFF);
	SPI.transfer(WREG_OFC1);
	SPI.transfer(0x00);
	//SPI.transfer(0x00);
	SPI.transfer(offset >> 8 & 0xFF);
	SPI.transfer(WREG_OFC2);
	SPI.transfer(0x00);
	//SPI.transfer(0x00);
	SPI.transfer(offset & 0xFF);*/
	
	///////////////////////////////////////////////////////////////////
	//Begin transfer
	SPI.transfer(RDATA);
	unsigned long MSB  = SPI.transfer(0x00);	//Most significant byte
	unsigned long MiSB = SPI.transfer(0x00);	//Middle significant byte
	unsigned long LSB  = SPI.transfer(0x00);	//Least significant byte
	
	unsigned long highlong		= ((MSB << 16) & 0x00FF0000);
	unsigned long middlelong	= (MiSB << 8)  & 0x0000FF00;
	unsigned long lowlong		= (LSB) & 0x000000FF;
	unsigned long highest		=  0x00;
	unsigned long retVal		= 0;
	
	if ((MSB & 0x80) == 0x80) highest = 0xFF000000;
	
	retVal	=	highest | highlong | middlelong | lowlong;
	///////////////////////////////////////////////////////////////////
	//End SPI transaction
	SPI.endTransaction();
	digitalWrite(CS,HIGH);
	
	///////////////////////////////////////////////////////////////////
	//Return Data as long (MSB is B00000000)
	return retVal;
}

void SetResetMag(){
	digitalWrite(SetReset,LOW);
	delayMicroseconds(20);
	digitalWrite(SetReset,HIGH);	
}
