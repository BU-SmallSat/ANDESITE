// ** Things to Do **
// Check return value form Sd.open
// How to format SD card from board

// ========
// Includes
// ========

#include "libandesite.h"


// ========
// Declares
// ========

//Command codes for ADC 
const byte DataByte0 = B00000000;  //NOTE: Transfer a single byte all the time
const byte DataByte1 = B00000001;
const byte DataByte2 = B00000010;
const byte ReadControlByte0 = B00000100;
const byte ReadControlByte1 = B00000101;
const byte ReadControlByte2 = B00000110;
const byte WriteControlByte0 = B10000100;
const byte WriteControlByte1 = B10000101;
const byte WriteControlByte2 = B10000110;

const byte Standby = B00001001;  //*note: gain will need to be reset after exiting standby
const byte Gain1   = B00000001;
const byte Gain2   = B00100001;
const byte Gain4   = B01000001;
const byte Gain8   = B01100001;
const byte Gain16  = B10000001;
const byte Gain32  = B10100001;
const byte Gain64  = B11000001;
const byte Gain128 = B11100001;  

const byte C1Filter1000 = B0100001;//Control Byte 1
const byte C1Filter500  = B0111001;
const byte C1Filter200  = B0010001;
const byte C1Filter100  = B0011001;
const byte C1Filter50   = B0111001;
const byte C1Filter30   = B1100001;

const byte C2Filter1000 = B0000001; //Control Byte 2
const byte C2Filter500  = B0000010;
const byte C2Filter200  = B0000110;
const byte C2Filter100  = B0001100;
const byte C2Filter50   = B0011000;
const byte C2Filter30   = B0101000;

const byte conversionStart = B0010001;

//Digital Potentiometer control bytes
const byte Xaddress = B0101000; 
const byte Yaddress = B0101001;
const byte Zaddress = B0101011; 
const byte CommandTapA = B00010001;  //just change wipper A 
const byte CommandTapB = B00010010;  //just change wipper B
const byte CommandTapAB = B0000011;  //change both wipper A and B





// ////////////////////////////////
// ///// MULE/WSN BOARD SETUP /////
// ////////////////////////////////

// Setup the LEDs on the board
void acdh_init_led() {
    pinMode(12,OUTPUT);
    pinMode(13,OUTPUT);
    pinMode(77,OUTPUT);
    pinMode(78,OUTPUT);
    pinMode(79,OUTPUT);
	
    digitalWrite(12,LOW);
    digitalWrite(13,LOW);
    digitalWrite(77,LOW);
    digitalWrite(78,LOW);
    digitalWrite(79,LOW);
}



// //////////////////////////////////////////////
// ///// DATA COMPRESSION AND DECOMPRESSION /////
// //////////////////////////////////////////////

// Return length of a string
int acdh_strlen(uint8_t *str) {
	int i = 0;
	while ( str[i] != 0 ) 
		++i;
	
	return i;
}



// Compress a string (cuts off by one char, fix this shit)
int acdh_str_compress(uint8_t *hex, const char *str, int n, int nmax) {
	
	// Loop variables
    int i = 0, 
		j = 0, 
		count = 0;
    uint8_t temp = 0, c = 0;
	
	// Iterate through chars and convert them
    for ( i = 0; i < n; ++i ) {
		
		// Clear the conversion container before using it
        if ( i < nmax )
			hex[i] = 0;
        
		// Change/skip certain variables
       if ( str[i] == 'M' )
            temp = 0xA1;
        else if ( str[i] == 'G' )
            temp = 0xA2;
        else if ( str[i] == 'T' )
            temp = 0xA3;
		else if ( str[i] == ':' )
			temp = 0xB;
		else if ( str[i] == '0' )
			temp = 0xC;
        else if ( str[i] == '.' )
            temp = 0xD;
        else if ( str[i] == '-' )
            temp = 0xE;
        else if ( str[i] == ',' )
            temp = 0xF;
        else if ( (str[i] >= '1') && (str[i] <= '9') )
            temp = str[i] - '0';
		else
			continue;
		
		// Byte that indicates data type (mag/gps/gyro) 
		if ( temp >= 0xA1 ) {
			
			// Already looked at char, add it before adding data type indicator
			if ( count ) {
				hex[j] = c + 0xA;
				count = 0;
				++j;
			}
			
			// Add data type indicator
			hex[j] = temp;
			count = 0;
			++j;
			continue;
		}
        
		// Pack the first/second nibble (half a byte) into the MSB/LSB
		// Then add resultant byte to compression container
        if ( !count ) {
            c = temp << 4;
            ++count;
        } else {
			hex[j] = c + temp;
            count = 0;
            ++j;
        }
    }
	
	// Add unpacked MSB nibble to end of compression container
	if ( count && (j < nmax) ) {
		hex[j] = c + 0xA;
		++j;
	}
	
    return j;
}



// Decompress the string
void acdh_str_decompress(char *str, uint8_t *hex, int n) {
	
	// Loop variables
    int i,
        j,
        k = 0, 
		sz = 2*n;
    
	// Decompression container
    uint8_t c;
    
	// Iterate through compressed string
    for ( i = 0; i < n; ++i ) {
		
		// Iterate through each nibble of a byte (MSB first)
        for ( j = 1; j >= 0; --j ) {
			
			// Isolate nibble
            c = (hex[i] >> (4*j)) & 0xf;
            
			// Decompress data header of type mag/gps/gyro
            if ( (hex[i] >= 0xa1) && (hex[i] <= 0xa3) ) {
				if ( j ) 
					continue;
				else if ( !j ) {
					if ( c == 0x1 )
						c = 'M';
					else if ( c == 0x2 )
						c = 'G';
					else if ( c == 0x3 )
						c = 'T';
				}
			} else {
				
				// Convert nibble to a char (only A-F are greater than 0xa)			
				if ( c >= 0xa ) {
					c = c + 'A' - 0xa;
					
					if ( c == 'A' )
						continue;
					else if ( c == 'B' )
						c = ':';
					else if ( c == 'C' )
						c = '0';
					else if ( c == 'D' )
						c = '.';
					else if ( c == 'E' )
						c = '-';
					else if ( c == 'F' )
						c = ',';
				} else
					c = c + '0';
			}
			
			// Add to decompression container
            str[k] = c;
            ++k;
		}
	}
    
	// Add null character
	if ( k <= (sz - 1) )
		str[k] = 0;
	
    return;
}

// //////////////////////////////////////
// /////     Instrument Drivers     /////
// //////////////////////////////////////

void acdh_adc_setup(){
    //ACDH_ADC_XSS_PIN, ACDH_ADC_SYNC_PIN, ACDH_ADC_RESET_PIN)
    //local variables
    // SPI.end();
  
    byte temp=0;
    const int sck = 52;
    const int sdio = 51;
  
    //First set the correct pinmodes
    pinMode(ACDH_ADC_SYNC_PIN, OUTPUT); 
    pinMode(ACDH_ADC_RESET_PIN, OUTPUT);
  
    pinMode(ACDH_ADC_XSS_PIN,OUTPUT);
    pinMode(ACDH_ADC_YSS_PIN,OUTPUT);
    pinMode(ACDH_ADC_ZSS_PIN,OUTPUT);
    digitalWrite(ACDH_ADC_XSS_PIN,HIGH);
    digitalWrite(ACDH_ADC_YSS_PIN,HIGH);
    digitalWrite(ACDH_ADC_ZSS_PIN,HIGH);
    
    //Reset the ADC
    digitalWrite(ACDH_ADC_RESET_PIN,HIGH);
    delay(10); //10 millisecond pulse (minimum is 100ns)
    digitalWrite(ACDH_ADC_RESET_PIN,LOW);
    delay(10); //10 millisecond pulse (minimum is 100ns)
    digitalWrite(ACDH_ADC_RESET_PIN,HIGH);
  
    //initialize the ADCs
    digitalWrite(ACDH_ADC_SYNC_PIN, HIGH); //active low pin, set low after finishing setup
 
    //do X first
    digitalWrite(ACDH_ADC_XSS_PIN, LOW);  
    //Use Shiftout to first intialize the ADC to use regular SPI communications
    // when the ADC first starts it uses the sdio as a bidirectional pin
    // by setting Control Byte 0 with the appropriate value we use regular SPI comms.
    pinMode(sck,OUTPUT);
    pinMode(sdio,OUTPUT);
    shiftOut(sdio,sck,MSBFIRST,WriteControlByte0);
    delay(1);
    shiftOut(sdio,sck,MSBFIRST,Gain8); //B01100001;
    delay(1);
    digitalWrite(ACDH_ADC_XSS_PIN,HIGH);
  
    digitalWrite(ACDH_ADC_YSS_PIN, LOW);  
    //Use Shiftout to first intialize the ADC to use regular SPI communications
    // when the ADC first starts it uses the sdio as a bidirectional pin
    // by setting Control Byte 0 with the appropriate value we use regular SPI comms.
    pinMode(sck,OUTPUT);
    pinMode(sdio,OUTPUT);
    shiftOut(sdio,sck,MSBFIRST,WriteControlByte0);
    delay(1);
    shiftOut(sdio,sck,MSBFIRST,Gain8); //B01100001;
    delay(1);
    digitalWrite(ACDH_ADC_YSS_PIN,HIGH);
  
    digitalWrite(ACDH_ADC_ZSS_PIN, LOW);  
    //Use Shiftout to first intialize the ADC to use regular SPI communications
    // when the ADC first starts it uses the sdio as a bidirectional pin
    // by setting Control Byte 0 with the appropriate value we use regular SPI comms.
    pinMode(sck,OUTPUT);
    pinMode(sdio,OUTPUT);
    shiftOut(sdio,sck,MSBFIRST,WriteControlByte0);
    delay(1);
    shiftOut(sdio,sck,MSBFIRST,Gain8); //B01100001;
    delay(1);
    digitalWrite(ACDH_ADC_ZSS_PIN,HIGH);

    //--------------------------------------------------------------------------------
    digitalWrite(ACDH_ADC_XSS_PIN,LOW);
    // start the SPI library:
    SPI.begin();
    //Make sure that the hardware chipselect pin is an output
    pinMode(53, OUTPUT);
    digitalWrite(53,HIGH); //make sure that the regular SS pin is an output and NOT active
    //setup the SPI library
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV4); //16MHz divided by 4 = 4MHz SPI speed
//#define NEWWAY
#ifdef NEWWAY 
//to get raw data from the ADC write 000000 (h) to the Offset Calibration Register, 800000 (h) to the Positive Full Scale Calibration Register,and 800000 (h) to the Negative Full Scale Calibration Register

    //zero out the offset calibration register
    SPI.transfer(0b11001010);
    SPI.transfer(0x00);
    //SPI.transfer(0b10001001);
    SPI.transfer(0x00);
    //SPI.transfer(0b10001010);
    SPI.transfer(0x00);

    //write 800000h to positive full scale calibration register
    SPI.transfer(0b11001110);
    SPI.transfer(0x80);
    SPI.transfer(0x00);
    SPI.transfer(0x00);

    //write 800000h to the negative full scale calibration register
    SPI.transfer(0b11011110);
    SPI.transfer(0x80);
    SPI.transfer(0x00);
    SPI.transfer(0x00);


    SPI.transfer(0b00001000);
    temp = SPI.transfer(0x00);
    Serial.print("offset calibration register 2: ");
    Serial.print(temp, BIN);
    Serial.print(" 1: ");
    SPI.transfer(0b00001001);
    temp = SPI.transfer(0x00);
    Serial.print(temp, BIN);
    SPI.transfer(0b00001010);
    temp = SPI.transfer(0x00);
    Serial.print(" 0: ");
    Serial.println(temp, BIN);

    SPI.transfer(0b01001110);
    Serial.print("positive calibration register 2: ");
    Serial.print(SPI.transfer(0x00), BIN);
    Serial.print(" 1: ");
    Serial.print(SPI.transfer(0x00), BIN);
    Serial.print(" 0: ");
    Serial.println(SPI.transfer(0x00), BIN);

    SPI.transfer(0b01011110);
    Serial.print("negative calibration register 2: ");
    Serial.print(SPI.transfer(0x00), BIN);
    Serial.print(" 1: ");
    Serial.print(SPI.transfer(0x00), BIN);
    Serial.print(" 0: ");
    Serial.println(SPI.transfer(0x00), BIN);
/*
    SPI.transfer(WriteControlByte0);
    SPI.transfer(Gain1);  //Set for Gain of 1: B00000001

    SPI.transfer(WriteControlByte1);  //B0011001
    SPI.transfer(C1Filter200);  
    SPI.transfer(WriteControlByte2);  //B0001100;
    SPI.transfer(C2Filter200);
*/
#else

    //--------------------------------------------------------------------------------
    //Setup ADC to defined values
    //Gain = 1
    //Notch Frequency/Nyquist Sampling Rate = 100Hz
    SPI.transfer(ReadControlByte0);
    temp = SPI.transfer(0x00);

    if (temp == Gain1){}   //Check the value of Control Byte 0
    else{
        SPI.transfer(WriteControlByte0);
        SPI.transfer(Gain1);  //Set for Gain of 1: B00000001
    }    

    //Filter Setting
    SPI.transfer(ReadControlByte1);
    temp = SPI.transfer(0x00);
    if (temp == C1Filter200){}   //Check the value of Control Byte 1
    else{
        SPI.transfer(WriteControlByte1);  //B0011001
        SPI.transfer(C1Filter200);  
        SPI.transfer(WriteControlByte2);  //B0001100;
        SPI.transfer(C2Filter200);
    }  

#endif
    //deselect the chip
    digitalWrite(ACDH_ADC_XSS_PIN, HIGH);  


    //---------------------------------------------------------------------------------
    digitalWrite(ACDH_ADC_YSS_PIN,LOW);
    //Setup ADC to defined values
    //Gain = 1
    //Notch Frequency/Nyquist Sampling Rate = 100Hz
    SPI.transfer(ReadControlByte0);
    temp = SPI.transfer(0x00);
    if (temp == Gain1){}   //Check the value of Control Byte 0
    else{
        SPI.transfer(WriteControlByte0);
        SPI.transfer(Gain1);  //Set for Gain of 1: B00000001
    }    
    //Filter Setting
    SPI.transfer(ReadControlByte1);
    temp = SPI.transfer(0x00);
    if (temp == C1Filter200){}   //Check the value of Control Byte 1
    else{
        SPI.transfer(WriteControlByte1);  //B0011001
        SPI.transfer(C1Filter200);  
        SPI.transfer(WriteControlByte2);  //B0001100;
        SPI.transfer(C2Filter200);
    }
    //deselect the chip
    digitalWrite(ACDH_ADC_YSS_PIN, HIGH); 
  
  
    //-------------------------------------------------------------------------------
    digitalWrite(ACDH_ADC_ZSS_PIN,LOW);
    //-----------------------------------------------------------------------------
    //Setup ADC to defined values
    //Gain = 1
    //Notch Frequency/Nyquist Sampling Rate = 100Hz
    SPI.transfer(ReadControlByte0);
    temp = SPI.transfer(0x00);
    if (temp == Gain1){}   //Check the value of Control Byte 0
    else{
        SPI.transfer(WriteControlByte0);
        SPI.transfer(Gain1);  //Set for Gain of 1: B00000001
    }    
    //Filter Setting
    SPI.transfer(ReadControlByte1);
    temp = SPI.transfer(0x00);
    if (temp == C1Filter200){}   //Check the value of Control Byte 1
    else{
        SPI.transfer(WriteControlByte1);  //B0011001
        SPI.transfer(C1Filter200);  
        SPI.transfer(WriteControlByte2);  //B0001100;
        SPI.transfer(C2Filter200);
    }
  
    digitalWrite(ACDH_ADC_XSS_PIN,LOW);
    SPI.transfer(WriteControlByte1);  //B0011001
    SPI.transfer(conversionStart);  
    digitalWrite(ACDH_ADC_ZSS_PIN,HIGH);


    digitalWrite(ACDH_ADC_XSS_PIN,HIGH);

    //deselect the chip
    digitalWrite(ACDH_ADC_XSS_PIN, HIGH); 
    digitalWrite(ACDH_ADC_YSS_PIN, HIGH);
    digitalWrite(ACDH_ADC_ZSS_PIN, HIGH);



    digitalWrite(ACDH_ADC_SYNC_PIN,HIGH);
    delay(10); //10 millisecond pulse (minimum is 100ns)
    digitalWrite(ACDH_ADC_SYNC_PIN,LOW);
    delay(10); //10 millisecond pulse (minimum is 100ns)
    digitalWrite(ACDH_ADC_SYNC_PIN,HIGH);
  
    delay(500); //allow the ADC to settle
}

//Used to read Magnetometer data from the ADC
unsigned int acdh_adc_readData(int ChipSelect, int SyncPin, int ResetPin){ 
    //Works by calling for the 3 bytes of data (3x8bytes=24bits)
    //and then combining them and returning the result
    //set local variables
    unsigned int result=0;
    byte byte0=0, byte1=0, byte2=0;
    byte temp = 0;
    //select the approripate ADC
    digitalWrite(ChipSelect,LOW);
/*
    SPI.transfer(ReadControlByte1);
    temp = SPI.transfer(0x00);
    Serial.print("control byte1: ");
    Serial.println(temp, BIN);
*/
    SPI.transfer(DataByte0);    //Call the register that we want to read and store data
    byte0 = SPI.transfer(0x00); 
    SPI.transfer(DataByte1);
    byte1 = SPI.transfer(0x00);
    SPI.transfer(DataByte2); 
    byte2 = SPI.transfer(0x00);


  
    //shift the data appropirately and combine the values
    //the | is an or bitwise operator. so all values are stored
    //within result
    result = byte2 << 16 | byte1 << 8 | byte0 ; 
  
    //deselect chip and return the result
    digitalWrite(ChipSelect,HIGH); 
    return result;
}

//////////////////Digital Potentiometer Items -------------------------------------

void acdh_digipot_setup(byte Address){
    Wire.beginTransmission(Address); //send start Command
    Wire.write(CommandTapAB);  //Alter both wipers
    Wire.write(128);                 //Set wipers to middle value
    Wire.endTransmission();
}

void acdh_digipot_gainset(byte Address, int Gain){
    /*
      Gain Table for ISL28533/ISL28633
      G1 | G0 | Gain
      0  | 0  | 1
      0  | z  | 2
      0  | 1  | 4
      z  | 0  | 5
      z  | z  | 10
      z  | 1  | 20
      1  | 0  | 40
      1  | z  | 50
      1  | 1  | 100
  
      *Note: Wipper A is connected to G0 and B to G1
      */
    Wire.beginTransmission(Address);
    if (Gain == 1){
        Wire.write(CommandTapA);
        Wire.write(0);
        Wire.write(CommandTapB);
        Wire.write(0); 
    }
    if (Gain == 2){
        Wire.write(CommandTapA);
        Wire.write(128);
        Wire.write(CommandTapB);
        Wire.write(0); 
    }
    if (Gain == 4){
        Wire.write(CommandTapA);
        Wire.write(255);
        Wire.write(CommandTapB);
        Wire.write(0); 
    }
    if (Gain == 5){
        Wire.write(CommandTapA);
        Wire.write(0);
        Wire.write(CommandTapB);
        Wire.write(128); 
    }if (Gain == 10){
        Wire.write(CommandTapA);
        Wire.write(128);
        Wire.write(CommandTapB);
        Wire.write(128); 
    }
    if (Gain == 20){
        Wire.write(CommandTapA);
        Wire.write(255);
        Wire.write(CommandTapB);
        Wire.write(128); 
    }
    if (Gain == 40){
        Wire.write(CommandTapA);
        Wire.write(0);
        Wire.write(CommandTapB);
        Wire.write(255); 
    }
    if (Gain == 50){
        Wire.write(CommandTapA);
        Wire.write(128);
        Wire.write(CommandTapB);
        Wire.write(255); 
    }
    if (Gain == 100){
        Wire.write(CommandTapA);
        Wire.write(255);
        Wire.write(CommandTapB);
        Wire.write(255); 
    }
    Wire.endTransmission();
}



// //////////////////////
// ///// BOARD LEDs /////
// //////////////////////

// Turn on LEDs for the different mission modes
void acdh_led_set(int mode){
    
    // Led for general Waiting
    if ( mode == 1 ) {
        digitalWrite(12, LOW);
        digitalWrite(13, LOW);
        digitalWrite(77, LOW);
        digitalWrite(78, HIGH);
        digitalWrite(79, LOW);
    }
	
    // Led for Science Mode
    if ( mode == 2 ) {
        digitalWrite(12, LOW);
        digitalWrite(13, LOW);
        digitalWrite(77, HIGH);
        digitalWrite(78, LOW);
        digitalWrite(79, LOW);
    }
    
    // Led for waiting to send transmission 
    if ( mode == 3 ) {
        digitalWrite(12, LOW);
        digitalWrite(13, HIGH);
        digitalWrite(77, HIGH);
        digitalWrite(78, LOW);
        digitalWrite(79, LOW);
    }
	
    // Led for Transmission Mode
    if ( mode == 4 ) {
        digitalWrite(12, LOW);
        digitalWrite(13, HIGH);
        digitalWrite(77, LOW);
        digitalWrite(78, LOW);
        digitalWrite(79, LOW);
    }
}
