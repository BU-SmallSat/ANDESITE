#include "Arduino.h"
#include "Tx.h"

// creates a general packet with a data type followed by data
void Tx::makeNetworkPacket(Packet & packet, XbeeInfo & xbeeInfo, int dataType, int dataId, byte data[], int dataLength)
{
        // make sure the packet has no bytes already in it
        if(packet.length() > 0)
        {
			packet.clear();
        }
        
	/*byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, dataType, dataId}; //packet frame, the 0s are adresses to be added later*/
        
	int packetLength = 19;
	
        // create the packet except for the last byte which is the checksum
	for(int x = 0; x < packetLength; x++)
	{
		packet.add(0);
	}
	
        // set the given bytes in the packet
	packet.set(0, START_BYTE);
	packet.set(3, FRAME_TYPE);
	packet.set(4, FRAME_ID);
	packet.set(15, BROADCAST_RADIUS);
	packet.set(16, OPTIONS);
	packet.set(17, dataType);
	packet.set(18, dataId);

	//split up and put 64-bit addresses and 16-bit address in
	//need to use two parts for 64-bit address because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
        int myLength = 2;
	byte sh[shLength];
	byte sl[slLength];
        byte my[myLength];
	Common().breakUp(xbeeInfo.getSh(), sh, shLength);
	Common().breakUp(xbeeInfo.getSl(), sl, slLength);
        Common().breakUp(xbeeInfo.getMy(), my, myLength);
        

	for(int i = 0; i < shLength; i++) {
		packet.set(i + 5, sh[i]);
                packet.set(i + 9, sl[i]);
                if(i < 2)
                {
                    packet.set(i + 13, my[i]);
                }
	}

	//puts data in the packet if there is data
	int packetSize = packet.length();
	if(data)
	{
		for(int i = packetSize; i < packetSize + dataLength; i++)
		{
			packet.add(data[i - packetSize]);
		}
	}
        
        // add in the byte for the checksum
        packet.add(0);

	//calculates and puts MSB, LSB, and checksum into packet
        unsigned int calculatedLength = Common().calcLength(packet);
        byte lsb = byte(calculatedLength);
        byte msb = 0;
        if(calculatedLength > 0xff)
        {
            msb = calculatedLength - 0xff;
        }
        packet.set(1, msb);
	packet.set(2, lsb);
        packet.set(2, Common().calcLength(packet));
	packet.set(packet.length() - 1, Common().calcChecksum(packet));
}

// creates a general packet with a data type
void Tx::makeNetworkPacket(Packet & packet, XbeeInfo & xbeeInfo, int dataType)
{
        // make sure the packet has no bytes already in it
        if(packet.length() > 0)
        {
            packet.clear();
        }
        
	/*byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, dataType, dataId}; //packet frame, the 0s are adresses to be added later*/
        
	int packetLength = 19;
	
        // create the packet except for the last byte which is the checksum
	for(int x = 0; x < packetLength; x++)
	{
		packet.add(0);
	}
	
        // set the given bytes in the packet
	packet.set(0, START_BYTE);
	packet.set(3, FRAME_TYPE);
	packet.set(4, FRAME_ID);
	packet.set(15, BROADCAST_RADIUS);
	packet.set(16, OPTIONS);
	packet.set(17, dataType);

	//split up and put 64-bit addresses and 16-bit address in
	//need to use two parts for 64-bit address because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
        int myLength = 2;
	byte sh[shLength];
	byte sl[slLength];
        byte my[myLength];
	Common().breakUp(xbeeInfo.getSh(), sh, shLength);
	Common().breakUp(xbeeInfo.getSl(), sl, slLength);
        Common().breakUp(xbeeInfo.getMy(), my, myLength);
        

	for(int i = 0; i < shLength; i++) {
		packet.set(i + 5, sh[i]);
                packet.set(i + 9, sl[i]);
                if(i < 2)
                {
                    packet.set(i + 13, my[i]);
                }
	}

	//calculates and puts MSB, LSB, and checksum into packet
        unsigned int calculatedLength = Common().calcLength(packet);
        byte lsb = byte(calculatedLength);
        byte msb = 0;
        if(calculatedLength > 0xff)
        {
            msb = calculatedLength - 0xff;
        }
        packet.set(1, msb);
	packet.set(2, lsb);
        packet.set(2, Common().calcLength(packet));
	packet.set(packet.length() - 1, Common().calcChecksum(packet));
}

//Sends TX data packet
/*void TX::txDataPacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int dataId, 
	String timeStamp, int value)
{
	byte frameID = 0x0;
	int dataLength = 2, timeStampLength = 4;
	byte packetFrame[] = {Shared().getStart(), MSB, LSB, frameType, frameID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		broadcastRadius, options, SENSOR_READING, dataId}; //packet frame, the 0s are adresses to be added later
	
	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xBeeSH, sh, shLength);
	Shared().breakUp(xBeeSL, sl, slLength);

	for(int i = 0; i < shLength; i++) { 
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	//creates packet to be sent
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
	Shared().breakUp(value, byteValue, dataLength);
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

	//puts msb, lsb, and checksum into frame
	transmitPacket[Shared().getMSBIndex()] = Shared().getMSB(transmitPacketLength);
	transmitPacket[Shared().getLSBIndex()] = Shared().getLSB(transmitPacketLength);
	transmitPacket[transmitPacketLength - 1] = Shared().getChecksum(transmitPacket, transmitPacketLength);

	for(int i = 0; i < transmitPacketLength; i++) {
		txPacket[i] = transmitPacket[i];
	}
}*/