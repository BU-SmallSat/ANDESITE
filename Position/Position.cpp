#include "Arduino.h"
#include "Position.h"
#include <String.h>

void Position::sortInfo(String GPS)
{
	int firstcomma = GPS.indexOf(',');
	String type = GPS.substring(0, firstcomma);

	int secondcomma = GPS.indexOf(',', firstcomma + 1);
	String latitude = GPS.substring(firstcomma + 1, secondcomma);

	int thirdcomma = GPS.indexOf(',', secondcomma + 1);
	String NS = GPS.substring(secondcomma + 1, thirdcomma);

	int fourthcomma = GPS.indexOf(',', thirdcomma + 1);
	String longitude = GPS.substring(thirdcomma + 1, fourthcomma);

	int fifthcomma = GPS.indexOf(',', fourthcomma + 1);
	String EW = GPS.substring(fourthcomma + 1, fifthcomma);

	int sixthcomma = GPS.indexOf(',', fifthcomma + 1);
	String time = GPS.substring(fifthcomma + 1, sixthcomma);

	int seventhcomma = GPS.indexOf(',', sixthcomma + 1);
	String active = GPS.substring(sixthcomma + 1, seventhcomma);

	int eigthcomma = GPS.indexOf(',', seventhcomma + 1);
	String checksum = GPS.substring(seventhcomma + 1, eigthcomma);


	Serial.print(type);
	Serial.print(":");
	Serial.print(latitude);
	Serial.print(":");
	Serial.print(NS);
	Serial.print(":");
	Serial.print(longitude);
	Serial.print(":");
	Serial.print(EW);
	Serial.print(":");
	Serial.print(time);
	Serial.print(":");
	Serial.print(active);
	Serial.print(":");
	Serial.println(checksum);
	while (true);
}

