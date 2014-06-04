package xbee.regular;

import java.util.ArrayList;

public class AtRemoteResponse {
	
	public static String getCommand(ArrayList<Byte> packet)
	{
		char first = (char)(byte)packet.get(15);
		char second = (char)(byte)packet.get(16);
		return String.valueOf(first) + String.valueOf(second);
	}
	
	public static String getFrameId(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(4));
	}
	
	public static String getStatus(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(17));
	}
	
	public static String getData(ArrayList<Byte> packet)
	{
		if(packet.size() > 19)
		{
			int start = 18;
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
	
	public static String getSh(ArrayList<Byte> packet)
	{
		int length = 4;
		int start = 5;
		
		byte[] temp = new byte[length];
		for(int n = 0; n < length; n++)
		{
			temp[n] = packet.get(n + start);
		}
		return Common.byteToHex(temp);
	}
	
	public static String getSl(ArrayList<Byte> packet)
	{
		int length = 4;
		int start = 9;
		
		byte[] temp = new byte[length];
		for(int n = 0; n < length; n++)
		{
			temp[n] = packet.get(n + start);
		}
		return Common.byteToHex(temp);
	}
	
	public static String getMy(ArrayList<Byte> packet)
	{
		int length = 2;
		int start = 13;
		
		byte[] temp = new byte[length];
		for(int n = 0; n < length; n++)
		{
			temp[n] = packet.get(n + start);
		}
		return Common.byteToHex(temp);
	}
	
	public static String getNi(ArrayList<Byte> packet)
	{
		int length = packet.size() - 1;
		byte temp[] = new byte[length - 18];
		String ni = "";
		
		for(int x = 18; x < length; x++)
		{
			temp[x - 18] = packet.get(x);
			ni += String.valueOf((char)Integer.parseInt(packet.get(x).toString()));
		}
		
		return ni;
	}

}
