package com.example.andesitev9;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.HashMap;

import xbee.explicit.*;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import ioio.lib.api.AnalogInput;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;

public class Tester extends IOIOActivity implements OnClickListener{
	private Uart uart = null;
	private InputStream in = null;
	private OutputStream out = null;
	private EditText data_type, rf_data, sent, received;
	private Button send;
	private XbeeInfo broadcast;
	private boolean ioioReady = false;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.tester);
		
		data_type = (EditText)findViewById(R.id.data_type);
		rf_data = (EditText)findViewById(R.id.rf_data);
		sent = (EditText)findViewById(R.id.sent);
		received = (EditText)findViewById(R.id.received);
		send = (Button)findViewById(R.id.send);
		send.setOnClickListener(this);
		
		broadcast = new XbeeInfo(0x0, 0x0000ffff, 0xfffe, "BROADCAST");
	}
	
	@Override
	protected void onDestroy() 
	{
		super.onDestroy();
		
		if(uart != null)
		{
			uart.close();
		}
		
		if(in != null)
		{
			try {
				in.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		if(out != null)
		{
			try {
				out.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
	
class Looper extends BaseIOIOLooper{
		
	protected void setup() throws ConnectionLostException, InterruptedException{
		//uart uses pins 3 and 4 and baud rate of 19200
		uart = ioio_.openUart(3, 4, 19200
				, Uart.Parity.NONE, Uart.StopBits.ONE);
		
		//get the inputstream and outputstream from the uart
		in = uart.getInputStream();
		out = uart.getOutputStream();
		
		ioioReady = true;
	}
		
	public void loop() throws ConnectionLostException, InterruptedException
	{
		try {
			int available = in.available();
			byte[] rdbuff = new byte[available];		
			in.read(rdbuff);
			if(available > 0)
			{
				for(byte b : rdbuff)
				{
					appendReceived(Common.byteToHex(b) + ", ");
				}
				appendReceived("\n");
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

	public void appendReceived(final String text)
	{
		runOnUiThread( new Runnable() {
			public void run() {
				received.append(text);
			}
		});
	}

	protected IOIOLooper createIOIOLooper(){
		return new Looper();
	}

	@Override
	public void onClick(View v) {
		if(ioioReady)
		{
			String stringDataType = data_type.getText().toString();
			boolean isRfData = false;
			if(stringDataType.length() > 0)
			{
				int dataType = Integer.parseInt(stringDataType, 16);
				String stringData = rf_data.getText().toString();
				int[] data;
				int[] txPacket;
				if(stringData.length() > 0)
				{
					data = new int[stringData.length() / 2];
					for(int x = 0; x < stringData.length(); x+=2)
					{
						data[x/2] = Integer.parseInt(stringData.substring(x, x+2), 16);
					}
					txPacket = TxRequest.makePacket(broadcast, 0x0, dataType, data);
				}
				else
				{
					txPacket = TxRequest.makePacket(broadcast, 0x0, dataType);
				}
				
				String sentPacket = "";
				
				for(int t : txPacket)
				{
					try {
						out.write((byte)t);
						String ts = Common.byteToHex((byte)t);
						if(ts.length() < 2)
						{
							ts = "0" + ts;
						}
						sentPacket += ts + ", ";
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
				sent.append(sentPacket + "\n");
			}
		}
	}	
	
}