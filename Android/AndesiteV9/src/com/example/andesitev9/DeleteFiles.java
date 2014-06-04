package com.example.andesitev9;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

import android.annotation.SuppressLint;
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
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

@SuppressLint("NewApi")
public class DeleteFiles extends Activity
{
	private Button delete_files = null;
	private ArrayList<String> fileNames = null;
	private ListView listView = null;
	File dirs;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.delete_files);
		delete_files=(Button)findViewById(R.id.delete_files);
		//delete_files.setOnClickListener(this);
		fileNames = new ArrayList<String>();
		listView = (ListView) findViewById(R.id.list);
		
		//create an intent and get the ArrayList<String> of filenames from it
		Intent intent = getIntent();
		fileNames = intent.getStringArrayListExtra("fileNames");
		
		//ArrayAdapter is used to display filenames in a list so that multiple files can be selected
		final ArrayAdapter adapter = new ArrayAdapter(this, android.R.layout.simple_list_item_multiple_choice, fileNames);
		listView.setChoiceMode(ListView.CHOICE_MODE_MULTIPLE);
		listView.setAdapter(adapter);
		
		//selected files are deleted when the delete_files button is pressed
		//the ArrayAdapter is updated to reflect which files are still available for deletion
		delete_files.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v){
				int length = listView.getCount();
				SparseBooleanArray checkedFiles = listView.getCheckedItemPositions();
				for(int x = 0; x < length; x++)
				{
					if(checkedFiles.get(x))
					{
						String item = fileNames.get(x);
						//File dir = getBaseContext().getFilesDir();
						//dir = getBaseContext().getExternalFilesDir(null);
						dirs = new File("/sdcard/Andesite/");
						File f = new File(dirs, item);
						f.delete();
						fileNames.remove(x);
						//adapter.remove(x);
						adapter.notifyDataSetChanged();
					}
					
				}
				adapter.notifyDataSetChanged();
			}
		});
	}
}
