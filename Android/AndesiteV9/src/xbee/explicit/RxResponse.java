package xbee.explicit;

import java.util.ArrayList;

public class RxResponse {
	
	public static String getDataType(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(21));
	}
	
	public static String getRun(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(22));
	}
	
	public static String getDataId(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(23));
	}

	public static String getReceiveOptions(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(20));
	}
	
	public static String getSh(ArrayList<Byte> packet)
	{
		int length = 4;
		int start = 4;
		
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
		int start = 8;
		
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
		int start = 12;
		
		byte[] temp = new byte[length];
		for(int n = 0; n < length; n++)
		{
			temp[n] = packet.get(n + start);
		}
		return Common.byteToHex(temp);
	}
	
	public static String getSourceEndpoint(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(14));
	}
	
	public static String getDestEndpoint(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(15));
	}
	
	public static String getClusterId(ArrayList<Byte> packet)
	{
		byte temp[] = {packet.get(16), packet.get(17)};
		return Common.byteToHex(temp);
	}
	
	public static String getProfileId(ArrayList<Byte> packet)
	{
		byte[] temp = {packet.get(18), packet.get(19)};
		return Common.byteToHex(temp);
	}
	
	public static String getData(ArrayList<Byte> packet)
	{
		int start = 24;
		int length = packet.size() - 1 - start;
		
		byte[] temp = new byte[length];
		for(int n = 0; n < length; n++)
		{
			temp[n] = packet.get(n + start);
		}
		
		return Common.byteToHex(temp);
	}
	
	public static double getPotentiometerValue(String data)
	{
		String first = data.substring(0, 2);
		String second = data.substring(2, 4);
		byte[] value = {(byte)Integer.parseInt(first, 16), (byte)Integer.parseInt(second, 16)};
		short shortValue = Common.toShort(value);
		return (double)Math.round(shortValue * 3.3 / 1024.0 * 100) / 100;
	}
	
	public static String getTimeStamp(String data)
	{
		int timeStampLength = 4;
		int[] intTimeStamp = new int[timeStampLength];
		int endIndex = timeStampLength * 2 + 4;
		for(int x = 4; x < endIndex; x+=2)
		{
			intTimeStamp[x / 2 - 2] = Integer.parseInt(data.substring(x, x + 2), 16);
		}
		String stringTimeStamp = "";
		for(int i : intTimeStamp)
		{
			if(i < 10)
			{
				stringTimeStamp += "0" + i;
			}
			else
			{
				stringTimeStamp += i + "";
			}
		}
		return stringTimeStamp;
	}
	
	public static int getMillis(String data)
	{
		String millis = "";
		int temp;
		int millisLength = 8;
		int millisStart = 12;
		byte[] bytes = new byte[4];
		int length = millisStart + millisLength;
		
		for(int x = millisStart; x < length; x+=2)
		{
			temp = Integer.parseInt(data.substring(x, x+2), 16);
			bytes[(x - millisStart) / 2] = (byte)temp;
			millis += data.substring(x, x+2);
		}
		return Integer.parseInt(millis, 16);
	}
	
	public static String getRoutingTableEntries(ArrayList<Byte> packet)
	{
		int start = 26;
		int length = packet.size() - 1;
		String routes = "";
		
		for(int x = start; x < length; x+=5)
		{
			byte dest[] = {packet.get(x + 1), packet.get(x)};
			byte status = (byte)((packet.get(x + 2) >> 5) & 0xFF);
			byte[] nextHop = {packet.get(x + 4), packet.get(x + 3)};
			routes += Common.byteToHex(dest) + Common.byteToHex(status) + Common.byteToHex(nextHop) + ", ";
		}
		
		return routes;
	}
	
	public static String getThroughputRun(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(22));
	}
	
	public static String getThroughputDataId(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(23));
	}
	
	public static String getFirstDataByte(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(22));
	}
	
}
