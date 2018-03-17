
#include "ADS1248.h"


ADS1248::ADS1248(){
		START = 16;
		CS	= 6;
		DRDY	= 7;
		SetReset = A0;
		reset	= 8;
		ADC_Settings = SPISettings(4000000, MSBFIRST, SPI_MODE1); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
}
void ADS1248::initialize()
{
//Set pin modes
pinMode(START,OUTPUT);
pinMode(CS,OUTPUT);
pinMode(DRDY,INPUT);
pinMode(reset,OUTPUT);
pinMode(SetReset,OUTPUT);

pinMode(13,OUTPUT); //LED

//Set initial outputs
digitalWrite(START,HIGH);
digitalWrite(CS,HIGH);
digitalWrite(RESET,HIGH);
digitalWrite(SetReset,HIGH);

SetResetMag();

delay(50); //Allow start up time

//Setup ADC
setupADC(); //Setup ADC configuration
//(resets, internal reference, bias, self calibration, disable continuous stream)

SetResetMag();
}

String ADS1248::payloadMessage(){
	String retStr = "M";
	return retStr;
  
}


void ADS1248::setupADC(){
	///////////////////////////////////////////////////////////////////
	//Mag Reset
	SetResetMag();
	
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
	//Set PGA and sample rate 

	SPI.transfer(WREG_SYS0);
	SPI.transfer(0x00);
	SPI.transfer(SCR0_PGA64_SPS320);  
	delayMicroseconds(10000);
	
	///////////////////////////////////////////////////////////////////
	//End SPI communications
	SPI.endTransaction();
	digitalWrite(CS,HIGH);
}


long ADS1248::Read_Register(byte waddress,byte raddress, byte value){
	
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
	
}

unsigned long ADS1248::READ_DATA(byte address){
	
//Address can be as follows: MUX_CH1_X, MUX_CH2_Y, MUX_CH3_Z, MUX_CH4_T
	
	///////////////////////////////////////////////////////////////////
	//Enable the SPI bus
	digitalWrite(CS,LOW);
	SPI.beginTransaction(ADC_Settings);
	
	///////////////////////////////////////////////////////////////////
	//Set MUX to correct channel
	SPI.transfer(WREG_MUX0);	//1st command byte: WREG Command
	SPI.transfer(0x00);			//2nd command byte: # of bytes-1
	SPI.transfer(address);		//Data Byte
	delayMicroseconds(3200);
	//delayMicroseconds(100);
		
	///////////////////////////////////////////////////////////////////
	//Send Read data command and transfer 24 bits
	SPI.transfer(RDATA);
	
	unsigned long MSB  = SPI.transfer(0x00);	//Most significant byte
	unsigned long MiSB = SPI.transfer(0x00);	//Middle significant byte
	unsigned long LSB  = SPI.transfer(0x00);	//Least significant byte
	
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

	
	///////////////////////////////////////////////////////////////////
	//Return Data as long (MSB is B00000000)
	return retVal;
	
}

unsigned long ADS1248::READ_TEMP(){
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
	return retVal;
}

void ADS1248::SetResetMag(){
	digitalWrite(SetReset,LOW);
	digitalWrite(SetReset,HIGH);
}