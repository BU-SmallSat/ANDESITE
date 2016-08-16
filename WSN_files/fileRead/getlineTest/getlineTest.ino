#include <SPI.h>

#include <SdFat.h>
SdFat SD;
//File handle;
#define ACDH_SD_PIN               9
void setup() {
  Serial.begin(115200);
    // Setup SD card
    if ( !SD.begin(ACDH_SD_PIN) ) {
        Serial.println("ERROR: SD card initialization failed.");
    }
    else{
      Serial.println("SD success");
    }

}
  const int line_buffer_size = 5;
  char buffer[line_buffer_size];

  int line_number = 0;
  int _position = 0;
void loop() { 
 ifstream sdin("001.txt");
 ifstream::pos_type pos = sdin.tellg();
 Serial.println(pos);
 _position = (long)pos;
 Serial.println(_position);
 pos++;
 sdin.seekg(pos);
 while (sdin.getline(buffer, line_buffer_size, '\n') || sdin.gcount()) {
    int count = sdin.gcount();
    if (sdin.fail()) {
      Serial.println("Partial long line");
      sdin.clear(sdin.rdstate() & ~ios_base::failbit);
    } else if (sdin.eof()) {
      Serial.println("Partial final line");  // sdin.fail() is false
    } else {
      count--;  // Don’t include newline in count
      Serial.print("Line ");
      Serial.print(++line_number);
    }
    Serial.print(" (");
    Serial.print(count);
    Serial.print(" chars): ");
    Serial.println(buffer);
 } 
 while(1){}
}
