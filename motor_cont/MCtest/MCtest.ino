#include "MC33926_Arduino.h"

#define D1 6
#define INV 3
#define FB A0
#define notSF 5
#define EN 4

MC33926_Arduino mc(D1,INV,FB,notSF,EN);
 
void stopIfFault()
{
  if (mc.getStatusFlag())
  {
    Serial.println("fault");
    //while(1);
  }
}
 
void setup()
{
  Serial.begin(115200);
  Serial.println("Starting MC33926 Arduino test");
  mc.init();
}
 
void loop()
{
  int i;
  i = 20;
  mc.setSpeed(20);
 
  for (int i = 10; i < 100; i++)
  {
    mc.setSpeed(i);
    Serial.print("M1 current: ");

    stopIfFault();
    delay(500);
  }

  /* 
  for (int i = 100; i >= -100; i--)
  {
    mc.setSpeed(i);
    stopIfFault();
    if (abs(i)%10 == 0)
    {
      Serial.print("M1 current: ");
      Serial.println(mc.getCurrent());
    }
    delay(2);
  }
   
  for (int i = -100; i <= 0; i++)
  {
    mc.setSpeed(i);
    stopIfFault();
    if (abs(i)%10 == 0)
    {
      Serial.print("M1 current: ");
      Serial.println(mc.getCurrent());
    }
    delay(2);
  }
  */
}
