
#include "ADS1248.h"


ADS1248::ADS1248(){
	    STARTPin = 16;
	    CSPin  = 6;
	    DRDYPin  = 7;
	    //MISO  = 50;
	    //MOSI  = 51;
	    //SCK = 52;
	    resetPin = 8;
}
void ADS1248::initialize(SPISettings ADC_Settings)
{
  //Set pin modes
  pinMode(STARTPin,OUTPUT);
  pinMode(CSPin,OUTPUT);
  pinMode(DRDYPin,INPUT);
  pinMode(resetPin,OUTPUT);
  
  pinMode(9,OUTPUT);
  
  //Set initial outputs
  digitalWrite(STARTPin,HIGH);
  digitalWrite(CSPin,HIGH);
  digitalWrite(9,HIGH);
  digitalWrite(resetPin,HIGH);
  
  //Start Serial Terminal 
  Serial.begin(115200);
  
  //Start SPI Library
  SPI.begin();
  
  delay(50); //Allow start up time
  
  //Setup ADC 
  
  setupADC(ADC_Settings); //Setup ADC configuration 
        //(resets, internal reference, vbias, self calibration, disable continuous stream)
  
}

String ADS1248::payloadMessage(SPISettings ADC_Settings){
	long X_raw = READ_DATA(MUX_CH1_X, OFCX,ADC_Settings);
	long Y_raw = READ_DATA(MUX_CH2_Y, OFCY,ADC_Settings);
	long Z_raw = READ_DATA(MUX_CH3_Z, OFCZ,ADC_Settings);
	long T_int = READ_TEMP(OFCT_int,ADC_Settings);      //page 35 188mv @ 25*C & 405uV/*C

	float X_float = ((5.0/8388608.0)*X_raw)/8.0 ; /////////////////////////////////////////////////////////////////// divided by 8 because scaling?
	float Y_float = ((5.0/8388608.0)*Y_raw)/8.0 ;
	float Z_float = ((5.0/8388608.0)*Z_raw)/8.0 ;
	float Tint_float = ((5.0/8388608.0)*T_int)/8.0;
	
	String retStr = "M"+String(X_float)+","+String(Y_float)+","+String(Z_float)+","+String(Tint_float);
	
	return retStr;
 
  
}


void ADS1248::setupADC(SPISettings ADC_Settings){

	
	///////////////////////////////////////////////////////////////////
	//Hardware Reset
	//Trigger pin RESET
	digitalWrite(resetPin,HIGH);
	delayMicroseconds(100000);
	digitalWrite(resetPin,LOW);		//Active LOW reset
	delayMicroseconds(100000);
	digitalWrite(resetPin,HIGH);
	delayMicroseconds(100000);						//Allow ADC to settle (0.6ms page 36)
	
	///////////////////////////////////////////////////////////////////
	//Enable SPI communications
	digitalWrite(CSPin,LOW);
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
	SPI.transfer(WREG_SYS0);
	SPI.transfer(0x00);
	SPI.transfer(SCR0_PGA1_SPS320);  //Gain of 4 @ 320sps
	delayMicroseconds(10000);
	
	
	SPI.transfer(RREG_SYS0);
	SPI.transfer(0x00);
	SPI.transfer(0xFF);
	delayMicroseconds(10000);
	
	///////////////////////////////////////////////////////////////////
	//Self Calibrate. Change MUX inputs, self calibrate, record calibration values
	//Set Analog Positive and Negative pairings (probably not necessary as this will be switching a lot of times)
	//X-axis
	SPI.transfer(WREG_MUX0);	//1st command byte: WREG Command
	SPI.transfer(0x00);	//2nd command byte: # of bytes
	SPI.transfer(MUX_CH1_X);	//Data Byte
	delayMicroseconds(20000);					//MUX settle time
	SPI.transfer(SELFOCAL);		//Send self calibration command for DC offset
	delayMicroseconds(100000);					//Allow settling time. Table can be found: [Page 41]
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
				//MUX settle time

	///////////////////////////////////////////////////////////////////
	//End SPI communications
	SPI.endTransaction();
	digitalWrite(CSPin,HIGH);
	
}

unsigned long ADS1248::READ_DATA(byte address, unsigned long offset,SPISettings ADC_Settings){
	//Address can be as follows: MUX_CH1_X, MUX_CH2_Y, MUX_CH3_Z, MUX_CH4_T
	
	///////////////////////////////////////////////////////////////////
	//Enable the SPI bus
	digitalWrite(CSPin,LOW);
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
	delayMicroseconds(1000);
	//delayMicroseconds(100);
		
	///////////////////////////////////////////////////////////////////
	//Send Read data command and transfer 24 bits
	SPI.transfer(RDATA);
	
	unsigned long MSB  = SPI.transfer(0x00);	//Most significant byte
	unsigned long MiSB = SPI.transfer(0x00);	//Middle significant byte
	unsigned long LSB  = SPI.transfer(0x00);	//Least significant byte
	
	//Testing twos compliment
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
	digitalWrite(CSPin,HIGH);
	
	///////////////////////////////////////////////////////////////////
	//Return Data as long (MSB is B00000000)
	return retVal;
	
}

unsigned long ADS1248::READ_TEMP(long offset,SPISettings ADC_Settings){
		///////////////////////////////////////////////////////////////////
	//Enable the SPI bus
	digitalWrite(CSPin,LOW);
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
	digitalWrite(CSPin,HIGH);
	
	///////////////////////////////////////////////////////////////////
	//Return Data as long (MSB is B00000000)
	return retVal;
}
