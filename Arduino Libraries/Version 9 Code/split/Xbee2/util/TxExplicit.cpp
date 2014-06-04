#include "Arduino.h"
#include "TXExplicit.h"

void TXExplicit::makePacket(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
	int transactionNumber, int clusterId, int startIndex)
{
	byte packet[] = {Shared().getStart(), Shared().getMSB(), Shared().getLSB(),
		frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sourceEndpoint,
		destEndpoint, 0, 0, profileId0, profileId1, broadcastRadius, options
		transactionNumber, startIndex, checksum};
	int packetLength = sizeof(packet);

	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xBeeSH, sh, shLength);
	Shared().breakUp(xBeeSL, sl, slLength);
			
	for(int i = 0; i < shLength; i++) {
		packet[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++) {
		packet[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packet[i + 13] = my[i];
	}

	int clusterLength = 2;
	byte cluster[clusterLength];
	Shared().breakUp(clusterId, cluster, clusterLength);
	for(int i = 0; i < clusterLength; i++) {
		packet[i + 17] = cluster[i];
	}

	packet[Shared().getLSBIndex()] = Shared().getLSB(packet);
	packet[packetLength - 1] = Shared().getChecksum(packet, packetLength);

	for(int i = 0; i < packetLength) {
		txPacket[i] = packet[i];
	}
}

void TXExplicit::getRoutingTables(byte txPacket[], long xBeeSH, long xBeeSL, long xBeeMY, int frameId,
	int transactionNumber, int startIndex)
{
	byte clusterId0 = 0x0, clusterId1 = 0x32;
	byte packet[] = {Shared().getStart(), Shared().getMSB(), Shared().getLSB(),
		frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sourceEndpoint,
		destEndpoint, clusterId0, clusterId1, profileId0, profileId1, broadcastRadius, options
		transactionNumber, startIndex, checksum};
	int packetLength = sizeof(packet);

	//split up and put 64-bit address in
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	Shared().breakUp(xBeeSH, sh, shLength);
	Shared().breakUp(xBeeSL, sl, slLength);
			
	for(int i = 0; i < shLength; i++) {
		packet[i + 5] = sh[i];
	}
			
	for(int i = 0; i < slLength; i++) {
		packet[i + 9] = sl[i];
	}
			
	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	Shared().breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packet[i + 13] = my[i];
	}

	packet[Shared().getLSBIndex()] = Shared().getLSB(packet);
	packet[packetLength - 1] = Shared().getChecksum(packet, packetLength);

	for(int i = 0; i < packetLength) {
		txPacket[i] = packet[i];
	}
}