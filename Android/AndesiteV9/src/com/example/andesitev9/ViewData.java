package com.example.andesitev9;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.util.SparseBooleanArray;
import android.view.View.OnClickListener;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

public class ViewData extends Activity implements OnClickListener
{
	private Button delete_files = null;
	private ArrayList<String> fileNames = null;
	private ListView listView = null;
	File dirs;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.view_data);
		delete_files=(Button)findViewById(R.id.delete_files);
		delete_files.setOnClickListener(this);
		fileNames = new ArrayList<String>();
		listView = (ListView) findViewById(R.id.file_list);
		
		// Directory path here
		String path = getBaseContext().getFilesDir().getPath();
		path = getBaseContext().getExternalFilesDir(null).getPath();
		//path = Environment.getExternalStorageDirectory().getPath();
		dirs = new File("/sdcard/Andesite/");
		
		//files from directory path are put into array
		String files;
		//File folder = new File(path);
		File[] listOfFiles = dirs.listFiles(); 
		 
		//filenames are copied into ArrayList<String>
		for (int i = 0; i < listOfFiles.length; i++) 
		{
			if (listOfFiles[i].isFile()) 
			{
				files = listOfFiles[i].getName();
				fileNames.add(files);
			}
		}
		
		//ArrayAdapter is used to display filenames in a list so the can be selected
		//name of file is put into intent to DisplayData.class can access which file was selected
		final ArrayAdapter adapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, fileNames);
		//listView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE);
		listView.setAdapter(adapter);
		
		listView.setOnItemClickListener(new OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
            	String selectedFile = fileNames.get(position);            	
                     Intent myIntent = new Intent(view.getContext(), DisplayData.class);
                     myIntent.putExtra("fileName", selectedFile);
                     startActivity(myIntent);
                   }
                 });
	}
	
	//This method ensures that when the user returns to the activity,
	//the list of available files has been updated
	@Override
	public void onResume()
	{
		super.onResume();
		
		fileNames = new ArrayList<String>();
		listView = (ListView) findViewById(R.id.file_list);
		
		// Directory path here
		String path = getBaseContext().getFilesDir().getPath();
		dirs = new File("/sdcard/Andesite/");
		 
		//files from directory path are put into array
		String files;
		//File folder = new File(path);
		File[] listOfFiles = dirs.listFiles(); 
		 
		//filenames are copied into ArrayList<String>
		for (int i = 0; i < listOfFiles.length; i++) 
		{
			if (listOfFiles[i].isFile()) 
			{
				files = listOfFiles[i].getName();
				fileNames.add(files);
			}
		}
		
		//ArrayAdapter is used to display filenames in a list so the can be selected
		//name of file is put into intent to DisplayData.class can access which file was selected
		final ArrayAdapter adapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, fileNames);
		//listView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE);
		listView.setAdapter(adapter);
	}
	
	//sends the user to DeleteFiles.java when the delete_files button is pressed
	@Override
	public void onClick(View v)
	{		
		if(v==delete_files)
		{
			Intent myIntent = new Intent(v.getContext(),DeleteFiles.class);
			myIntent.putStringArrayListExtra("fileNames",fileNames);
			startActivityForResult(myIntent,0);
		}
	}
	
	/*private void displayFilesToDelete()
	{
		final AlertDialog.Builder alert = new AlertDialog.Builder(this);

		alert.setTitle("Select File(s) to Delete");
		//alert.setMessage("Please choose a filename for this data.");

		// Set an EditText view to get user input 
		final EditText text = new EditText(this);
		alert.setView(text);
		alert.setCancelable(false);

		alert.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
		public void onClick(DialogInterface dialog, int whichButton) {
			String value = text.getText().toString();		  
		}
		});
		

		/*alert.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
		  public void onClick(DialogInterface dialog, int whichButton) {
			  //fileExists = false;
			  finish();
		  }
		});*/
		
		/*alert.setNegativeButton("Cancel",null);

		alert.show();
	}*/
	
	/*private void deleteFiles(String fileName)
	{
		
	}*/
}
