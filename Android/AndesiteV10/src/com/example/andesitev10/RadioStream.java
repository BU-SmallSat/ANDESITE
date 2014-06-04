package com.example.andesitev10;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

import xbee.Common;

public class RadioStream {
	
	private int availableBytes;
	private ArrayList<Byte> listBytes;
	private byte[] arrayBytes;
	private InputStream in;
	private OutputStream out;
	private ArrayList<ArrayList<Byte>> packets;
	
	// Constructor for RadioStream object that takes InputStream and OutputStream as its arguments
	RadioStream(InputStream in, OutputStream out)
	{
		availableBytes = 0;
		listBytes = new ArrayList<Byte>();
		arrayBytes = new byte[0];
		this.in = in;
		this.out = out;
		packets = new ArrayList<ArrayList<Byte>>();
	}
	
	// Sees how many bytes are available in the input stream
	public int calcAvailableBytes()
	{
		int available = 0;
		try {
			available = in.available();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return available;
	}
	
	// returns the input stream
	public InputStream getInStream()
	{
		return in;
	}

	// returns the output stream
	public OutputStream getOutStream()
	{
		return out;
	}
	
	// Read in available bytes to the arrayBytes variable
	public void readInBytes()
	{
		arrayBytes = new byte[availableBytes];
		try {
			in.read(arrayBytes);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}	
	
	// Return the next full packet from arrayBytes
	public int findNextPacket(int start)
	{
		int x;
		for(x = start; x < availableBytes; x++)
		{
			if(isCompletePacket(listBytes))
			{
				break;
			}
			listBytes.add(arrayBytes[x]);
		}
		
		return x;
	}
	
	// Evaluates packet to see if it is a complete packet
	public boolean isCompletePacket(ArrayList<Byte> listBytes)
	{
		if(listBytes.size() > 4
				&& Common.calcLength(listBytes) == Common.getPacketLength(listBytes)
				&& Common.calcChecksum(listBytes) == Common.getPacketChecksum(listBytes))
		{
			return true;
		}
		return false;
	}
	
	// Sends Packet using output stream
	public void sendPacket(int [] packet)
	{
		try {
			for(int p : packet)
			{
				out.write((byte)p);
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	// Close input and output streams
	public void closeStreams() throws IOException
	{
		in.close();
		out.close();
	}
	
	// Remove complete packets from the packets ArrayList
	public void removeCompletePackets()
	{
		for(int x = 0; x < packets.size(); x++)
		{
			if(isCompletePacket(packets.get(x)))
			{
				packets.remove(x);
				x--;
			}
		}
	}
	
	// Reads in all currents from the stream to return all complete packets
	public ArrayList<ArrayList<Byte>> getAvailablePackets()
	{
		removeCompletePackets();
		availableBytes = calcAvailableBytes();
		
		if(availableBytes > 0)
		{
			readInBytes();
			
			for(int n = 0; n < availableBytes;)
			{
				n = findNextPacket(n);
				
				if(isCompletePacket(listBytes))
				{
					ArrayList<Byte> temp = new ArrayList<Byte>();
					for(int x = 0; x < listBytes.size(); x++)
					{
						temp.add(listBytes.get(x));
					}
					packets.add(temp);
					listBytes.clear();
				}
			}
		}
		
		return packets;
	}
	
}
