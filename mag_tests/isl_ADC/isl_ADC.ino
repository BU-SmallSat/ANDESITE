void setup() {
  Serial.begin(SERIAL_BAUD);
  pinMode(PDWN_PIN, OUTPUT);
  digitalWrite(PDWN_PIN, LOW);
  delay(5);
  digitalWrite(PDWN_PIN, HIGH);
  delay(50);
  pinMode(CS_PIN, OUTPUT);
  pinMode(RDY_PIN, OUTPUT);
  pinMode(SD_PIN, OUTPUT);
    
  digitalWrite(SD_PIN, LOW);
  // Setup SD card
  if ( !SD.begin(SD_PIN) ) {
      Serial.println("ERROR: SD card initialization failed.");
  }
  digitalWrite(SD_PIN, HIGH);
  // start the SPI library:
  SPI.begin();
  //setup the SPI library
  SPI.beginTransaction(SPISettings(4096000, MSBFIRST, SPI_MODE1)); // initialize SPI with 4Mhz clock, MSB first, SPI Mode0

  digitalWrite(CS_PIN,LOW);
  //setup register
  digitalWrite(CS_PIN, HIGH);
  delay(1);


  digitalWrite(CS_PIN,LOW);
  //setup register
  digitalWrite(CS_PIN, HIGH);
  delay(1);
  
}

void loop() {


}
