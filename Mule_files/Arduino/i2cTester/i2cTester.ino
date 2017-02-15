
#define MAGADDR 0x10
#define EPSADDR 0x2B
#define BATADDR 0x2A

#include <SoftWire.h>

#include <Wire.h>

#define maxRspLen 24
#define maxMsgLen 24 //max message plus one for header bit of which device to read from

SoftWire eps(5,6);
SoftWire mag(3,4);

const byte SLAVE_ADDRESS = 0x20;

int EPSselect = 0;
int BATselect = 0;
int MAGselect = 0;

byte tempList[maxMsgLen];

byte MAGcommandList[maxMsgLen];
byte MAGresponse[maxRspLen];

byte EPScommandList[maxMsgLen];
byte EPSresponse[maxRspLen];

byte BATcommandList[maxMsgLen];
byte BATresponse[maxRspLen];


void EPScomm(int msgLen, byte tempList[]){
	for (int i = 0; i <maxMsgLen;i++){
		EPScommandList[i] = 0;
	}
	for(int i =0;i<maxRspLen;i++){
		EPSresponse[i] = 0;
	}
	for (int i = 0; i <msgLen;i++){
		EPScommandList[i] = tempList[i+1];   //take off first i2c device command
	}
	Serial.print("Recieved ");
	Serial.print(msgLen);
	Serial.print(" bytes of data from BBB");
	eps.startWait(EPSADDR, SoftWire::writeMode);
	for(int i = 0; i <msgLen;i++){
		eps.write(EPScommandList[i]);
	}
	eps.stop();
	delay(2);
	//Serial.println("Reading from EPS device");
	//Change this to variable reads when you are reading more than 2 bytes
	eps.startWait(EPSADDR, SoftWire::readMode);
	for(int i =0;i<maxRspLen-1;i++){
		eps.readThenAck(EPSresponse[i]);
	}
	eps.readThenNack(EPSresponse[maxRspLen-1]);
	eps.stop();
}

void BATcomm(int msgLen, byte tempList[]){
	for (int i = 0; i <maxMsgLen;i++){
		BATcommandList[i] = 0;
	}
	for(int i =0;i<maxRspLen;i++){
		BATresponse[i] = 0;
	}
	for (int i = 0; i <msgLen;i++){
		BATcommandList[i] = tempList[i+1];   //take off first i2c device command
	}
	Serial.print("Recieved ");
	Serial.print(msgLen);
	Serial.print(" bytes of data from BBB");
	eps.startWait(BATADDR, SoftWire::writeMode);
	for(int i = 0; i <msgLen;i++){
		eps.write(BATcommandList[i]);
	}
	eps.stop();
	delay(2);
	//Serial.println("Reading from EPS device");
	//Change this to variable reads when you are reading more than 2 bytes
	eps.startWait(BATADDR, SoftWire::readMode);
	for(int i =0;i<maxRspLen-1;i++){
		eps.readThenAck(BATresponse[i]);
	}
	eps.readThenNack(BATresponse[maxRspLen-1]);
	eps.stop();
}
void MAGcomm(int msgLen, byte tempList[]){
	for (int i = 0; i <maxMsgLen;i++){
		MAGcommandList[i] = 0;
	}
	for(int i =0;i<maxRspLen;i++){
		MAGresponse[i] = 0;
	}
	for (int i = 0; i <msgLen;i++){
		MAGcommandList[i] = tempList[i+1]; //take off first i2c device command
	}
	Serial.print("Recieved ");
	Serial.print(msgLen);
	Serial.print(" bytes of data from BBB");
	mag.startWait(MAGADDR, SoftWire::writeMode);
	for(int i = 0; i <msgLen;i++){
		mag.write(MAGcommandList[i]);
	}
	mag.stop();
	delay(2);
	//Serial.println("Reading from EPS device");
	//Change this to variable reads when you are reading more than 2 bytes
	mag.startWait(MAGADDR, SoftWire::readMode);
	for(int i =0;i<maxRspLen-1;i++){
		mag.readThenAck(MAGresponse[i]);
	}
	mag.readThenNack(MAGresponse[maxRspLen-1]);
	mag.stop();
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
	if(EPSselect && !BATselect && !MAGselect){
		for(int i = 0; i<maxRspLen; i++){
			Wire.write(EPSresponse[i]);
			EPSresponse[i] = 0;
		}
		Serial.print("Responding to BBB with EPS data");
	}
	else if(!EPSselect && BATselect && !MAGselect){
		for(int i = 0; i<maxRspLen;i++){
			Wire.write(BATresponse[i]);
			BATresponse[i] = 0;
		}
		Serial.print("Responding to BBB with BATTERY data");
	}
	else if(!EPSselect && !BATselect && MAGselect){
		for(int i = 0; i<maxRspLen;i++){
			Wire.write(MAGresponse[i]);
			MAGresponse[i] = 0;
		}
		Serial.print("Responding to BBB with MAG data");
	}
	else{
		Serial.println("***unknown I2C device READ request***");
	}
}

void receiveEvent(size_t howMany){
	//Serial.println("Recieved write command on I2C bus");
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
	if(tempList[0] == EPSADDR){
		EPSselect = 1;
		BATselect = 0;
		MAGselect = 0;
		if(tempList[1] != compare){
			EPScomm(msgLen, tempList);
		}
	}
	else if(tempList[0] == BATADDR){
		EPSselect = 0;
		BATselect = 1;
		MAGselect = 0;
		if(tempList[1] != compare){
			BATcomm(msgLen, tempList);
		}
	}
	else if(tempList[0] == MAGADDR){
		EPSselect = 0;
		BATselect = 0;
		MAGselect = 1;
		if(tempList[1] != compare){
			MAGcomm(msgLen, tempList);
		}
	}
	else{
		Serial.println("***unknown I2C device WRITE request***");
	}

}


void setup() {
	Serial.begin(9600);
	Wire.begin(SLAVE_ADDRESS);                // join i2c bus with address #8
	eps.setDelay_us(5);
	mag.setDelay_us(5);
	eps.begin();
	mag.begin();
	Wire.onRequest(requestEvent); // register event
	Wire.onReceive(receiveEvent);
	Serial.println("starting EPS test");
}

void loop() {
	delay(100);
}
