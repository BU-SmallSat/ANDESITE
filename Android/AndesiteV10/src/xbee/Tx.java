package xbee;

public class Tx {
	
	final protected static int START_BIT = 0x7e, LENGTH_MSB_INDEX = 1, LENGTH_LSB_INDEX = 2,
			BROADCAST_RADIUS = 0, OPTIONS = 0, EXPLICIT_FRAMETYPE = 0x11, REGULAR_FRAMETYPE = 0x10,
			LENGTH_MSB = 0, LENGTH_LSB = 0, CHECKSUM = 0;
	
	//Makes a packet according to the explicit packet type of 0x11
	public static int[] makeExplicitPacket(XbeeInfo xbeeInfo, int frameId, int transactionNumber, int clusterId, int startIndex)
	{
		// Variables for ints to go into the packet.
		int sourceEndpoint = 0x0, destEndpoint = 0x0, profileId0 = 0x0, profileId1 = 0x0;
		
		// Create the packet
		int[] packet = {START_BIT, LENGTH_MSB, LENGTH_MSB, EXPLICIT_FRAMETYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				sourceEndpoint, destEndpoint, 0x0, 0x0, profileId0, profileId1, BROADCAST_RADIUS,
				OPTIONS, transactionNumber, startIndex, CHECKSUM};
		
		// Get packet length
		int packetLength = packet.length;
		
		//split up and put 64-bit address in
		int[] shAddress = Common.breakUp(32, xbeeInfo.getSh());
		int[] slAddress = Common.breakUp(32, xbeeInfo.getSl());
				
		for(int i = 0; i < 4; i++) {
			packet[i + 5] = shAddress[i];
		}
				
		for(int i = 0; i < 4; i++)
		{
			packet[i+9] = slAddress[i];
		}
				
		//Splits up and puts the 16-bit address into the frame
		int[] myAddress = Common.breakUp(16, xbeeInfo.getMy());
		for(int i = 0; i < 2; i++) {
			packet[i + 13] = myAddress[i];
		}
					
		//Splits up and puts the cluster id in
		int[] cluster = Common.breakUp(16, clusterId);
		packet[17] = cluster[0];
		packet[18] = cluster[1];
		
		//calculates and inserts length of the packet length
		byte[] calculatedLength = Common.calcLength(packet);
		packet[LENGTH_MSB_INDEX] = calculatedLength[0];
		packet[LENGTH_LSB_INDEX] = calculatedLength[1];
						
		//calculates and inserts checksum of the packet
		packet[packetLength - 1] = Common.calcChecksum(packet);
		
		//return packet
		return packet;
	}
	
	//Make a packet with science data to be transmitted.
	public static int[] makeDataPacket(XbeeInfo xbeeInfo, int frameId, int dataType, int dataId, int[] data)
	{		
		// Make the packet.
		int[] packetFrame = {START_BIT, LENGTH_MSB, LENGTH_LSB, REGULAR_FRAMETYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				BROADCAST_RADIUS, OPTIONS, dataType, dataId};
		
		//split up and put 64-bit address in
		int[] shAddress = Common.breakUp(32, xbeeInfo.getSh());
		int[] slAddress = Common.breakUp(32, xbeeInfo.getSl());
				
		for(int i = 0; i < 4; i++) {
			packetFrame[i + 5] = shAddress[i];
		}
				
		for(int i = 0; i < 4; i++)
		{
			packetFrame[i+9] = slAddress[i];
		}
				
		//Splits up and puts the 16-bit address into the frame
		int[] myAddress = Common.breakUp(16, xbeeInfo.getMy());
		for(int i = 0; i < 2; i++) {
			packetFrame[i + 13] = myAddress[i];
		}
					
		int frameLength = packetFrame.length;
		int transmitPacketLength = frameLength + data.length + 1;
		int[] transmitPacket = new int[transmitPacketLength];
				
		//Fills frame in packetToSend
		int i = 0;
		for(; i < frameLength; i++) {
			transmitPacket[i] = packetFrame[i];
		}
		
		int start = i;
				
		//puts data in packetToSend
		if(data != null)
		{
			for(; i < transmitPacketLength - 1; i++) {
				transmitPacket[i] = data[i - start];
			}
		}
				
		//calculates and inserts length of the packet length
		byte[] calculatedLength = Common.calcLength(transmitPacket);
		transmitPacket[LENGTH_MSB_INDEX] = calculatedLength[0];
		transmitPacket[LENGTH_LSB_INDEX] = calculatedLength[1];
				
		//calculates and inserts checksum of the packet
		transmitPacket[transmitPacketLength - 1] = Common.calcChecksum(transmitPacket);
		
		return transmitPacket;
	}
	
	// Makes a packet that will be used to send codes for information purposes for the network.
	public static int[] makeNetworkPacket(XbeeInfo xbeeInfo, int frameId, int dataType)
	{		
		// Make the packet.
		int[] packetFrame = {START_BIT, LENGTH_MSB, LENGTH_LSB, REGULAR_FRAMETYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				BROADCAST_RADIUS, OPTIONS, dataType, CHECKSUM};
		
		//split up and put 64-bit address in
		int[] shAddress = Common.breakUp(32, xbeeInfo.getSh());
		int[] slAddress = Common.breakUp(32, xbeeInfo.getSl());
				
		for(int i = 0; i < 4; i++) {
			packetFrame[i + 5] = shAddress[i];
		}
				
		for(int i = 0; i < 4; i++)
		{
			packetFrame[i+9] = slAddress[i];
		}
				
		//Splits up and puts the 16-bit address into the frame
		int[] myAddress = Common.breakUp(16, xbeeInfo.getMy());
		for(int i = 0; i < 2; i++) {
			packetFrame[i + 13] = myAddress[i];
		}
					
		int frameLength = packetFrame.length;
				
		//calculates and inserts length of the packet length
		byte[] calculatedLength = Common.calcLength(packetFrame);
		packetFrame[LENGTH_MSB_INDEX] = calculatedLength[0];
		packetFrame[LENGTH_LSB_INDEX] = calculatedLength[1];
				
		//calculates and inserts checksum of the packet
		packetFrame[frameLength - 1] = Common.calcChecksum(packetFrame);
		
		return packetFrame;
	}
	
	// Makes a packet that will be used to send codes for information purposes for the network.
	public static int[] makeNetworkPacket(XbeeInfo xbeeInfo, int frameId, int dataType, int[] data)
	{
		// Make the packet
		int[] packetFrame = {START_BIT, LENGTH_MSB, LENGTH_LSB, REGULAR_FRAMETYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				BROADCAST_RADIUS, OPTIONS, dataType};
		
		//split up and put 64-bit address in
		int[] shAddress = Common.breakUp(32, xbeeInfo.getSh());
		int[] slAddress = Common.breakUp(32, xbeeInfo.getSl());
				
		for(int i = 0; i < 4; i++) {
			packetFrame[i + 5] = shAddress[i];
		}
				
		for(int i = 0; i < 4; i++)
		{
			packetFrame[i+9] = slAddress[i];
		}
				
		//Splits up and puts the 16-bit address into the frame
		int[] myAddress = Common.breakUp(16, xbeeInfo.getMy());
		for(int i = 0; i < 2; i++) {
			packetFrame[i + 13] = myAddress[i];
		}
					
		int frameLength = packetFrame.length;
		int transmitPacketLength = frameLength + data.length + 1;
		int[] transmitPacket = new int[transmitPacketLength];
				
		//Fills frame in packetToSend
		int i = 0;
		for(; i < frameLength; i++) {
			transmitPacket[i] = packetFrame[i];
		}
		
		int start = i;
				
		//puts data in packetToSend
		if(data != null)
		{
			for(; i < transmitPacketLength - 1; i++) {
				transmitPacket[i] = data[i - start];
			}
		}
				
		//calculates and inserts length of the packet length
		byte[] calculatedLength = Common.calcLength(transmitPacket);
		transmitPacket[LENGTH_MSB_INDEX] = calculatedLength[0];
		transmitPacket[LENGTH_LSB_INDEX] = calculatedLength[1];
				
		//calculates and inserts checksum of the packet
		transmitPacket[transmitPacketLength - 1] = Common.calcChecksum(transmitPacket);
		
		return transmitPacket;
	}
	
	// Gets the routing tables from radio that the xbeeInfo object contains address information for
	public static int[] getRoutingTables(XbeeInfo xbeeInfo, int frameId, int transactionNumber, int startIndex)
	{
		// Variables for ints to go into the packet.
		int sourceEndpoint = 0x0, destEndpoint = 0x0, profileId0 = 0x0, profileId1 = 0x0,
				clusterId0 = 0x0, clusterId1 = 0x32;
		
		// Make the packet
		int[] packet = {START_BIT, LENGTH_MSB, LENGTH_LSB, EXPLICIT_FRAMETYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				sourceEndpoint, destEndpoint, clusterId0, clusterId1, profileId0, profileId1, BROADCAST_RADIUS,
				OPTIONS, transactionNumber, startIndex, CHECKSUM};
		
		// Get the packet length
		int packetLength = packet.length;
		
		//split up and put 64-bit address in
		int[] shAddress = Common.breakUp(32, xbeeInfo.getSh());
		int[] slAddress = Common.breakUp(32, xbeeInfo.getSl());
				
		for(int i = 0; i < 4; i++) {
			packet[i + 5] = shAddress[i];
		}
				
		for(int i = 0; i < 4; i++)
		{
			packet[i+9] = slAddress[i];
		}
				
		//Splits up and puts the 16-bit address into the frame
		int[] myAddress = Common.breakUp(16, xbeeInfo.getMy());
		for(int i = 0; i < 2; i++) {
			packet[i + 13] = myAddress[i];
		}
		
		//calculates and inserts length of the packet length
		byte[] calculatedLength = Common.calcLength(packet);
		packet[LENGTH_MSB_INDEX] = calculatedLength[0];
		packet[LENGTH_LSB_INDEX] = calculatedLength[1];
						
		//calculates and inserts checksum of the packet
		packet[packetLength - 1] = Common.calcChecksum(packet);
		
		return packet;
	}

}
