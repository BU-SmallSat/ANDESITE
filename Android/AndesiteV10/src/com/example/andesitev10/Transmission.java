package com.example.andesitev10;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;

public class Transmission extends IOIOActivity implements OnClickListener{

	private Uart uart1 = null, uart2 = null;
	private InputStream in1 = null, in2 = null;
	private OutputStream out1 = null, out2 = null;
	private boolean isFinished = false, isIOIOSetup = false;
	private Button button = null;
	private static final String log = "APP_STATUS";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.one_button);
		
		// Declare button variables
		button = (Button)findViewById(R.id.button);
		
		// Set onClickListener for Buttons
		button.setOnClickListener(this);
	}
	
	@Override
	public void onDestroy()
	{
		uart1.close();
		uart2.close();
		
		try {
			in1.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			in2.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			out1.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			out2.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	class Looper extends BaseIOIOLooper{
		
		protected void setup() throws ConnectionLostException, InterruptedException{
			//uart1 uses pins from Constants file and baud rate from Constants file
			uart1 = ioio_.openUart(Constants.UART_1_RX, Constants.UART_1_TX, Constants.IOIO_XBEE_BAUD
					, Uart.Parity.NONE, Uart.StopBits.ONE);
			
			//uart2 uses pins from Constants file and baud rate from Constants file
			uart2 = ioio_.openUart(Constants.UART_2_RX, Constants.UART_2_TX, Constants.IOIO_XBEE_BAUD,
					Uart.Parity.NONE, Uart.StopBits.ONE);
			
			//get the inputstream from the uart
			in1 = uart1.getInputStream();
			in2 = uart2.getInputStream();
		
			//get the outputstream from the uart
			out1 = uart1.getOutputStream();
			out2 = uart2.getOutputStream();

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
		}
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
	
	private void returnToMain()
	{
		Intent returnIntent = new Intent();
		returnIntent.putExtra("message", "Returned from Transmission Activity.");
		setResult(RESULT_OK, returnIntent);
		finish();
		isFinished = false;
	}
	
}
