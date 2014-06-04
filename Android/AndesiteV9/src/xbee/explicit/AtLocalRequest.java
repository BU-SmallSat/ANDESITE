package xbee.explicit;

public class AtLocalRequest {
	
	final protected static int START_BIT = 0x7e, LENGTH_MSB_INDEX = 1, LENGTH_LSB_INDEX = 2;
	
	//makes at local command packet without parameters
	public static int[] makePacket(int frameId, char[] atCommand)
	{
		int msb = 0x0, lsb = 0x04, frameType = 0x08;
		int[] packet = {START_BIT, msb, lsb, frameType, frameId, 0x0, 0x0, 0x0};
		packet[5] = atCommand[0];
		packet[6] = atCommand[1];
		packet[7] = Common.calcChecksum(packet);
		return packet;
	}
		
	//makes at local command packet with parameters
	public static int[] makePacket(int frameId, char[] atCommand, char[] parameters)
	{
		int lengthMsb = 0x0, lengthLsb = 0x0, frameType = 0x08;
		int[] framePacket = {START_BIT, lengthMsb, lengthLsb, frameType, frameId, 0x0, 0x0};
		framePacket[5] = atCommand[0];
		framePacket[6] = atCommand[1];
			
		//transfer everything from packetFrame to packet
		int frameLength = framePacket.length;
		int packetLength = frameLength + parameters.length + 1;
		int[] packet = new int[packetLength];
		for(int x = 0; x < frameLength; x++)
		{
			packet[x] = framePacket[x];
		}
			
		//put the parameters in the packet
		for(int x = 7; x < packetLength - 1; x++)
		{
			packet[x] = parameters[x - 7];
		}
			
		//calculate the length and the checksum
		byte[] calculatedLength = Common.calcLength(packet);
		packet[LENGTH_MSB_INDEX] = calculatedLength[0];
		packet[LENGTH_LSB_INDEX] = calculatedLength[1];
		packet[packetLength - 1] = Common.calcChecksum(packet);
		return packet;
	}

}
