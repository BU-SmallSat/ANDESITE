#include <SPI.h>
//#include <SdFat.h>

/*
This is a test for the ISL26104 ADC at a possible 5,10,20,40,80,160HZ data rates.
It is currently configured for the y-axis of the magnetometer, but can be configured for any axis
Use with Write Data Python Script
*/

#define VREF 2.51
#define MAX_INT 8388607

//DEFINE ADC REGISTERS
#define SDO         0x82
#define STANDBY     0x83
#define RATE        0x85
#define INMUX       0x87
#define CHANNEL     0x88
#define PGA         0x97
#define CONVERT     0x84

//DEFINE ADC PINS    
#define SCLK_PIN    13    
#define CS_PIN      5
#define PDWN_PIN    6
#define RDY_PIN    12
#define DEGAUSS_PIN 9

#define SERIAL_BAUD 115200
#define SD_PIN      8

/*
Function definitions for writing to registers, reading from registers, and sampling the ADC
*/
SPISettings writeRegisterSettings(1000000, MSBFIRST, SPI_MODE0);
SPISettings readRegisterSettings(1000000, MSBFIRST, SPI_MODE0);
SPISettings readSampleSettings(1000000, MSBFIRST, SPI_MODE1);

/*
This is the function that samples the ADC, it reads the SPI bus and stores the output in the 3 bytes
It then concatenates the bytes and returns the 24bit value
*/
long readSample() {
  SPI.beginTransaction(readSampleSettings);

  byte high = SPI.transfer(0x00);
  byte middle = SPI.transfer(0x00);
  byte low = SPI.transfer(0x00);
  
  SPI.endTransaction();

  byte highest = 0;
  if ((high & 0x80) == 0x80) highest = 0xFF;
  long retVal = (highest << 24) | (high << 16) | (middle << 8) | low;
  //Serial.print("Read sample ");
  //Serial.println(retVal, DEC);

  return retVal;
}

/*
This function reads from a designated register on the ADC
*/
byte readRegister(byte regAddr) {
  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(1);
  SPI.beginTransaction(readRegisterSettings);


  //define the register that you want to read from 
  SPI.transfer(regAddr & 0x7F); // Clear the high bit of the register address to indicate a read operation
  //SPI.endTransaction();
  //SPI.beginTransaction(readRegisterSettings);
  
  //read in contents of register and store in value
  byte value = SPI.transfer(0x00);
  SPI.endTransaction();

  delayMicroseconds(5);
  digitalWrite(CS_PIN, HIGH);
  /*
  Serial.print("Read 0x");
  Serial.print(value, HEX);
  Serial.print(" from register 0x");
  Serial.println(regAddr, HEX);
*/
  return value;
}

/*
this function writes to a designated register on the ADC
*/
void writeRegister(byte regAddr, byte value) {
  digitalWrite(CS_PIN, LOW);
  delayMicroseconds(1);
  SPI.beginTransaction(writeRegisterSettings);

  //define the register that you want to write to and then transfer the value
  SPI.transfer(regAddr | 0x80); // Set the high bit of the register address to indicate a write operation
  SPI.transfer(value);

  SPI.endTransaction();
  delayMicroseconds(5);
  digitalWrite(CS_PIN, HIGH);

  /*
  Serial.print("Wrote 0x");
  Serial.print(value, HEX);
  Serial.print(" to register 0x");
  Serial.println(regAddr, HEX); 
*/
}



void setup() {

//Power cycle the ADC
  pinMode(PDWN_PIN, OUTPUT);
  digitalWrite(PDWN_PIN, LOW);
  delay(5);
  Serial.begin(SERIAL_BAUD);
  /*
  digitalWrite(SD_PIN, LOW);
  // Setup SD card
  if ( !SD.begin(SD_PIN) ) {
      Serial.println("ERROR: SD card initialization failed.");
      while(1){
        delay(1000);
      }
  }
  */
  digitalWrite(PDWN_PIN, HIGH);
  delay(20);

  //configure arduino pins
  pinMode(CS_PIN, OUTPUT);
  pinMode(RDY_PIN, INPUT);
  pinMode(SD_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  

  digitalWrite(SD_PIN, HIGH);
  // start the SPI library:
  SPI.begin();

  Serial.println();
  Serial.println();
  //Serial.println("isl_ADC test beginning");
  readRegister(0x00);
  
  writeRegister(SDO, 0x00); // SDO ON, LSPS OFF
  readRegister(SDO);
  
  writeRegister(STANDBY, 0x00); // Disable standby mode
  readRegister(STANDBY);

//Select the data rate
  //writeRegister(RATE, 0x01);  // Set the output sample rate to 5 Hz
  //writeRegister(RATE, 0x02);  // Set the output sample rate to 10 Hz
  //writeRegister(RATE, 0x03);  // Set the output sample rate to 20 Hz
  //writeRegister(RATE, 0x04);   // Set the output sample rate to 40 Hz
  //writeRegister(RATE, 0x05);   // Set the output sample rate to 80 Hz
  writeRegister(RATE, 0x06);  // Set the output sample rate to 160 Hz
  //writeRegister(RATE, 0x01);  // Set the output sample rate to 5 Hz
  readRegister(RATE);

  writeRegister(INMUX, 0x01); // Set the input multiplexer to read channel AIN2
  readRegister(INMUX);

  writeRegister(CHANNEL, 0x01); // Set the channel pointer to Channel 2
  readRegister(CHANNEL);

  writeRegister(PGA, 0x00);
  readRegister(PGA);

  writeRegister(CONVERT, 0x02); // Perform continuous conversions
  readRegister(CONVERT);
}

//FILE _handle;
int count = 0;
double magInt;
double magVolts;

void loop() {
  // Select the ADC device to wait for a new sample to be available
  digitalWrite(CS_PIN, HIGH);
  // Wait for a High to Low pulse, which signals that a sample is ready to be read
  pulseIn(RDY_PIN, HIGH);
  long sample = readSample();
  // De-select the ADC device
  digitalWrite(CS_PIN, LOW);

  // Calculate the voltage
  magVolts = ((double)sample/(double)MAX_INT)*VREF;
  /*
  // Select the SD device
  digitalWrite(SD_PIN, LOW);
  _handle = SD.open("isl_TEST.txt", FILE_WRITE);
  if(SD.exists("isl_TEST.txt")){
    _handle.println(magVolts);
    _handle.close();
  }
  // De-select the SD device
  digitalWrite(SD_PIN, HIGH);
*/
  Serial.print(sample);
  Serial.print(" ");
  Serial.println(0);
  return;
}
