package com.example.andesitev9;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import xbee.explicit.*;

public class XbeeQuery extends IOIOActivity implements OnClickListener{
	private Uart uart = null;
	private OutputStream out = null;
	private InputStream in= null;
	private EditText at_command, parameters, packet, data, sent, error_count;
	private Button send_local_command = null, send_remote_command = null;
	private static final String REMOTE_COMMAND_RESPONSE = "97", RX_RESPONSE = "90", LOCAL_COMMAND_RESPONSE = "88";
	final public static String NI_COORD = "C", NI_ROUTER_ZERO = "R0", NI_ROUTER_ONE = "R1", NI_ROUTER_TWO = "R2",
			broadcast = "Broadcast Node";
	private HashMap<String, XbeeInfo> xbeeInfoObjects = new HashMap<String, XbeeInfo>();
	private XbeeInfo xbeeInfo;
	private XbeeInfo xbeeBroadcast;
	private int shBroadcast = 0x0;
	private int slBroadcast = 0x0000FFFF;
	private int myBroadcast = 0xFFFE;
	private ArrayList<Byte> receivedBytes;
	private int errors = 0;
	private String errorString = "";
	private boolean ioioReady = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.xbee_query);
		
		at_command = (EditText)findViewById(R.id.at_command);
		parameters = (EditText)findViewById(R.id.parameters);
		packet = (EditText)findViewById(R.id.packet);
		data = (EditText)findViewById(R.id.data);
		sent = (EditText)findViewById(R.id.sent);
		send_local_command = (Button)findViewById(R.id.send_local_command);
		send_local_command.setOnClickListener(this);
		send_remote_command = (Button)findViewById(R.id.send_remote_command);
		send_remote_command.setOnClickListener(this);
		error_count = (EditText)findViewById(R.id.error_count);
		
		/*//create an intent and get all the sh, sl, and my addresses passed from StartDataCollection.java
		Intent intent = getIntent();
		String[] shExtra = intent.getStringArrayExtra("sh");
		String[] slExtra = intent.getStringArrayExtra("sl");
		String[] myExtra = intent.getStringArrayExtra("my");
				
		//makes XbeeInfo objects and puts them into hashmap based on the sh, sl, and my in the intent
		for(int x = 0; x < shExtra.length; x++)
		{
			xbeeInfoObjects.put(Integer.toString(x), new XbeeInfo(Integer.parseInt(shExtra[x], 16),
					Integer.parseInt(slExtra[x], 16), Integer.parseInt(myExtra[x], 16)));
		}
				
		xbeeBroadcast = new XbeeInfo(shBroadcast, slBroadcast, myBroadcast, broadcast);
		xbeeInfoObjects.put(broadcast, xbeeBroadcast);*/
		
		xbeeInfo = new XbeeInfo(0x0, 0x0000FFFF, 0XFFFE, NI_ROUTER_TWO);
		receivedBytes = new ArrayList<Byte>();
	}
	
	@Override
	protected void onDestroy()
	{
		super.onDestroy();
		
		try {
			if(in != null)
			{
				in.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			if(out != null)
			{
				out.close();
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			if(uart != null)
			{
				uart.close();
			}
		} finally {
		}
	}
	
	class Looper extends BaseIOIOLooper{
		
		
		protected void setup() throws ConnectionLostException, InterruptedException{
			//uart uses pins 3 and 4 and baud rate of 9600
			uart = ioio_.openUart(3, 4, 19200
					, Uart.Parity.NONE, Uart.StopBits.ONE);
			
			in = uart.getInputStream();
			out = uart.getOutputStream();
			
			ioioReady = true;
		}
		
		public void loop() throws ConnectionLostException, InterruptedException
		{
			runOnUiThread(new Runnable(){
				public void run()
				{
					try {
						int availableBytes = in.available();
						byte[] rdBuff = new byte[availableBytes];
						in.read(rdBuff);
						if(availableBytes > 0)
						{
							for(byte b : rdBuff)
							{
								if(receivedBytes.size() > 0 && Common.byteToHex(b).equals("7e") && 
										Common.byteToHex(Common.calcChecksum(receivedBytes)).equals(Common.getPacketChecksum(receivedBytes)) && 
										Common.calcLength(receivedBytes) == Integer.parseInt(Common.getPacketLength(receivedBytes), 16))
								{
									if(Common.getFrameType(receivedBytes).equalsIgnoreCase(REMOTE_COMMAND_RESPONSE))
									{
										String at = AtRemoteResponse.getCommand(receivedBytes);
										String parameters = AtRemoteResponse.getData(receivedBytes);
										if(AtRemoteResponse.getCommand(receivedBytes).equalsIgnoreCase("NI"))
										{
											parameters = AtRemoteResponse.getNi(receivedBytes);
											Log.d("DATA_TAG", parameters);
										}
										data.append("Remote At Command: " + at + ", Command Data: " + parameters + "\n");
									}
									else if(Common.getFrameType(receivedBytes).equalsIgnoreCase(LOCAL_COMMAND_RESPONSE))
									{
										String at = AtLocalResponse.getCommand(receivedBytes);
										String parameters = AtLocalResponse.getData(receivedBytes);
										data.append("Local At Command: " + at + ", Command Data: " + parameters + "\n");
									}
									/*else if(Common.getFrameType(receivedBytes).equalsIgnoreCase(RX_RESPONSE))
									{
										String dataType = RxResponse.getDataType(receivedBytes);
										String dataId = RxResponse.getDataId(receivedBytes);
										String rxData = RxResponse.getData(receivedBytes);
										data.append("Data Type: " + dataType + ", Data Id: " + dataId + ", Data: " + rxData + "\n");
									}*/
									receivedBytes.clear();
									packet.append("\n");
								}
								receivedBytes.add(b);
								packet.append(Common.byteToHex(b) + ", ");
							}
							
							error_count.setText(errors + "\n" + errorString);
							
							if(receivedBytes.size() > 0 && 
									Common.byteToHex(Common.calcChecksum(receivedBytes)).equals(Common.getPacketChecksum(receivedBytes)) && 
									Common.calcLength(receivedBytes) == Integer.parseInt(Common.getPacketLength(receivedBytes), 16))
							{
								if(Common.getFrameType(receivedBytes).equalsIgnoreCase(REMOTE_COMMAND_RESPONSE))
								{
									String at = AtRemoteResponse.getCommand(receivedBytes);
									String parameters = AtRemoteResponse.getData(receivedBytes);
									if(AtRemoteResponse.getCommand(receivedBytes).equalsIgnoreCase("NI"))
									{
										parameters = AtRemoteResponse.getNi(receivedBytes);
										Log.d("DATA_TAG", parameters);
									}
									data.append("Remote At Command: " + at + ", Command Data: " + parameters + "\n");
								}
								else if(Common.getFrameType(receivedBytes).equalsIgnoreCase(LOCAL_COMMAND_RESPONSE))
								{
									String at = AtLocalResponse.getCommand(receivedBytes);
									String parameters = AtLocalResponse.getData(receivedBytes);
									data.append("Local At Command: " + at + ", Command Data: " + parameters + "\n");
								}
								/*else if(Common.getFrameType(receivedBytes).equalsIgnoreCase(RX_RESPONSE))
								{
									String dataType = RxResponse.getDataType(receivedBytes);
									String dataId = RxResponse.getDataId(receivedBytes);
									String rxData = RxResponse.getData(receivedBytes);
									data.append("Data Type: " + dataType + ", Data Id: " + dataId + ", Data: " + rxData + "\n");
								}*/
								receivedBytes.clear();
								packet.append("\n");
							}
							
							error_count.setText(errors + "\n" + errorString);
						}						
					} catch (IOException e) {
							e.printStackTrace();
					}catch(Throwable t){
						Toast.makeText(getApplicationContext(), t.toString(), Toast.LENGTH_LONG).show();
						errors++;
						errorString += "Error: " + t.toString() + "\n";
					}
				}
			});
		}
	}
	
	protected IOIOLooper createIOIOLooper(){
		return new Looper();
	}
	
	@Override
	public void onClick(View v) {
		if(ioioReady)
		{
			if(v == send_local_command || v == send_remote_command)
			{
				String command = at_command.getText().toString().toUpperCase();
				if(command != "")
				{
					int frameId = 0x01;
					int options = 0x02;
					char[] charCommand = {command.charAt(0), command.charAt(1)};
					int[] atPacket;
					if(v == send_local_command)
					{
						atPacket = AtLocalRequest.makePacket(frameId, charCommand);
					}
					else
					{
						atPacket = AtRemoteRequest.makePacket(xbeeInfo, frameId, options, charCommand);
					}
			
					try {
						for(int a : atPacket)
						{
							out.write((byte)a);
							sent.append(Integer.toHexString(a) + ", ");
						}
						sent.append("\n");
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}
		
			/*if(v == send_explicit_tx)
			{
				int clusterId = 0x0032;
				int frameId = 0x0;
				int transactionNumber = 0x76;
				int startIndex = 0;
				int sh = 0x0013A200, sl = 0x409BB9E7;
				XbeeInfo xbeeInfo = new XbeeInfo(sh, sl);
				int[] explicitTx = Xbee.explicitTxPacket(xbeeInfo, frameId, transactionNumber, clusterId, startIndex);
			
				try {
					for(int x : explicitTx)
					{
						out.write((byte)x);
						sent.append(Integer.toHexString(x) + ", ");
					}
					sent.append("\n");
				} catch (IOException e) {
					e.printStackTrace();
				}
			}*/
		}
		else
		{
			Toast.makeText(getApplicationContext(), "IOIO connection has not been extablished yet. Please try again.", Toast.LENGTH_SHORT).show();
		}
	}
}
