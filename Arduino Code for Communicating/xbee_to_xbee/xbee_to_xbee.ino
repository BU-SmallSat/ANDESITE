#include <SoftwareSerial.h>

SoftwareSerial nss(2, 3);

void setup() {
  Serial.begin(9600);
  nss.begin(9600);
}

void loop() {
  if(Serial.available())
    nss.write(Serial.read());
  if(nss.available())
    Serial.write(nss.read());
}
