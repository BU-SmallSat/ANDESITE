#include <SPI.h>
#include <SdFat.h>

#define VREF 2.61
#define MAX_INT 8388607

//DEFINE COMMANDS TO ADC
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

//DEFINE COMMON REGISTER WRITES TO ADC
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

//DEFINE ADC REGISTERS
#define MUX0        0x00
#define VBIAS       0x01
#define MUX1        0x02
#define SYS0        0x03

//DEFINE ADC PINS        
#define CS_PIN      5
#define START_PIN   6
#define DRDY_PIN    7

#define DEGAUSS_PIN 9

#define SERIAL_BAUD 115200
#define SD_PIN      8


SdFat SD;
SPISettings SPIsettings(4000000, MSBFIRST, SPI_MODE1); // initialize SPI with 4Mhz clock, MSB first, SPI Mode1
long readSample(){
  digitalWrite(CS_PIN, LOW);
  delay(1);
  SPI.beginTransaction(SPIsettings); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
  byte byteOne = SPI.transfer(NOP);
  //Serial.println(byteOne);
  byte byteTwo = SPI.transfer(NOP);
  //Serial.println(byteTwo);
  byte byteThree = SPI.transfer(NOP);
  //Serial.println(byteThree);
  SPI.endTransaction();
  delay(5); 
  digitalWrite(CS_PIN, HIGH);
  byte signByte = 0;
  if((byteOne & 0x80) == 0x80){
    byte signByte = 0xFF;    //sign extend with ones if MSB of data is 1;
  }
  long magData = (signByte << 24) | (byteOne << 16) | (byteTwo << 8) | byteThree;
  return magData;
}

byte readRegister(byte regAddr) {
    digitalWrite(CS_PIN,LOW);
    delay(1);
    SPI.beginTransaction(SPIsettings); 
    SPI.transfer(RREG | regAddr);   //read from register at given address
    SPI.transfer(0x00);              //read from one register
    byte value = SPI.transfer(0x00);   //store value read from register   
    SPI.endTransaction();
    delay(1);
    digitalWrite(CS_PIN, HIGH);
    Serial.print("Read 0x");
    Serial.print(value, HEX);
    Serial.print(" from register 0x");
    Serial.println(regAddr, HEX);
    return value; 
}

void sendCommand(byte command){
    digitalWrite(CS_PIN,LOW);
    SPI.beginTransaction(SPIsettings);
    delay(1);
    SPI.transfer(command); //Issue command
    delay(1);
    SPI.endTransaction();
    digitalWrite(CS_PIN, HIGH);
}

void writeRegister(byte regAddr, byte value){
    digitalWrite(CS_PIN,LOW);
    delay(1);
    SPI.beginTransaction(SPIsettings); 
    SPI.transfer(WREG | regAddr);   //write to register at given address
    SPI.transfer(0x00);             //only write to one register   
    SPI.transfer(value);             //write contents of value to the register
    SPI.endTransaction();
    delay(1);
    digitalWrite(CS_PIN, HIGH);
    Serial.print("Wrote 0x");
    Serial.print(value, HEX);
    Serial.print(" to register 0x");
    Serial.println(regAddr, HEX); 
}

void setup(){
    //while(!Serial.available()){}
    // Set baud rate
    Serial.begin(SERIAL_BAUD);
    pinMode(SD_PIN, OUTPUT);

    //pinMode(DEGAUSS_PIN, OUTPUT);
    //digitalWrite(DEGAUSS_PIN, LOW);
    
    digitalWrite(SD_PIN, LOW);
    // Setup SD card
    if ( !SD.begin(SD_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
    }
    if(SD.exists("XoutByte.txt")){
      SD.remove("XoutByte.txt");
    }

    digitalWrite(SD_PIN, HIGH);

    //First set the correct pinmodes 
    pinMode(CS_PIN, OUTPUT);
    pinMode(START_PIN, OUTPUT);         // set START pin as Output
    pinMode(DRDY_PIN, INPUT);           // DRDY read
       
    digitalWrite(CS_PIN, HIGH);
    digitalWrite(START_PIN, HIGH);      // HIGH = Start Convert Continuously


    //---------------------------------------------------------------------------------
    // start the SPI library:
    SPI.begin();


///*
    sendCommand(RESET);
    delayMicroseconds(1);
    sendCommand(SDATAC);
    delayMicroseconds(1);

//*/

/*
    SPI.beginTransaction(SPIsettings);
    digitalWrite(CS_PIN,LOW);
    delay(1);
    SPI.transfer(RESET); //Reset the ADS1247
    delay(2); //Minimum 0.6ms required for Reset to finish.
    SPI.transfer(SDATAC); //Issue SDATAC
    delay(1);
    digitalWrite(CS_PIN, HIGH);
    SPI.endTransaction();
    */

    //writing to the MUX0 register -- ONLY UNCOMMENT ONE OF THE FOLLOWING
    writeRegister(MUX0, YOUT);
    //writeRegister(MUX0, XDIFF);
    //writeRegister(MUX0, XOUT);
    //writeRegister(MUX0, YDIFF);
    //writeRegister(MUX0, ZOUT);

    //reading from MUX0 register
    readRegister(MUX0);
    

    //writing to the VBIAS register 
    writeRegister(VBIAS, 0x00);  //no bias voltage enabled

    //reading from VBIAS register
    readRegister(VBIAS);

    //writing to the MUX1 register
    writeRegister(MUX1, 0x08);   //no internal reference volate, instead reference voltage is supplied to REF1

    //reading from the MUX1 register
    readRegister(MUX1);

    //writing to the SYS0  -- ONLY UNCOMMENT ONE OF THE FOLLOWING
    writeRegister(SYS0, DATA_RATE1);    //5SPS
    //writeRegister(SYS0, DATA_RATE2);       //20SPS
    //writeRegister(SYS0, DATA_RATE3);       //40SPS
    //writeRegister(SYS0, DATA_RATE4);       //80SPS
    //writeRegister(SYS0, DATA_RATE5);       //160SPS
    //writeRegister(SYS0, DATA_RATE6);       //320SPS

    //reading from the SYS0 register
    readRegister(SYS0);

    sendCommand(RDATAC);
/*
    SPI.beginTransaction(SPISettings(4096000, MSBFIRST, SPI_MODE1)); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
    digitalWrite(CS_PIN,LOW);
    delay(1);
    SPI.transfer(RDATAC); //Issue RDATAC
    delay(1);
    digitalWrite(CS_PIN, HIGH);
    SPI.endTransaction();
    */
    
    //SPI.end();
}

File _handle;
double magVolts;
int magInt;
int count = 1;

void loop(){
 
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
      /*
      if(count == 25){
        Serial.println("NOW");
        digitalWrite(DEGAUSS_PIN, HIGH);
        delay(100);
        digitalWrite(DEGAUSS_PIN, LOW);
      }
      */

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
 }
