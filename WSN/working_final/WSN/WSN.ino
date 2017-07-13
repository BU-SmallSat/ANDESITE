
void setup() {
	Serial.begin(115200);
	pinMode(13, OUTPUT);
	digitalWrite(13,HIGH);
	Serial.println("Not the com board either");
}

void loop() {
	digitalWrite(13,!digitalRead(13));
	delay(1000);
	Serial.println("blink");
}
