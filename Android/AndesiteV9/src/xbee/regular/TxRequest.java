package xbee.regular;

public class TxRequest {
	
	final protected static int START_BIT = 0x7e, LENGTH_MSB_INDEX = 1, LENGTH_LSB_INDEX = 2;
	
	public static int[] makePacket(XbeeInfo xbeeInfo, int frameId, int dataType, int dataId, int[] data)
	{
		
		int lengthMsb = 0x0, lengthLsb = 0x0, frameType = 0x10, broadcastRadius = 0x0, options = 0x0;
		int[] packetFrame = {START_BIT, lengthMsb, lengthLsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				broadcastRadius, options, dataType, dataId};
		
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
	
	public static int[] makePacket(XbeeInfo xbeeInfo, int frameId, int dataType)
	{
		
		int lengthMsb = 0x0, lengthLsb = 0x0, frameType = 0x10, broadcastRadius = 0x0, options = 0x0,
				checksum = 0x0;
		int[] packetFrame = {START_BIT, lengthMsb, lengthLsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				broadcastRadius, options, dataType, checksum};
		
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
	
	public static int[] makePacket(XbeeInfo xbeeInfo, int frameId, int dataType, int[] data)
	{
		
		int lengthMsb = 0x0, lengthLsb = 0x0, frameType = 0x10, broadcastRadius = 0x0, options = 0x0;
		int[] packetFrame = {START_BIT, lengthMsb, lengthLsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				broadcastRadius, options, dataType};
		
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
	
}
