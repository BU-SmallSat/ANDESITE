package com.example.andesitev10;

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
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import xbee.*;

public class Detumble extends IOIOActivity implements OnClickListener{

	private Uart uart1 = null, uart2 = null;
	private boolean isFinished = false, isIOIOSetup = false, isSecondRadioFound = false,
			hasSecondRadioNotified = false;
	private Button button = null;
	private static final String log = "APP_STATUS";
	private int[] secondRadioPacket;
	private XbeeInfo coordinatorXbee, broadcastXbee, secondXbee;
	private EditText in1_text = null, out1_text = null, in2_text = null, out2_text = null;
	private HashMap<Integer, RadioStream> streams;
	private static final int COORD_INDEX = 0, SECOND_INDEX = 1;
	private ArrayList<ArrayList<Byte>> packets;
	private final static String app = "APP_STATUS";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.one_button);
		
		// Declare button variables
		button = (Button)findViewById(R.id.button);
		
		// Set onClickListener for Buttons
		button.setOnClickListener(this);
		
		// Setup EditText fields to be accessed.
		in1_text = (EditText)findViewById(R.id.in1_text);
		in2_text = (EditText)findViewById(R.id.in2_text);
		out1_text = (EditText)findViewById(R.id.out1_text);
		out2_text = (EditText)findViewById(R.id.out2_text);
	}
	
	@Override
	public void onDestroy()
	{
		super.onDestroy();
		
		for(int x = 0; x < Constants.NUM_RADIOS; x++)
		{
			try {
				streams.get(Integer.valueOf(x)).closeStreams();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		uart1.close();
		uart2.close();
	}

	class Looper extends BaseIOIOLooper{
		
		protected void setup() throws ConnectionLostException, InterruptedException{
			//uart1 uses pins from Constants file and baud rate from Constants file
			uart1 = ioio_.openUart(Constants.UART_1_RX, Constants.UART_1_TX, Constants.IOIO_XBEE_BAUD
					, Uart.Parity.NONE, Uart.StopBits.ONE);
			
			//uart2 uses pins from Constants file and baud rate from Constants file
			uart2 = ioio_.openUart(Constants.UART_2_RX, Constants.UART_2_TX, Constants.IOIO_XBEE_BAUD,
					Uart.Parity.NONE, Uart.StopBits.ONE);
			
			// Setup RadioStream ArrayList to keep track of bytes being read in
			streams = new HashMap<Integer, RadioStream>(Constants.NUM_RADIOS);
			streams.put(Integer.valueOf(COORD_INDEX), new RadioStream(uart1.getInputStream(), uart1.getOutputStream()));
			streams.put(Integer.valueOf(SECOND_INDEX), new RadioStream(uart2.getInputStream(), uart2.getOutputStream()));
			
			//Setup coordinator and broadcast xbeeinfo objects
			coordinatorXbee = new XbeeInfo(Constants.COORD_SH, Constants.COORD_SL, Constants.COORD_MY);
			broadcastXbee = new XbeeInfo(Constants.BROADCAST_SH, Constants.BROADCAST_SL, Constants.BROADCAST_MY);
			
			//Create packet for second radio to send out to identify its address to the coordinator
			secondRadioPacket = Tx.makeNetworkPacket(coordinatorXbee, 1, Constants.SECOND_RADIO_ADDRESS);

			// Setup is complete
			isIOIOSetup = true;
		}
		
		public void loop() throws ConnectionLostException, InterruptedException
		{
			// If activity is finished, return to main activity.
			if(isFinished)
			{
				returnToMain();
			}
			
			// If the second radio has not sent out a notification to the coordinator,
			// have it send out a notification.
			if(!hasSecondRadioNotified)
			{
				secondRadioNotifies();
				hasSecondRadioNotified = true;
			}
			
			for(Integer x = 0; x < Constants.NUM_RADIOS; x++)
			{
				// Get RadioStream object
				RadioStream temp = streams.get(x);
				
				// Get the available packets from the stream
				packets = temp.getAvailablePackets();
				
				if(x == 0)
				{
					for(int n = 0; n < packets.size(); n++)
					{
						for(int i = 0; i < packets.get(n).size(); i++)
						{
							appendText(in1_text, Integer.toHexString(Common.byteToHex(packets.get(n).get(i))) + ", ");
						}
						appendText(in1_text, "\n");
					}
				}
				
				if(x == 1)
				{
					for(int n = 0; n < packets.size(); n++)
					{
						for(int i = 0; i < packets.get(n).size(); i++)
						{
							appendText(in2_text, Integer.toHexString(Common.byteToHex(packets.get(n).get(i))) + ", ");
						}
						appendText(in2_text, "\n");
					}
				}
			}
		}
	}
	
	// Second radio sends out a notification to the coordinator so the coordinator will know its address.
	private void secondRadioNotifies()
	{
		streams.get(SECOND_INDEX).sendPacket(secondRadioPacket);
		for(int s : secondRadioPacket)
		{
			appendText(out2_text, Integer.toHexString(s) + ", ");
			
		}
		appendText(out2_text, "\n");
	}
	
	// Append string to given EditText field
	private void appendText(final EditText edit_text, final String str)
	{
		runOnUiThread(new Runnable() {
            @Override
            public void run() {
                  edit_text.append(str);
            }
        });
	}
	
	// Set string to given EditText field
	private void setText(final EditText edit_text, final String str)
	{
		runOnUiThread(new Runnable() {
			@Override
			public void run() {
				edit_text.setText(str);
			}
		});
	}
	
	// Clear given EditText field
	private void clearText(final EditText edit_text)
	{
		runOnUiThread(new Runnable() {
			@Override
			public void run(){
				edit_text.setText("");
			}
		});
	}
	
	protected IOIOLooper createIOIOLooper(){
		return new Looper();
	}

	@Override
	public void onClick(View v) {
		if(v == button && isIOIOSetup)
		{
			isFinished = true;
		}
	}
	
	// Returns activity to the main acitivity
	private void returnToMain()
	{
		Intent returnIntent = new Intent();
		returnIntent.putExtra("message", "Returned from Detumble Activity.");
		setResult(RESULT_OK, returnIntent);
		finish();
		isFinished = false;
	}
	
}
