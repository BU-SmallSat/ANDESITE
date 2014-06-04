#include <Time.h>
#include <XBeeInfo.h>
#include <TXRequest.h>
#include <Potentiometer.h>
#include <Common.h>

#include <SoftwareSerial.h>

//Input pin
const int analogPin = 0;
int potentiometerVal;
//Addresses of the xbee
//const int SH_ROUTER_ONE = 0x0013A200;
//const int SL_ROUTER_ONE = 0x409BB9E7;

SoftwareSerial out(2, 3);

long SH_COORD = 0x0013A200;
long SL_COORD = 0x407B6C7D;
long SH_ROUTER_ZERO = 0x0013A200;
long SL_ROUTER_ZERO = 0x4078EFB8;
long SH_ROUTER_ONE = 0x0013A200;
long SL_ROUTER_ONE = 0x409BB9E7;


int MY_COORD = 0x0;

//String timeStamp = "130620123118";
XBeeInfo coord(SH_COORD, SL_COORD, "Coord", MY_COORD);
Common com;
byte test[2];

bool begin = false;

void setup()
{
	Serial.begin(9600);
        out.begin(9600);
        analogReference(EXTERNAL);
}

void loop()
{
  if(!begin) {
    byte recieved[19];
    for(int i = 0; i < sizeof(recieved); i++) { 
      recieved[i] = out.read();
    }
    if(recieved[18] == 0x00) {
      begin = true;
    }
  }
  
  if(begin) {
    //Creates a potentiomeeter object with the value
    potentiometerVal = analogRead(analogPin);
    float val = potentiometerVal * 3.3 / 1024.0;
    Serial.println(val);
  
    String timeStamp = getTime();
    
    Potentiometer pot(potentiometerVal, timeStamp);

    //Creates the request
    TXRequest request(coord.getSH(), coord.getSL(), coord.getMY(), pot.getValue(), pot.getTimeStamp());

    //Creates the packet to send and fills in the frame
    int packetToSendLength = request.getTXLength();
    byte packetToSend[packetToSendLength];
    for (int i = 0; i < request.getTXLength(); i++) {
      packetToSend[i] = request.getTXRequest(i);
    }

    for(int x = 0; x < packetToSendLength; x++) {
      out.write(packetToSend[x]); // "out.write" sends the data to the other XBee
      /*for (int i = 0; i < 26; i++) {
	if(packetToSend[i] < 0x10) {
	  Serial.print('0');
	}
	Serial.print(packetToSend[i], HEX);
	Serial.print(" ");
      }
      Serial.println();*/
    }
  }
  delay(10);
}

String getTime() {
  time_t t;
 t  = now();
  
  String time = String((year(t) % 100), HEX);
  time += String(month(t), HEX);
  time += String(day(t), HEX);
  time += String(hour(t), HEX);
  time += String(minute(t), HEX);
  time += String(second(t), HEX);
  
  return time;
}

/*void printData(Potentiometer pot, XBeeInfo xBee, TXRequest request, byte packetToSend[])
{
	Serial.print(pot.getTimeStamp());
        Serial.print(" ");
        Serial.println(pot.getValue(), HEX);
        
	Serial.print(xBee.getSH(), HEX);
	Serial.print(" ");
	Serial.print(xBee.getSL(), HEX);
	Serial.print(" ");
	Serial.print(xBee.getMY(), HEX);
	Serial.print(" ");
	Serial.println(xBee.getNI());

	for (int i = 0; i < 26; i++) {
		if(packetToSend[i] < 0x10) {
			Serial.print('0');
		}
		Serial.print(packetToSend[i], HEX);
		Serial.print(" ");
	}
	Serial.println();
}*/
