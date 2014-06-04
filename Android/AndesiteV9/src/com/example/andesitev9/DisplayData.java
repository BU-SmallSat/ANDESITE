package com.example.andesitev9;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;

import android.app.Activity;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.view.View.OnClickListener;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.Toast;

public class DisplayData extends Activity
{
	private EditText data = null;
	private String selectedFile = null;
	private InputStream in = null;
	private RotationAwareTask task = null;
	private StringBuilder buf;
	File dirs;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.display_data);
		data = (EditText)findViewById(R.id.data);
		selectedFile = getIntent().getStringExtra("fileName");
		buf = new StringBuilder();
		dirs = new File("/sdcard/Andesite/");
		
		//bar=(ProgressBar)findViewById(R.id.progress);
		task=(RotationAwareTask)getLastNonConfigurationInstance();
		
		if(task==null)
		{
			task = new RotationAwareTask(this);
			task.execute();
		}
		else
		{
			task.attach(this);
			//bar.setProgress(task.getProgress());
			//updateProgress(task.getProgress());
			
			if(task.getProgress() >= 100)
			{
				markAsDone();
			}
		}
	}
	
	//displays String str in the EditText field
	private void displayData(final String str) {
        runOnUiThread(new Runnable() {
                @Override
                    public void run() {
                	if(str.length() > 0)
                	{
                		data.setText(str);
                	}
                }
                });
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
	}
			
			@SuppressWarnings("deprecation")
			@Override
			public Object onRetainNonConfigurationInstance()
			{
				task.detach();
				return(task);
			}
			
			void markAsDone()
			{
				displayData(buf.toString());
				//findViewById(R.id.complete).setVisibility(View.VISIBLE);
			}
			
			private class RotationAwareTask extends AsyncTask<Void, Void, Void>
			{
				DisplayData activity = null;
				int progress = 0;
				ArrayList<Integer> data = new ArrayList<Integer>();
				ArrayList<Integer> packet = new ArrayList<Integer>();
				private InputStreamReader in;
				private InputStreamReader tmp;
				private BufferedReader reader;
				private String str;
				private OutputStreamWriter output;//dataToZero, dataToOne;
				private String zeroSL = "4078efb8", oneSL = "409bb9e7";
				private int fileNum = 1, start;
				private String filename = "Received Data " + Integer.toString(fileNum);
				private boolean moreFiles = true;
				
				RotationAwareTask(DisplayData activity)
				{
					attach(activity);
				}
				
				@Override
				protected Void doInBackground(Void... unused)
				{
					try
					{
						File file = new File(dirs, selectedFile);
						FileInputStream os = new FileInputStream(file);
						in = new InputStreamReader(os);
						//in = openFileInput(selectedFile);
						
						if(in != null)
						{
							//InputStreamReader tmp = new InputStreamReader(in);
							BufferedReader reader = new BufferedReader(in);
							String str;
							//StringBuilder buf = new StringBuilder();
							
							while((str = reader.readLine()) != null)
							{
								buf.append(str+"\n");
							}
							in.close();
							//displayData(buf.toString());
						}
					}
					catch(java.io.FileNotFoundException e){
						//probably just hasn't been created yet
					}
					catch(Throwable t){
						//Toast.makeText(this, "Exception: "+t.toString(), Toast.LENGTH_LONG).show();
					}
					
					return null;
				}
				
				@Override
				protected void onProgressUpdate(Void... unused)
				{
					if(activity != null)
					{
						progress = (int)Math.round((double)fileNum / 54.0 * 100.0);
						//bar.setProgress(progress);
						//activity.updateProgress(progress);
					}
				}
				
				@Override
				protected void onPostExecute(Void unused)
				{
					if(activity != null)
					{
						activity.markAsDone();
					}
				}
				
				void detach()
				{
					activity = null;
				}
				
				void attach(DisplayData activity)
				{
					this.activity = activity;
				}
				
				int getProgress()
				{
					return progress;
				}
				
			}

		}
