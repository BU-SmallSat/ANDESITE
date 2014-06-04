package xbee.regular;

import java.nio.ByteBuffer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;

public class Common {
	
	final protected static char[] hexArray = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
	DateFormat formatter = new SimpleDateFormat("dd/MM/yyyy hh:mm:ss.SSS");
	
	public String millisToDate(long millis)
	{
		Calendar calendar = Calendar.getInstance();
        calendar.setTimeInMillis(millis);
        return formatter.format(calendar.getTime());
	}
	
	public static String getFrameType(ArrayList<Byte> packet)
	{
		return byteToHex(packet.get(3));
	}
	
	public static String getPacketChecksum(ArrayList<Byte> packet)
	{
		return byteToHex(packet.get(packet.size() - 1));
	}
	
	public static int[] breakUp(int bits, int addressToBreakUp)
	{
		int length = 0;
		switch (bits)
		{
		case 32:
			length = 4;
			break;
		case 16:
			length = 2;
			break;
		}
		
		int[] brokenUpAddress = new int[length];
		
		for(int x = 0; x < length; x++)
		{
			brokenUpAddress[length-x-1] = addressToBreakUp % 0x100;
			addressToBreakUp /= 0x100;
		}
		
		return brokenUpAddress;
	}
	
	//Calculates and returns the checksum
	public static int getChecksum(int[] packet) {
		int length = packet.length;
		int sum = 0x0;
		
		for (int x = 3; x <length - 1; x++) { 
			sum += packet[x];
		}
			
		int checksum = (byte)0xFF - (byte)sum;
		return checksum;
	}
	
	//Calculates and returns the checksum
	public static int calcChecksum(int[] packet) {
		int length = packet.length;
		int sum = 0x0;
				
		for (int x = 3; x <length - 1; x++) { 
			sum += packet[x];
		}
					
		byte checksum = (byte)(0xFF - (byte)sum);
		
		return Integer.parseInt(byteToHex(checksum), 16);
	}
		
	//Calculates and returns the checksum
	public static byte calcChecksum(ArrayList<Byte> packet) {
		int length = packet.size();
		int sum = 0x0;
				
		for (int x = 3; x <length - 1; x++) { 
			sum += packet.get(x);
		}
					
		byte checksum = (byte) (0xFF - (byte)sum);
		return checksum;
	}
		
	//calculates and returns the MSB and LSB of the packet length
	public static byte[] calcLength(int[] packet) {
		short length = (short)(packet.length - 4);
		return Common.toByteArray(length);
	}
		
	public static int calcLength(ArrayList<Byte> packet)
	{
		return packet.size() - 4;
	}
	
	public static String getPacketLength(ArrayList<Byte> packet)
	{
		byte[] temp = {packet.get(1), packet.get(2)};
		return byteToHex(temp);
	}
	
	//converts float value to array of bytes
	public static byte[] toByteArray(float value)
	{
		byte[] bytes = new byte[4];
		ByteBuffer.wrap(bytes).putFloat(value);
		return bytes;
	}
	
	//converts double value to array of bytes
	public static byte[] toByteArray(double value)
	{
		byte[] bytes = new byte[8];
		ByteBuffer.wrap(bytes).putDouble(value);
		return bytes;
	}
	
	//converts long value to array of bytes
	public static byte[] toByteArray(long value)
	{
		byte[] bytes = new byte[8];
		bytes[7] = (byte) (value & 0xFF);
		bytes[6] = (byte) ((value >> 8) & 0xFF);
		bytes[5] = (byte) ((value >> 16) & 0xFF);
		bytes[4] = (byte) ((value >> 24) & 0xFF);
		bytes[3] = (byte) ((value >> 32) & 0xFF);
		bytes[2] = (byte) ((value >> 40) & 0xFF);
		bytes[1] = (byte) ((value >> 48) & 0xFF);
		bytes[0] = (byte) ((value >> 56) & 0xFF);
		return bytes;
	}
	
	//converts short value to array of bytes
	public static byte[] toByteArray(short value)
	{
		byte[] bytes = new byte[2];
		bytes[1] = (byte) (value & 0xFF);
		bytes[0] = (byte) ((value >> 8) & 0xFF);
		return bytes;
	}
	
	//converts int value to array of bytes
	public static byte[] toByteArray(int value)
	{
		byte[] bytes = new byte[4];
		bytes[3] = (byte) (value & 0xFF);
		bytes[2] = (byte) ((value >> 8) & 0xFF);
		bytes[1] = (byte) ((value >> 16) & 0xFF);
		bytes[0] = (byte) ((value >> 24) & 0xFF);
		return bytes;
	}
	
	//converts array of bytes to float value
	public static float toFloat(byte[] bytes)
	{
		return ByteBuffer.wrap(bytes).getFloat();
	}
	
	//converts array of bytes to double value
	public static double toDouble(byte[] bytes)
	{
		return ByteBuffer.wrap(bytes).getDouble();
	}
	
	//converts array of bytes to long value
	public static long toLong(byte[] bytes)
	{
		long value = (long) ((bytes[0] << 56) + (bytes[1] << 48) + (bytes[2] << 40)
				+ (bytes[3] << 32) + (bytes[4] << 24) + (bytes[5] << 16) + (bytes[6] << 8)
				+ (bytes[7] & 0xFF));
		return value;
	}
	
	//converts array of bytes to short value
	public static short toShort(byte[] bytes)
	{
		short value = (short) ((bytes[0] << 8) + (bytes[1] & 0xFF));
		return value;
	}
	
	//converts array of bytes to int value
	public static int toInt(byte[] bytes)
	{
		int value = (int) ((bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + (bytes[3] & 0xFF));
		return value;
	}
	
	public static String byteToHex(byte[] bytes) {
	    char[] hexChars = new char[bytes.length * 2];
	    int v;
	    for ( int j = 0; j < bytes.length; j++ ) {
	        v = bytes[j] & 0xFF;
	        hexChars[j * 2] = hexArray[v >>> 4];
	        hexChars[j * 2 + 1] = hexArray[v & 0x0F];
	    }
	    return new String(hexChars);
	}
	
	public static String byteToHex(byte b)
	{
		char[] hexChars = new char[2];
		int v = b & 0xFF;
		hexChars[0] = hexArray[v >>> 4];
		hexChars[1] = hexArray[v & 0x0F];
		return new String(hexChars);
	}
	
}
