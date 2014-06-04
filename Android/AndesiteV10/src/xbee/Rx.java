package xbee;

import java.util.ArrayList;

public class Rx {

	// Return the Serial High address from the packet
	public static int getSh(ArrayList<Byte> packet)
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
	
	// Return the Serial Low address from the packet
	public static int getSl(ArrayList<Byte> packet)
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
	
	// Return the 16-bit address from the packet
	public static int getMy(ArrayList<Byte> packet)
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
	
	// Return the data type from the packet
	public static int getDataType(ArrayList<Byte> packet)
	{
		return Common.byteToHex(packet.get(21));
	}
	
	// Return the rest of the RF data from the packet
	public static int[] getData(ArrayList<Byte> packet)
	{
		int start = 24;
		int length = packet.size() - 1 - start;
		
		int[] temp = new int[length];
		for(int n = 0; n < length; n++)
		{
			temp[n] = Common.byteToHex(packet.get(n + start));
		}
		
		return temp;
	}
		
	// Return the routing table entries from a packet
	public static ArrayList<Integer[]> getRoutingTableEntries(ArrayList<Byte> packet)
	{
		int start = 26;
		int length = packet.size() - 1;
		ArrayList<Integer[]> routes = new ArrayList<Integer[]>(length);;
		
		for(int x = start; x < length; x+=5)
		{
			byte dest[] = {packet.get(x + 1), packet.get(x)};
			byte status = (byte)((packet.get(x + 2) >> 5) & 0xFF);
			byte[] nextHop = {packet.get(x + 4), packet.get(x + 3)};
			Integer array[] = {Common.byteToHex(dest), Common.byteToHex(status), Common.byteToHex(nextHop)};
			routes.add(array);
			
			//routes[x] = Common.byteToHex(dest) + Common.byteToHex(status) + Common.byteToHex(nextHop) + ", ";
		}
		
		return routes;
	}
}
