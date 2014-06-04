package xbee.regular;

import java.util.ArrayList;

public class AtLocalResponse {
	
	public static String getFrameId(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(4));
	}
	
	public static String getCommand(ArrayList<Byte> packet)
	{
		char first = (char)(byte)packet.get(5);
		char second = (char)(byte)packet.get(6);
		return String.valueOf(first) + String.valueOf(second);
	}
	
	public static String getStatus(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(7));
	}
	
	public static String getData(ArrayList<Byte> packet)
	{
		if(packet.size() > 9)
		{
			int start = 8;
			int length = packet.size() - 1 - start;
			byte[] temp = new byte[length];
			for(int n = 0; n < length; n++)
			{
				temp[n] = packet.get(n + start);
			}
			return Common.byteToHex(temp);
		}
		
		return null;
	}

}
