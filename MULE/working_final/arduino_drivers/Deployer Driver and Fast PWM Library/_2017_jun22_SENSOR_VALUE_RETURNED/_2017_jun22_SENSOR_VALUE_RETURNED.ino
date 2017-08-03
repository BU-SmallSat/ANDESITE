#include "MC33926_Arduino_FAST_PWM.h"
#define D1 3 //originally 6, then 5
#define INV 4 //originally 3, then 6
#define FB A4 // IT IS A0 - THE SAME WTIH ONE OF SWITCH PINS!!! WATCH OUT!!!
#define notSF 8 //originally 5, then 8
#define EN 7 //originally 4, then 7
#define Desired_Current 2600 // The actual needed value
//#define Desired_Current 1300 // Lowered value introduced due to the weakness of the power supply
// IR Sensor pins are: analog 2, 3, 4, 5; Switch pins are 9, 10, 11, 12;
int IR_Threshold = 8000; // DIFFERENT FROM THE ACTUAL ONE. PREVIOUSLY WAS 800

int Start_the_Loop = 0; // variable idicatin 

MC33926_Arduino_FAST_PWM mc(D1,INV,FB,notSF,EN);
void stopIfFault()
{
  if (mc.getStatusFlag())
  {
    //    Serial.println("FAULT!");
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
//  Serial.println("WE BEGIN THE SETUP");
//  while(message != "1:initSuccess") // Originally is was supposed to be 1:initSuccess, then changed to just "z"
//  {
//    if(Serial.available() > 0)
//    {
//      message = Serial.readString();
//      Serial.print("We (Arduino) read message, and it is  ");
//      Serial.println(message);
//      if (message == "1:initSuccess") // Originally is was supposed to be 1:initSuccess, then changed to just "z"
//      {
//        Serial.println("The message is what we need");
//      }
//      else
//      {
//        Serial.print("The message is incorrect");
//      }
//      //Serial.print(_Mule.message);
//    }
//    delay(100);
//  }
//  Serial.println("1:initSuccess");
  mc.init();
  mc.setSpeed(0);
//  pinMode(LED_BUILTIN, OUTPUT); // FOR TESTS LED IS LIT WHEN THE CURRENT IS SUPPOSED TO BE ON
}



void loop()
{
//  delay(2000); // delay added 28.01
  Serial.println("WE BEGIN THE LOOP");
  while (message[0] != 't')
  {
    message = Serial.readString();
    Serial.print("The ignition message is   ");
    Serial.println(message);
  }

  String timeString = message.substring(6,10);
  int timePower = timeString.toInt();
  Serial.print("timePower="); // IMPORTNANT PRINT
  Serial.println(timePower); // IMPORTNANT PRINT
  String SpeedString = message.substring(11);
  int Speed = SpeedString.toInt();
  //  char ArduinoID_Char = message[2];
  int ArduinoID = message[2] - 48; // a really ugly way of converting a string to an int
  //  char NodeID_Char = message[4];
  int NodeID = message[4] - 48; // a really ugly way of converting a string to an int

  Serial.print("ArduinoID = ");
  Serial.println(ArduinoID);
  if (ArduinoID == 1)
  {
    delay(2000);
//    int Sensor_pin = NodeID + 1;
    int Sensor_pin = 0;
    int Switch_pin = 0;
    if (NodeID == 1)
    {
      Sensor_pin = A0; // used to be A0
      Switch_pin = 5;
    }
    else if (NodeID == 2)
    {
      Sensor_pin = A1; // used to be A2
      Switch_pin = 6;
    }
    else if (NodeID == 3)
    {
      Sensor_pin = A2; // used to be A3
      Switch_pin = 9;
    }
    else if (NodeID == 4)
    {
      Sensor_pin = A3; // used to be A6
      Switch_pin = 10;
      Serial.println("Our pin is A6");
    }
    else // bogus case implemented solely for investigation
    {
      Sensor_pin = 20; // some kind of pin that really works
      Serial.println("We use pin 20 instead");
    }
    Serial.print("Sensor_pin=");
    Serial.println(Sensor_pin);
//    pinMode(Sensor_pin, INPUT); // Was not in the IR sensor code found in the Web
//    int Switch_pin = NodeID + 9;
    pinMode(Switch_pin, OUTPUT);
    digitalWrite(Switch_pin, HIGH);
    if (Switch_pin != 5)
    {
      pinMode(5, OUTPUT);
      digitalWrite(5, LOW);
    }
    if (Switch_pin != 6)
    {
      pinMode(6, OUTPUT);
      digitalWrite(6, LOW);
    }
    if (Switch_pin != 9)
    {
      pinMode(9, OUTPUT);
      digitalWrite(9, LOW);
    }
    if (Switch_pin != 10)
    {
      pinMode(10, OUTPUT);
      digitalWrite(10, LOW);
    }
    
//    for (int k = 9; k <= 13; k++) // originally from 10 to 13, not from 9
//    {
//      if (k != Switch_pin)
//      {
//        pinMode(k, OUTPUT);
//        digitalWrite(k, LOW); //it should be LOW for a switch that we want to close
//      }
//    }
    //  Pseudo code for indicating whether the node is deployed or not
    //    int Isdeployed = digitalRead(Sensor_pin);
    //  //  Serial.print("Isdeployed = ");
    //  //  Serial.println(Isdeployed);
    //  Actual IR sensor code for determining whether the node is deployed or not
    int QRE_Value = analogRead(Sensor_pin);
    int Isdeployed = QRE_Value < IR_Threshold; // For some reason Isdeployed==1 when the node is not yet deployed, and vice versa
    Serial.print("IR Sensor reads: ");
    Serial.println(QRE_Value);
    Serial.print("Isdeployed = ");
    Serial.println(Isdeployed);
    
//    int Speed = 30; //WAS 90 ON THE ACTUAL CODE
    mc.setSpeed(Speed);
    unsigned long time1;
    time1 = millis();
    while ((millis()-time1 <= timePower*1000) && Isdeployed == 1)
    {
      float Average_Current = 0;
//      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      for (int i = 0; i <= 200; i++)
      {
        stopIfFault();
        int Current = mc.getCurrent();
        Average_Current = Average_Current + Current;
        //        Isdeployed = digitalRead(Sensor_pin);
        QRE_Value = analogRead(Sensor_pin);
        Isdeployed = QRE_Value < IR_Threshold;
        delay(5);
      }
      //      Isdeployed = digitalRead(Sensor_pin);
      QRE_Value = analogRead(Sensor_pin);
      Isdeployed = QRE_Value < IR_Threshold;
      
      
      Average_Current = Average_Current / 201;
            Serial.print("Average_Current = "); // IMPORTNANT PRINT
            Serial.print(Average_Current); // IMPORTNANT PRINT
            Serial.print(" For Speed = "); // IMPORTNANT PRINT
            Serial.println(Speed); // IMPORTNANT PRINT
      if (Average_Current > Desired_Current + 10)//since 23 stands for 1/1024 of our range
      {
        Speed = Speed - 0; // originally adjusted by 1
        mc.setSpeed(Speed);
      }
      else if (Average_Current < Desired_Current - 10 && Speed <= 99)
      {
        Speed = Speed + 0; // originally adjusted by 1
        mc.setSpeed(Speed);
      }
      
    }
    String QRE_String = String(QRE_Value);
    Serial.print("1:" + QRE_String);
    mc.setSpeed(0);
//    digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
//    message = "";
  }
  else if (ArduinoID == 3)
  {
    Serial.print("privyet");
  }
  else if (ArduinoID == 4) // THIS FUNCTION RETURNS THE VALUE OF THE IR SENSOR NUMBER 1
  {
    int QRE_Value1 = analogRead(A0);
    String QRE_String1 = String(QRE_Value1);
    Serial.println("QRE:" + QRE_String1);
  }
  else if (ArduinoID == 5) // THIS FUNCTION RETURNS THE VALUE OF THE IR SENSOR NUMBER 2
  {
    int QRE_Value2 = analogRead(A1);
    String QRE_String2 = String(QRE_Value2);
    Serial.println("QRE:" + QRE_String2);
  }
  else if (ArduinoID == 6) // THIS FUNCTION RETURNS THE VALUE OF THE IR SENSOR NUMBER 3
  {
    int QRE_Value3 = analogRead(A2);
    String QRE_String3 = String(QRE_Value3);
    Serial.println("QRE:" + QRE_String3);
  }
  else if (ArduinoID == 7) // THIS FUNCTION RETURNS THE VALUE OF THE IR SENSOR NUMBER 4
  {
    int QRE_Value4 = analogRead(A3);
    String QRE_String4 = String(QRE_Value4);
    Serial.println("QRE:" + QRE_String4);
  }
  else
  {
    Serial.println("CALL TO ANOTHER ARDUINO");
  }
  message = "";
}
