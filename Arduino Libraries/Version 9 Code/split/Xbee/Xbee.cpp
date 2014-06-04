#include "Arduino.h"
#include "Xbee.h"

void Xbee::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) ((address >> ((arrayLength * 8) - (8 * i))) & 0xFF);
	}
}

void Xbee::txCommandPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
	int dataType, int dataId, byte data[], int dataLength)
{
	byte MSB = 0x00, LSB = 0x00, frameType = 0x10, broadcastRadius = 0x00, options = 0x00;
	byte packetFrame[] = {START, MSB, LSB, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				broadcastRadius, options, dataType, dataId};
	
	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);
			
	for(int i = 0; i < shLength; i++) {
		packetFrame[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}
				
	int frameLength = sizeof(packetFrame);
	int transmitPacketLength = frameLength + dataLength + 1;
	byte transmitPacket[transmitPacketLength];
			
	//Fills frame in packetToSend
	int i = 0;
	for(; i < frameLength; i++) {
		transmitPacket[i] = packetFrame[i];
	}

	int start = i;
	//puts data in packetToSend
	if(data) {
		for(; i < transmitPacketLength - 1; i++) {
			transmitPacket[i] = data[i - start];
		}
	}

	transmitPacket[LSBIndex] = getLSB(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = getChecksum(transmitPacket, transmitPacketLength);

	for(int j = 0; j < transmitPacketLength; j++) {
		txPacket[j] = transmitPacket[j];
	}
}

void Xbee::txDataPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId, 
	String timeStamp, int value, int run)
{
	byte MSB = 0x00, LSB = 0x00, frameType = 0x10, frameId = 0x0, broadcastRadius = 0x00, options = 0x00;
	int dataLength = 2, timeStampLength = 6;
	byte packetFrame[] = {START, MSB, LSB, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, SENSOR_READING, run, dataId};
	
	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);
			
	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}
				
	int frameLength = sizeof(packetFrame);
	int transmitPacketLength = frameLength + dataLength + timeStampLength + 1;
	byte transmitPacket[transmitPacketLength];
			
	//Fills frame in packetToSend
	int index = 0;
	for(; index < frameLength; index++) {
		transmitPacket[index] = packetFrame[index];
	}
        
        int start = index;

	//Puts value and timeStamp into packetToSend
	byte byteValue[dataLength];
	breakUp(value, byteValue, dataLength);
	for(; index < start + dataLength; index++) {
		transmitPacket[index] = byteValue[index - start];
	}

	String temp;
	byte byteTimeStamp[timeStampLength];
	for(int i = 0; i < timeStampLength; i++) {
		temp = timeStamp.substring(2 * i, 2 * (i + 1));
		byteTimeStamp[i] = (byte)(temp.toInt() & 0xFF);
	}
        
        start = index;

	for(; index < timeStampLength + start; index++) {
		transmitPacket[index] = byteTimeStamp[index - start];
	}

	transmitPacket[LSBIndex] = getLSB(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = getChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		txPacket[i] = transmitPacket[i];
	}
}

void Xbee::atLocalCommand(byte atPacket[], int frameId, char atCommand[])
{
	byte MSB = 0x00, LSB = 0x04, frameType = 0x08;
	byte packet[] = {START, MSB, LSB, frameType, frameId, 0, 0, 0};
	packet[5] = atCommand[0];
	packet[6] = atCommand[1];
	packet[7] = getChecksum(packet, sizeof(packet));

	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}

void Xbee::atLocalCommand(byte atPacket[], int frameId, char atCommand[], char parameters[])
{
	byte MSB = 0x0, LSB = 0x0, frameType = 0x08;
	byte packetFrame[] = {START, MSB, LSB, frameType, frameId, 0x0, 0x0};
	packetFrame[5] = atCommand[0];
	packetFrame[6] = atCommand[1];
	
	//transfer everything from packetFrame to packet
	int frameLength = sizeof(packetFrame);
	int packetLength = frameLength + sizeof(parameters) + 1;
	byte packet[packetLength];
	for(int i = 0; i < frameLength; i++)
	{
		packet[i] = packetFrame[i];
	}
	
	//put the parameters in the packet
	for(int i = 7; i < packetLength - 1; i++)
	{
		packet[i] = parameters[i - 7];
	}
	
	//calculate the length and the checksum
	packet[LSBIndex] = getLSB(sizeof(packet));
	packet[packetLength - 1] = getChecksum(packet, sizeof(packet));
	
	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}

void Xbee::atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY, 
	int frameId, int remoteCommandOptions, char atCommand[])
{
	byte msb = 0x0, lsb = 0x0, frameType = 0x17, checksum = 0x0;
	byte packet[] = {START, msb, lsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, remoteCommandOptions,
		atCommand[0], atCommand[1], checksum};
		
	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);
			
	for(int i = 0; i < shLength; i++) {
		packet[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++)
	{
		packet[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packet[i + 13] = my[i];
	}
	
	//calculates and inserts length of the packet length
	packet[LSBIndex] = getLSB(sizeof(packet));
					
	//calculates and inserts checksum of the packet
	packet[sizeof(packet) - 1] = getChecksum(packet, sizeof(packet));

	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}

void Xbee::atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY,
	int frameId, int remoteCommandOptions, char atCommand[], char parameters[])
{
	byte msb = 0x0, lsb = 0x0, frameType = 0x17, checksum = 0x0;
	byte packetFrame[] = {START, msb, lsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, remoteCommandOptions,
		atCommand[0], atCommand[1]};
		
	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);
			
	for(int i = 0; i < shLength; i++) {
		packetFrame[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	//transfer everything from packetFrame to packet
	int frameLength = sizeof(packetFrame);
	int packetLength = frameLength + sizeof(parameters) + 1;
	byte packet[packetLength];
	for(int i = 0; i < frameLength; i++) {
			packet[i] = packetFrame[i];
		}

	for(int i = frameLength; i < packetLength - 1; i++) {
		packet[i] = parameters[i - frameLength];
	}
	
	//calculates and inserts length of the packet length
	packet[LSBIndex] = getLSB(sizeof(packet));
					
	//calculates and inserts checksum of the packet
	packet[sizeof(packet) - 1] = getChecksum(packet, sizeof(packet));

	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}

byte Xbee::getChecksum(byte packet[], int length)
{
	int sum = 0x0;

	for (int x = 3; x <length - 1; x++) { 
		sum += packet[x];
	}

	byte checksum = 0xFF - (byte)(sum & 0xFF);
	return checksum;
}

byte Xbee::getLSB(int packetLength)
{
	byte LSB = (byte) packetLength - 4;
	return LSB;
}

void Xbee::txDataTestPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId, 
	String timeStamp, int value, unsigned long timeMillis, int run)
{
	byte MSB = 0x00, LSB = 0x00, frameType = 0x10, frameId = 0x0, broadcastRadius = 0x00, options = 0x00;
	int dataLength = 2, timeStampLength = 6,millisLength = 4;
	byte packetFrame[] = {START, MSB, LSB, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, NETWORK_TEST, run, dataId};

	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);

	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	int frameLength = sizeof(packetFrame);
	int transmitPacketLength = frameLength + dataLength + timeStampLength + millisLength + 1;
	byte transmitPacket[transmitPacketLength];

	//Fills frame in packetToSend
	int index = 0;
	for(; index < frameLength; index++) {
		transmitPacket[index] = packetFrame[index];
	}

	//Puts value and timeStamp into packetToSend
	byte byteValue[dataLength];
	breakUp(value, byteValue, dataLength);
	for(int i = 0; i < dataLength; i++, index++) {
		transmitPacket[index] = byteValue[i];
	}

	String temp;
	byte byteTimeStamp[timeStampLength];
	for(int i = 0; i < timeStampLength; i++) {
		temp = timeStamp.substring(2 * i, 2 * (i + 1));
		byteTimeStamp[i] = (byte)(temp.toInt() & 0xFF);
	}

	for(int i = 0; i < timeStampLength; i++, index++) {
		transmitPacket[index] = byteTimeStamp[i];
	}

	//puts millis value in packet to send (for testing purposes)
	byte millisBytes[millisLength];
	breakUp(timeMillis, millisBytes, millisLength);
	for(int i = 0; i < millisLength; i++, index++) {
		transmitPacket[index] = millisBytes[i];
	}

	transmitPacket[LSBIndex] = getLSB(sizeof(transmitPacket));
	transmitPacket[transmitPacketLength - 1] = getChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		txPacket[i] = transmitPacket[i];
	}
}

void Xbee::toByteArray(int val, byte b[2])
{
    b[1] = (byte) (val & 0xFF);
    b[0] = (byte) ((val >> 8) & 0xFF);
}

int Xbee::toInt(byte b[2])
{
    int val = ((int) b[0]) << 8;
    val |= ((int) b[1]);
    return val;
}