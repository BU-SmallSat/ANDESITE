#include <SoftwareSerial.h>

SoftwareSerial out(2, 3);
int data;

void setup()
{
  Serial.begin(9600);
  out.begin(9600);
}

void loop()
{
  //if(Serial.available() > 21)
  /*if(Serial.available() == 0x7E)
  {
    for(int i = 0; i < 22; i++)
    {
      Serial.print(Serial.read(), HEX);
      Serial.print(", ");
    }
    Serial.println();
  }*/
  if(out.available() > 4){
    data = out.read();
    if(data == 0x7E)
    {
      Serial.print(data, HEX);
      Serial.print(", ");
      data = out.read();
      while(data != 0xFFFF)
      {
        Serial.print(data, HEX);
        Serial.print(", ");
        data = out.read();
      }
  //Serial.print(out.read(), HEX);
  Serial.println();}
  }
}
