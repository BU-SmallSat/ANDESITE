#ifndef DS3231RTC_h
 #define DS3231RTC_h
 
#include <Time.h>
 
// library interface description
 class DS3231
 {
 // user-accessible "public" interface
 public:
 DS3231();
 static time_t get();
 static void set(time_t t);
 static void read(tmElements_t &tm);
 static void write(tmElements_t &tm);
 float getTemp();
 
private:
 static uint8_t dec2bcd(uint8_t num);
 static uint8_t bcd2dec(uint8_t num);
 };
 
extern DS3231 RTC;
 
#endif



