#include "MC33926_Arduino.h"

#define D1 6
#define INV 3
#define FB A0
#define notSF 5
#define EN 4
#define Desired_Current 2600

MC33926_Arduino mc(D1,INV,FB,notSF,EN);

void stopIfFault()
{
  if (mc.getStatusFlag())
  {
    Serial.println("FAULT!");
//    while(1);
  }
}

String message = "";
int PowerTime = 0;

void setup()
{
  delay(500);
  Serial.begin(115200);
  delay(500);
  Serial.flush();
  delay(500);
  Serial.println("WE BEGIN THE SETUP");
  while(message != "1:initSuccess")
  {
    if(Serial.available() > 0)
    {
      message = Serial.readString();
      Serial.println("We read message");
      Serial.println(message);
      //Serial.print(_Mule.message);
    }
    delay(100);
  }
  Serial.println("1:initSuccess");
  mc.init();
  mc.setSpeed(0);
}

void loop()
{
  Serial.println("WE BEGIN THE LOOP");
  while (message[0] != '1'){
    message = Serial.readString();
  }
  String timeString = message.substring(4);
  int timePower = 0;
  for(int i =0;i<4;i++){
   int placeHolder = timeString[i]-48;
   if(placeHolder > 0){
    timePower += placeHolder*(10^(3-i));
   }
  }
  delay(2000);
  int pin = 7;
  pinMode(pin, INPUT);
  int Isdeployed = digitalRead(pin);

  int Speed = 90;
  mc.setSpeed(Speed);
  unsigned long time1;
  time1 = millis();
  while ((millis()-time1 <= timePower*1000) && Isdeployed == 1)
  {
    float Average_Current = 0;
  
    for (int i = 0; i <= 200; i++)
    {
      stopIfFault();
      int Current = mc.getCurrent();
      Average_Current = Average_Current + Current;
      Isdeployed = digitalRead(pin);
      delay(5);
    }
    Isdeployed = digitalRead(pin);
 
    Average_Current = Average_Current / 201;
    if (Average_Current > Desired_Current + 10)//since 23 stands for 1/1024 of our range
    {
      Speed = Speed - 1; 
      mc.setSpeed(Speed);
    }
    else if (Average_Current < Desired_Current - 10 && Speed <= 99)
    {
      Speed = Speed + 1; 
      mc.setSpeed(Speed);
    }

  }
  if (Isdeployed == 1)
  {
      Isdeployed = 0;
      Serial.print("1:Failed");
  }
  else
  {
      Serial.print("1:Success");  
  }
  mc.setSpeed(0);
  message = "";
}
