
#define MAGADDR 0x10

#include <SoftWire.h>

#include <Wire.h>

#define maxRspLen 350 //max response form EPS length
#define maxMsgLen 25 //max message from BBB plus two for header bit of which device to read from and read length

#define waitRead 200


SoftWire mag(5,6);

const byte SLAVE_ADDRESS = 0x20;

int MAGselect = 0;

int led = 13;

byte tempList[maxMsgLen];

byte MAGcommandList[maxMsgLen];
byte MAGresponse[maxRspLen];
int MAGreadLen;


void MAGcomm(int msgLen, byte tempList[]){
	for (int i = 0; i <msgLen;i++){
		MAGcommandList[i] = 0;
	}
	  Serial.print("Recieved ");
	  Serial.print(msgLen);
	  Serial.println(" bytes of data from BBB");
	  Serial.print("Sending data to iMTQ: ");
  for (int i = 0; i <msgLen;i++){
    MAGcommandList[i] = tempList[i+2]; //take off first two i2c device commands
	Serial.print(MAGcommandList[i]);
  }
  Serial.print("\n");

	for(int i =0;i<MAGreadLen;i++){
		MAGresponse[i] = 0;
	}
	mag.startWait(MAGADDR, SoftWire::writeMode);
	for(int i = 0; i <msgLen;i++){
		mag.write(MAGcommandList[i]);
	}
	mag.stop();
	delay(waitRead);
	//Serial.println("Reading from EPS device");
	//Change this to variable reads when you are reading more than 2 bytes
	mag.startWait(MAGADDR, SoftWire::readMode);
	for(int i =0;i<MAGreadLen-1;i++){
		mag.readThenAck(MAGresponse[i]);
	}
	mag.readThenNack(MAGresponse[MAGreadLen-1]);
	mag.stop();
	digitalWrite(led, HIGH);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
	if(MAGselect){
		Serial.print("Responding to BBB with MAG data: ");
		for(int i = 0; i<MAGreadLen;i++){
			Wire.write(MAGresponse[i]);
			Serial.print(MAGresponse[i]);
		}
		Serial.print("\n");
	}
	else{
		Serial.println("***unknown I2C device READ request***");
	}
}

void receiveEvent(size_t howMany){
	digitalWrite(led, LOW);
	//Serial.println("Recieved write command on I2C bus");
	int readLen;
	for(int i = 0;i<maxMsgLen;i++){
		tempList[i] = 0xFF;		//init tempList
	}
	int msgLen=0;
	while(0 < Wire.available() && msgLen < maxMsgLen){
		tempList[msgLen] = Wire.read();
		msgLen++;
	}
	// Read tempList[i] it will tell you how many bytes to read in hex
	byte compare = 0xFF;
	if(tempList[0] == MAGADDR){
		MAGselect = 1;
		readLen = tempList[1];
		if(readLen != compare){
			if(readLen == 140)
				MAGreadLen = 320;
			else
				MAGreadLen = readLen;
			MAGcomm((msgLen-2), tempList);
		}
	}
	else{
		Serial.println("***unknown I2C device WRITE request***");
	}

}


void setup() {
	Serial.begin(9600);
	Wire.begin(SLAVE_ADDRESS);                // join i2c bus with address #8
	mag.setDelay_us(5);
	mag.begin();
	Wire.onRequest(requestEvent); // register event
	Wire.onReceive(receiveEvent);
	Serial.println("starting MAG test");
	pinMode(led, OUTPUT);   
}

void loop() {
	delay(100);
}
