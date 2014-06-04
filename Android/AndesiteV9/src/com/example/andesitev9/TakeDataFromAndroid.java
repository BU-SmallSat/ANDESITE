package com.example.andesitev9;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.*;
import android.content.Intent;

public class TakeDataFromAndroid extends Activity implements SensorEventListener, OnClickListener{
	//create variables for accessing the internal magnetic sensor of the android device
	SensorManager sensorManager = null;
	Sensor magneticSensor = null;
	SensorEvent event = null;
	TextView dispGaussX = null, dispGaussY = null, dispGaussZ = null, dispVector = null;
	float xValue, yValue, zValue;
	double vector;
	Button view_data = null, take_data=null, back=null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.take_data_android);
		
		//setup variables to access internal magnetic sensor
		sensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
		magneticSensor = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
		
		//access display fields for x,y,z
		dispGaussX=(TextView)findViewById(R.id.x_gauss);
		dispGaussY=(TextView)findViewById(R.id.y_gauss);
		dispGaussZ=(TextView)findViewById(R.id.z_gauss);
		dispVector=(TextView)findViewById(R.id.vector);
		
		//access back button and set OnClickListener
		back=(Button)findViewById(R.id.back);
		back.setOnClickListener(this);
		}
	
	@Override
	protected void onResume()
	{
		super.onResume();
		//register magnetic sensor listener
		sensorManager.registerListener(this, magneticSensor, SensorManager.SENSOR_DELAY_NORMAL);
	}
	
	@Override
	protected void onPause()
	{
		super.onPause();
		//unregister magnetic sensor listener
		sensorManager.unregisterListener(this);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public void onAccuracyChanged(Sensor arg0, int arg1)
	{
		//Nothing to see here...
	}
	
	//converts uT to gauss since the default unit is the uT
	public float toGauss(float uT)
	{
		return (float) (uT * 0.01);
	}

	//accesses and displays the x,y,z values for the internal magnetic sensor of the android device
	@Override
	public void onSensorChanged(SensorEvent event){synchronized(this)
	{
		if(event.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD)
		{
			xValue = toGauss(event.values[0]);
			yValue = toGauss(event.values[1]);
			zValue = toGauss(event.values[2]);
			vector=Math.sqrt(Math.pow(xValue, 2) + Math.pow(yValue, 2) + Math.pow(zValue, 2));
			dispGaussX.setText(String.format("%.3f", xValue));
			dispGaussY.setText(String.format("%.3f", yValue));
			dispGaussZ.setText(String.format("%.3f", zValue));
			dispVector.setText(String.format("%.3f", vector));
		}
	}
	}

	//finishes the activity when the back button is pressed
	@Override
	public void onClick(View v)
	{
		Intent intent = new Intent();
		setResult(RESULT_OK, intent);
		finish();
	}
}
