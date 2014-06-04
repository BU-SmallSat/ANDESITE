package com.example.andesitev9;

import java.io.File;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
//import java.util.*;
import android.widget.*;
import android.content.Intent;

public class MainActivity extends Activity implements OnClickListener{
	//create TextView and Button variables for the display
	TextView dispGaussX = null, dispGaussY = null, dispGaussZ = null;
	Button view_data = null, xbee_query = null,
			network_test= null, process = null, tester = null;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		//access the buttons on the display and set the OnClickListener for those buttons
		view_data=(Button)findViewById(R.id.view_data);
		view_data.setOnClickListener(this);
		xbee_query=(Button)findViewById(R.id.xbee_query);
		xbee_query.setOnClickListener(this);
		network_test=(Button)findViewById(R.id.network_test);
		network_test.setOnClickListener(this);
		process=(Button)findViewById(R.id.process);
		process.setOnClickListener(this);
		tester=(Button)findViewById(R.id.tester);
		tester.setOnClickListener(this);
		
		File f = new File("/sdcard/Andesite/");
		if(!f.exists()){
			f.mkdir();
		}
		}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	//implements OnClickListener interface
	@Override
	public void onClick(View v)
	{	
		//start view data activity if take data from ioio button is pressed
		if(v==view_data)
		{
			Intent myIntent = new Intent(v.getContext(),ViewData.class);
			startActivityForResult(myIntent,0);
		}
		
		if(v==xbee_query)
		{
			Intent myIntent = new Intent(v.getContext(), XbeeQuery.class);
			startActivityForResult(myIntent,0);
		}
		
		if(v==network_test)
		{
			Intent myIntent = new Intent(v.getContext(), NetworkTestV2.class);
			startActivityForResult(myIntent,0);
		}
		
		if(v==process)
		{
			Intent myIntent = new Intent(v.getContext(), ProcessData.class);
			startActivityForResult(myIntent,0);
		}
		
		if(v==tester)
		{
			Intent myIntent = new Intent(v.getContext(), Tester.class);
			startActivityForResult(myIntent,0);
		}
	}
}
