#include <SPI.h>
#include <SdFat.h>

#define VREF 2.51
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
    //setup the SPI library
    SPI.beginTransaction(SPISettings(4096000, MSBFIRST, SPI_MODE1)); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0
    
    digitalWrite(CS_PIN,LOW);
    delay(1);
    SPI.transfer(RESET); //Reset the ADS1247
    delay(2); //Minimum 0.6ms required for Reset to finish.
    SPI.transfer(SELFOCAL); //perfrom self offset calibration
    delay(5000); //minimum of 3,000 ms required
    SPI.transfer(SDATAC); //Issue SDATAC
    delay(1);


    //write to MUX0 register 
    SPI.transfer(WREG | MUX0);      //write to MUX0 register
    SPI.transfer(0x00);             //one write to one register 
    SPI.transfer(0x37);             //no burnout current, positive input from AIN6 and negative input from AIN7;
    delay(1);

    //write to VBIAS register - do not enable any bias voltage
    SPI.transfer(WREG | VBIAS);     //write to VBIAS register  
    SPI.transfer(0x00);             //only writing to one register   
    SPI.transfer(0x00);             //no bias voltage enabled
    delay(1);

    //write to MUX1 register
    SPI.transfer(WREG | MUX1);      //write to MUX1 register
    SPI.transfer(0x00);             //onyl write to one register   
    SPI.transfer(0x08);             //no internal reference volate, instead reference voltage is supplied to REF1
    delay(1);

    SPI.transfer(WREG | SYS0);      //write to SYS0 register
    SPI.transfer(0x00);             //only write to one register
    SPI.transfer(0x00);             //gain of 1, and a DATA RATE OF 5SPS
    //SPI.transfer(0x02);              //gain of 1, and DATA RATE OF 20SPS
    //SPI.transfer(0x03);              //gain of 1, and DATA RATE OF 40SPS
    //SPI.transfer(0x04);              //gain of 1, and DATA RATE OF 80SPS
    //SPI.transfer(0x05);              //gain of 1, and DATA RATE OF 160SPS
    //SPI.transfer(0x06);              //gain of 1, and DATA RATE OF 320SPS
    delay(1);
    SPI.transfer(RDATAC); //Issue RDATAC
    delay(1);
    digitalWrite(CS_PIN, HIGH);
    delay(1);
}

File _handle;
double magVolts;
double magInt;
int count = 1;

void loop(){
    //Serial.println(VREF);
    unsigned long magData = 0;
    unsigned long byteOne = 0;
    unsigned long byteTwo = 0;
    unsigned long byteThree = 0;
    digitalWrite(CS_PIN, LOW);
    while(digitalRead(DRDY_PIN) != LOW){}
    if(count < 20){
      //count++;
      byteOne = SPI.transfer(NOP);
      //Serial.println(byteOne, BIN);
      byteTwo = SPI.transfer(NOP);
      //Serial.println(byteTwo, BIN);
      byteThree = SPI.transfer(NOP);
      //Serial.println(byteThree, BIN);
      magData |= byteOne;
      magData <<= 8;
      magData |= byteTwo;
      magData <<= 8;
      magData |= byteThree;
      delay(1);
      digitalWrite(CS_PIN, HIGH);
      //magData <<= 8;
      //magData |= SPI.transfer(NOP);
      /*
      if(count == 25){
        Serial.println("NOW");
        digitalWrite(DEGAUSS_PIN, HIGH);
        delay(100);
        digitalWrite(DEGAUSS_PIN, LOW);
      }
      */
      //Serial.println(magData, BIN);
      //Serial.println(magData);
      if(magData > MAX_INT){
       magInt = -((~magData)+1);
       //Serial.print("negative number");
      }
      else{
        magInt = magData;
      }
      //Serial.println(magInt);
      //Serial.println();
      //Serial.println(magInt/MAX_INT);
      magVolts = (magInt/MAX_INT)*VREF;
      digitalWrite(SD_PIN, LOW);
      _handle = SD.open("MAG_TEST.txt", FILE_WRITE);
      if(SD.exists("MAG_TEST.txt")){
        _handle.println(magVolts);
        _handle.close();
      }
      Serial.println(magVolts, 6);
      digitalWrite(SD_PIN, HIGH);
   }
 }
