package xbee.explicit;

public class AtRemoteRequest {
	
	final protected static int START_BIT = 0x7e, LENGTH_MSB_INDEX = 1, LENGTH_LSB_INDEX = 2;
	
	//makes at remote command without parameters
	public static int[] makePacket(XbeeInfo xbeeInfo, int frameId, int remoteCommandOptions, char[] atCommand)
	{
		int msb = 0x0, lsb = 0x0, frameType = 0x17, checksum = 0x0;
		int[] packet = {START_BIT, msb, lsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, remoteCommandOptions,
				atCommand[0], atCommand[1], checksum};
			
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
		packet[packet.length - 1] = Common.calcChecksum(packet);
		return packet;
	}
		
	//makes at remote command without parameters
	public static int[] makePacket(XbeeInfo xbeeInfo, int frameId, int remoteCommandOptions, char[] atCommand, char[] parameters)
	{
		int msb = 0x0, lsb = 0x0, frameType = 0x17;
		int[] packetFrame = {START_BIT, msb, lsb, frameType, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, remoteCommandOptions,
				atCommand[0], atCommand[1]};
				
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
			
		//transfer everything from packetFrame to packet
		int frameLength = packetFrame.length;
		int packetLength = frameLength + parameters.length + 1;
		int[] packet = new int[packetLength];
		for(int x = 0; x < frameLength; x++)
		{
			packet[x] = packetFrame[x];
		}
			
		for(int n = 18; n < packetLength - 1; n++)
		{
			packet[n] = parameters[n - 18];
		}
				
		//calculates and inserts length of the packet length
		byte[] calculatedLength = Common.calcLength(packet);
		packet[LENGTH_MSB_INDEX] = calculatedLength[0];
		packet[LENGTH_LSB_INDEX] = calculatedLength[1];
								
		//calculates and inserts checksum of the packet
		packet[packet.length - 1] = Common.calcChecksum(packet);
		return packet;
		}

}
