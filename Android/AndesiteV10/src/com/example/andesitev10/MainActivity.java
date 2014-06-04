package com.example.andesitev10;

import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;

import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends IOIOActivity implements OnClickListener{
	
	private Uart uart1 = null, uart2 = null;
	private Button deploy = null, detumble = null, idle = null, science = null, transmit = null;
	private EditText result = null;
	private boolean isIOIOSetup = false;
	private InputStream in1 = null, in2 = null;
	private OutputStream out1 = null, out2 = null;
	private File dirs;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		// Declare Button variables
		deploy = (Button)findViewById(R.id.deploy);
		detumble = (Button)findViewById(R.id.detumble);
		idle = (Button)findViewById(R.id.idle);
		science = (Button)findViewById(R.id.science);
		transmit = (Button)findViewById(R.id.transmit);
		
		// Set onClickListener for Button variables
		deploy.setOnClickListener(this);
		detumble.setOnClickListener(this);
		idle.setOnClickListener(this);
		science.setOnClickListener(this);
		transmit.setOnClickListener(this);
		
		// Declare EditText Fields
		result = (EditText)findViewById(R.id.result);
		
		// Create andesite directory on sd card
		dirs = new File("/sdcard/Andesite/");
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
			/*in1 = uart1.getInputStream();
			in2 = uart2.getInputStream();
		
			//get the outputstream from the uart
			out1 = uart1.getOutputStream();
			out2 = uart2.getOutputStream();*/

			// Setup is complete
			isIOIOSetup = true;
		}
		
		public void loop() throws ConnectionLostException, InterruptedException
		{
		}
	}
	
	protected IOIOLooper createIOIOLooper(){
		return new Looper();
	}

	// Set actions to be taken based on which button is pressed
	@Override
	public void onClick(View v) {
		
		//If a new activity is going to be started, close the uarts
		if((v == deploy || v == detumble || v == idle || v == science || v == transmit) && isIOIOSetup)
		{
			closeUarts();
		}
		
		// Start deployment activity if deploy button is pressed.
		if(v == deploy && isIOIOSetup)
		{
			Intent deployIntent = new Intent(v.getContext(), Deployment.class);
			startActivityForResult(deployIntent, Constants.DEPLOY_ACTIVITY);
		}
		
		// Start detumble activity if detumble button is pressed.
		else if(v == detumble && isIOIOSetup)
		{
			Intent detumbleIntent = new Intent(v.getContext(), Detumble.class);
			startActivityForResult(detumbleIntent, Constants.DETUMBLE_ACTIVITY);
		}
		
		// Start idle activity if idle button is pressed.
		else if(v == idle && isIOIOSetup)
		{
			Intent idleIntent = new Intent(v.getContext(), Idle.class);
			startActivityForResult(idleIntent, Constants.IDLE_ACTIVITY);
		}
		
		// Start science activity if science button is pressed.
		else if(v == science && isIOIOSetup)
		{
			Intent scienceIntent = new Intent(v.getContext(), Science.class);
			startActivityForResult(scienceIntent, Constants.SCIENCE_ACTIVITY);
		}
		
		// Start transmit activity if transmit button is pressed.
		else if(v == transmit && isIOIOSetup)
		{
			Intent transmitIntent = new Intent(v.getContext(), Transmission.class);
			startActivityForResult(transmitIntent, Constants.TRANSMIT_ACTIVITY);
		}
	}
	
	//close the uarts
	public void closeUarts()
	{
		uart1.close();
		uart2.close();
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		// Process data from deploy activity.
		if(requestCode == Constants.DEPLOY_ACTIVITY)
		{
			if(resultCode == RESULT_OK)
			{
				result.setText(data.getStringExtra("message"));
			}
		}
		
		// Process data from detumble activity.
		else if(requestCode == Constants.DETUMBLE_ACTIVITY)
		{
			if(resultCode == RESULT_OK)
			{
				result.setText(data.getStringExtra("message"));
			}
		}
		
		// Process data from idle activity.
		else if(requestCode == Constants.IDLE_ACTIVITY)
		{
			if(resultCode == RESULT_OK)
			{
				result.setText(data.getStringExtra("message"));
			}
		}
		
		// Process data from science activity.
		else if(requestCode == Constants.SCIENCE_ACTIVITY)
		{
			if(resultCode == RESULT_OK)
			{
				result.setText(data.getStringExtra("message"));
			}
		}
		
		// Process data from transmit activity.
		else if(requestCode == Constants.TRANSMIT_ACTIVITY)
		{
			if(resultCode == RESULT_OK)
			{
				result.setText(data.getStringExtra("message"));
			}
		}
	}

}
