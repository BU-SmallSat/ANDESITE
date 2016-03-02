#include "MC33926_Arduino.h"

#define D1 6
#define INV 3
#define FB A0
#define notSF 5
#define EN 4

MC33926_Arduino md(D1,INV,FB,notSF,EN);
int timeout = 15000;
unsigned long start=0;
bool deployment=false;
//float thres=1.5;
float thres=1;
float k=.5;
float error;
int speedOut=99;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Dual MC33926 Motor Shield");
  md.init();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  float currentLevel=0;
  
  //if(Serial.available())//uart stuff TBD
  //{
      
      start=millis();
      
      while(millis()-start<timeout)
      {
        delay(100);
        currentLevel=md.getCurrent();
        error=abs(currentLevel-thres);
        speedOut=speedOut+k*error;
        //if(millis()%2)
        {
        Serial.print(millis());
        md.setSpeed(speedOut);
        
        Serial.print(", ");
        Serial.println(currentLevel,5);  
        }      
        if(currentLevel>thres)
        {
            deployment=true;
            break;
        }
        
      }
      if(millis()-start<timeout)
      {
            deployment=false;
      }
      
  //}

}
