#include <SPI.h>
#include <SdFat.h>

#define VREF 2.61
#define MAX_INT 8388607

//DEFINE COMMANDS TO THE ADS_ADC
#define RESET       0x06
#define NOP         0xFF
#define RDATA       0x12
#define RDATAC      0x14
#define SDATAC      0x16
#define RREG        0x20
#define WREG        0x40
#define SYSOCAL     0x60
#define SYSGCAL     0x61
#define SELFOCAL    0x62

//DEFINE COMMON REGISTER WRITES TO ADS_ADC
#define YOUT        0x37    //no burnout current, positive input from AIN6 and negative input from AIN7 on U1
#define XDIFF       0x08    //no burnout current, positive input from AIN1 and negative input from AIN0 on U1
#define XOUT        0x25    //no burnout current, positive input from AIN4 and negative input from AIN5 on U1 
#define YDIFF       0x13    //no burnout current, positive input from AIN2 and negative input from AIN3 on U1
#define ZOUT        0x13    //no burnout current, positive input from AIN2 and negative input from AIN3 on U4  
#define DATA_RATE1  0x00    //gain of 1, and a DATA RATE OF 5SPS
#define DATA_RATE2  0x02    //gain of 1, and DATA RATE OF 20SPS
#define DATA_RATE3  0x03    //gain of 1, and DATA RATE OF 40SPS
#define DATA_RATE4  0x04    //gain of 1, and DATA RATE OF 80SPS
#define DATA_RATE5  0x05    //gain of 1, and DATA RATE OF 160SPS
#define DATA_RATE6  0x06    //gain of 1, and DATA RATE OF 320SPS

//DEFINE ISL_ADC REGISTERS
#define SDO         0x82
#define STANDBY     0x83
#define RATE        0x85
#define INMUX       0x87
#define CHANNEL     0x88
#define PGA         0x97
#define CONVERT     0x84

//DEFINE ADS_ADC REGISTERS
#define MUX0        0x00
#define VBIAS       0x01
#define MUX1        0x02
#define SYS0        0x03

//DEFINE ISL_ADC PINS    
#define SCLK_PIN    13    
#define ISL_CS_PIN      3
#define ISL_PDWN_PIN    5
#define ISL_RDY_PIN    12
#define DEGAUSS_PIN 6

//DEFINE ADS_ADC PINS        
#define ADS_CS_PIN      7
#define ADS_START_PIN  8
#define ADS_DRDY_PIN    9

#define SERIAL_BAUD 115200
#define SD_PIN      10


SdFat SD;
SPISettings ISL_writeRegisterSettings(1000000, MSBFIRST, SPI_MODE0);
SPISettings ISL_readRegisterSettings(1000000, MSBFIRST, SPI_MODE0);
SPISettings ISL_readSampleSettings(1000000, MSBFIRST, SPI_MODE1);
SPISettings ADS_settings(4000000, MSBFIRST, SPI_MODE1); // initialize SPI with 4Mhz clock, MSB first, SPI Mode1
long ADS_readSample(){
  digitalWrite(ADS_CS_PIN, LOW);
  delay(1);
  SPI.beginTransaction(ADS_settings); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
  byte byteOne = SPI.transfer(NOP);
  //Serial.println(byteOne);
  byte byteTwo = SPI.transfer(NOP);
  //Serial.println(byteTwo);
  byte byteThree = SPI.transfer(NOP);
  //Serial.println(byteThree);
  SPI.endTransaction();
  delay(5); 
  digitalWrite(ADS_CS_PIN, HIGH);
  byte signByte = 0;
  if((byteOne & 0x80) == 0x80){
    byte signByte = 0xFF;    //sign extend with ones if MSB of data is 1;
  }
  long magData = (signByte << 24) | (byteOne << 16) | (byteTwo << 8) | byteThree;
  return magData;
}

byte ADS_readRegister(byte regAddr) {
    digitalWrite(ADS_CS_PIN,LOW);
    delay(1);
    SPI.beginTransaction(ADS_settings); 
    SPI.transfer(RREG | regAddr);   //read from register at given address
    SPI.transfer(0x00);              //read from one register
    byte value = SPI.transfer(0x00);   //store value read from register   
    SPI.endTransaction();
    delay(1);
    digitalWrite(ADS_CS_PIN, HIGH);
    Serial.print("Read 0x");
    Serial.print(value, HEX);
    Serial.print(" from register 0x");
    Serial.println(regAddr, HEX);
    return value; 
}

void ADS_sendCommand(byte command){
    digitalWrite(ADS_CS_PIN,LOW);
    SPI.beginTransaction(ADS_settings);
    delay(1);
    SPI.transfer(command); //Issue command
    delay(1);
    SPI.endTransaction();
    digitalWrite(ADS_CS_PIN, HIGH);
}

void ADS_writeRegister(byte regAddr, byte value){
    digitalWrite(ADS_CS_PIN,LOW);
    delay(1);
    SPI.beginTransaction(ADS_settings); 
    SPI.transfer(WREG | regAddr);   //write to register at given address
    SPI.transfer(0x00);             //only write to one register   
    SPI.transfer(value);             //write contents of value to the register
    SPI.endTransaction();
    delay(1);
    digitalWrite(ADS_CS_PIN, HIGH);
    Serial.print("Wrote 0x");
    Serial.print(value, HEX);
    Serial.print(" to register 0x");
    Serial.println(regAddr, HEX); 
}


long ISL_readSample() {
  SPI.beginTransaction(ISL_readSampleSettings);

  byte high = SPI.transfer(0x00);
  byte middle = SPI.transfer(0x00);
  byte low = SPI.transfer(0x00);
  
  SPI.endTransaction();

  byte highest = 0;
  if ((high & 0x80) == 0x80) highest = 0xFF;
  long retVal = (highest << 24) | (high << 16) | (middle << 8) | low;
  Serial.print("Read sample ");
  Serial.println(retVal, DEC);

  return retVal;
}

byte ISL_readRegister(byte regAddr) {
  digitalWrite(ISL_CS_PIN, LOW);
  delayMicroseconds(1);
  SPI.beginTransaction(ISL_readRegisterSettings);

  SPI.transfer(regAddr & 0x7F); // Clear the high bit of the register address to indicate a read operation
  //SPI.endTransaction();
  //SPI.beginTransaction(readRegisterSettings);
  byte value = SPI.transfer(0x00);
  SPI.endTransaction();

  delayMicroseconds(5);
  digitalWrite(ISL_CS_PIN, HIGH);
  
  Serial.print("Read 0x");
  Serial.print(value, HEX);
  Serial.print(" from register 0x");
  Serial.println(regAddr, HEX);

  return value;
}

void ISL_writeRegister(byte regAddr, byte value) {
  digitalWrite(ISL_CS_PIN, LOW);
  delayMicroseconds(1);
  SPI.beginTransaction(ISL_writeRegisterSettings);

  SPI.transfer(regAddr | 0x80); // Set the high bit of the register address to indicate a write operation
  SPI.transfer(value);

  SPI.endTransaction();
  delayMicroseconds(5);
  digitalWrite(ISL_CS_PIN, HIGH);

  
  Serial.print("Wrote 0x");
  Serial.print(value, HEX);
  Serial.print(" to register 0x");
  Serial.println(regAddr, HEX); 
}


void setup(){

    pinMode(ISL_PDWN_PIN, OUTPUT);
    digitalWrite(ISL_PDWN_PIN, LOW);
    delay(5);
    digitalWrite(ISL_PDWN_PIN, HIGH);
    delay(20);
    
    //while(!Serial.available()){}
    // Set baud rate
    Serial.begin(SERIAL_BAUD);
    pinMode(SD_PIN, OUTPUT);
    digitalWrite(SD_PIN, HIGH);

    //pinMode(DEGAUSS_PIN, OUTPUT);
    //digitalWrite(DEGAUSS_PIN, LOW);


    ////////////////////////////////////////
    /////////////SETUP ISL ADC//////////////
    ////////////////////////////////////////

    pinMode(ISL_CS_PIN, OUTPUT);
    pinMode(ISL_RDY_PIN, INPUT);
    digitalWrite(ISL_CS_PIN, HIGH);  

    SPI.begin();

    ISL_readRegister(0x00);
  
    ISL_writeRegister(SDO, 0x00); // SDO ON, LSPS OFF
    ISL_readRegister(SDO);
  
    ISL_writeRegister(STANDBY, 0x00); // Disable standby mode
    ISL_readRegister(STANDBY);

    ISL_writeRegister(RATE, 0x01);  // Set the output sample rate to 5 Hz
    ISL_readRegister(RATE);

    ISL_writeRegister(INMUX, 0x01); // Set the input multiplexer to read channel AIN2
    ISL_readRegister(INMUX);

    ISL_writeRegister(CHANNEL, 0x01); // Set the channel pointer to Channel 2
    ISL_readRegister(CHANNEL);

    ISL_writeRegister(PGA, 0x00);
    ISL_readRegister(PGA);

    ISL_writeRegister(CONVERT, 0x02); // Perform continuous conversions
    ISL_readRegister(CONVERT);

    ////////////////////////////////////////
    /////////////SETUP ADS ADC//////////////
    ////////////////////////////////////////

    //First set the correct pinmodes 
    pinMode(ADS_CS_PIN, OUTPUT);
    pinMode(ADS_START_PIN, OUTPUT);         // set START pin as Output
    pinMode(ADS_DRDY_PIN, INPUT);           // DRDY read
       
    digitalWrite(ADS_CS_PIN, HIGH);
    digitalWrite(ADS_START_PIN, HIGH);      // HIGH = Start Convert Continuously

    ADS_sendCommand(RESET);
    delayMicroseconds(1);
    ADS_sendCommand(SDATAC);
    delayMicroseconds(1);

    //writing to the MUX0 register -- ONLY UNCOMMENT ONE OF THE FOLLOWING
    ADS_writeRegister(MUX0, YOUT);
    //ADS_writeRegister(MUX0, XDIFF);
    //ADS_writeRegister(MUX0, XOUT);
    //ADS_writeRegister(MUX0, YDIFF);
    //ADS_writeRegister(MUX0, ZOUT);

    //reading from MUX0 register
    ADS_readRegister(MUX0);
    

    //writing to the VBIAS register 
    ADS_writeRegister(VBIAS, 0x00);  //no bias voltage enabled

    //reading from VBIAS register
    ADS_readRegister(VBIAS);

    //writing to the MUX1 register
    ADS_writeRegister(MUX1, 0x08);   //no internal reference volate, instead reference voltage is supplied to REF1

    //reading from the MUX1 register
    ADS_readRegister(MUX1);

    //writing to the SYS0  -- ONLY UNCOMMENT ONE OF THE FOLLOWING
    ADS_writeRegister(SYS0, DATA_RATE1);    //5SPS
    //ADS_writeRegister(SYS0, DATA_RATE2);       //20SPS
    //ADS_writeRegister(SYS0, DATA_RATE3);       //40SPS
    //ADS_writeRegister(SYS0, DATA_RATE4);       //80SPS
    //ADS_writeRegister(SYS0, DATA_RATE5);       //160SPS
    //ADS_writeRegister(SYS0, DATA_RATE6);       //320SPS

    //reading from the SYS0 register
    ADS_readRegister(SYS0);

    ADS_sendCommand(RDATAC);


    ///////////////////////////////////////////
    ////////////SETUP THE SD CARD//////////////
    ///////////////////////////////////////////
    digitalWrite(SD_PIN, LOW);
    // Setup SD card
    if ( !SD.begin(SD_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
    }
    digitalWrite(SD_PIN, HIGH);
}

File _handle;
double magVolts;
int magInt;
int count = 1;

void loop(){
 /*
    digitalWrite(CS_PIN, LOW);
    //Serial.println("here");
    while(digitalRead(DRDY_PIN) != LOW){}
    //Serial.println("here");
    if(count < 20){
      //Serial.println("here");
      //count++;
      //SPI.begin();
      long magData = readSample();
      //SPI.end();
      if(count == 25){
        Serial.println("NOW");
        digitalWrite(DEGAUSS_PIN, HIGH);
        delay(100);
        digitalWrite(DEGAUSS_PIN, LOW);
      }

      //Serial.println(magData);
      magVolts = ((double)magData/(double)MAX_INT)*VREF;
      Serial.println(magVolts, 6);
      digitalWrite(SD_PIN, LOW);
      _handle = SD.open("XoutByte.txt", FILE_WRITE);
      if(SD.exists("XoutByte.txt")){
        _handle.println(magVolts, 6);
        _handle.close();
      }
      digitalWrite(SD_PIN, HIGH);
      //SPI.end();
    }
    */
 }
