package com.example.andesitev9;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.HashMap;
import xbee.explicit.*;
import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;

public class ProcessData extends Activity{
	
	private InputStreamReader dataFile, onlineXbees;
	private OutputStreamWriter processedFile, unknown, statistics;
	private File dirs, file;
	private FileInputStream inStream;
	private FileOutputStream outStream;
	private Process task = null;
	private EditText status = null;
	private String DATA_TAG = "DATA_TAG", DATA_LOG = "DATA_LOG", DATA_TRACKER = "DATA_TRACKER", processingError = "";
	private final static String RX_EXPLICIT = "91", SENSOR_READING = "01", NETWORK_TEST = "09", DONE_SENDING_DATA = "07";
	HashMap<String, DataTracker> dataTrackers = new HashMap<String, DataTracker>();
	
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.process_data);
		
		status = (EditText)findViewById(R.id.status);		
		
		dirs = new File("/sdcard/Andesite/");
		
		try {
			file = new File(dirs, "Online XBees");
			inStream = new FileInputStream(file);
			onlineXbees = new InputStreamReader(inStream);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		if(onlineXbees != null)
		{
			String line;
			BufferedReader read = new BufferedReader(onlineXbees);
			try {
				while((line = read.readLine()) != null)
				{
					String fileNi = line.substring(0, line.indexOf(","));
					String fileSl = line.substring(line.indexOf("," ) + 1);
					if(!dataTrackers.containsKey(fileSl))
					{
						dataTrackers.put(fileSl, new DataTracker(fileSl, fileNi, false));
					}
					
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		try {
			file = new File(dirs, "Processed Data");
			outStream = new FileOutputStream(file, true);
			processedFile = new OutputStreamWriter(outStream);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		try {
			file = new File(dirs, "Unknown Sensor Source");
			outStream = new FileOutputStream(file, true);
			unknown = new OutputStreamWriter(outStream);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		task = new Process(this);
		task.execute();
	}
	
	//called when the RotationAwareTask finishes its thread
	void markAsDone()
	{
		status.setText("Done.");
		/*Intent intent = new Intent(getApplicationContext(), MainActivity.class);
		startActivityForResult(intent, 0);*/
	}
	
	//AsyncTask class to run background thread to process the data
	private class Process extends AsyncTask<Void, Void, Void>
	{
		ProcessData activity = null;
		int progress = 0, goodput, throughput = 0, intPacketRun = 0, trackerLength;
		String val = "", timeStamp = "", frameType = "", ni = "", dataString = "", sl = "", latencyString = "",
				packetString = "", packetRun = "", routes = "", routeString = "", str = "", place = "not started yet",
				dataType = "", dataId = "", rxData = "";
		BufferedReader reader;
		StringBuilder buf;
		long routerNextMillis = 0L;
		DataTracker tracker;
		boolean earlierRun = false, checked = false;
		String[] trackers;
		
		Process(ProcessData activity)
		{
			attach(activity);
		}
		
		@Override
		//progress bar is set to reflect actual progress
		protected void onProgressUpdate(Void... unused)
		{
			if(activity != null)
			{
			}
		}
		
		@Override
		//processes the files in the background thread
		protected Void doInBackground(Void... params)
		{			
			for(int y = 0; y < 2; y++)
			{
				if(y == 0)
				{
					try {
						file = new File(dirs, "Data from Coordinator");
						inStream = new FileInputStream(file);
						dataFile = new InputStreamReader(inStream);
					} catch (FileNotFoundException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
				}
				else if(y == 1)
				{
					try {
						file = new File(dirs, "Data from Router");
						inStream = new FileInputStream(file);
						dataFile = new InputStreamReader(inStream);
					} catch (FileNotFoundException e1) {
						// TODO Auto-generated catch block
						e1.printStackTrace();
					}
				}
				
				trackers = new String[dataTrackers.size()];
				dataTrackers.keySet().toArray(trackers);
				Log.d(DATA_TAG, "keySet");
				
				trackerLength = trackers.length;
				for(int n = 0; n < trackerLength; n++)
				{
					tracker = dataTrackers.get(trackers[n]);
					tracker.setUsedStatus(false);
					dataTrackers.put(trackers[n], tracker);
				}
				
				try {
					if(dataFile != null)
					{
						reader = new BufferedReader(dataFile);
						buf = new StringBuilder();
						while((str = reader.readLine()) != null)
						{
							try {
								place = "new line";
								Log.d(DATA_TAG, place);
								buf.append(str);
								String data = str.substring(0, str.indexOf(","));
								Log.d(DATA_TAG, data);
								long coordMillis = Long.parseLong(str.substring(str.indexOf(",") + 1));
								ArrayList<Byte> bytes = new ArrayList<Byte>();
								int length = data.length();
								for(int x = 0; x < length; x += 2)
								{
									bytes.add((byte)Integer.parseInt(data.substring(x, x + 2), 16));
								}
							
								frameType = Common.getFrameType(bytes);
								place = "frame type";
								Log.d(DATA_TAG, place);
								
								if(frameType.equalsIgnoreCase(RX_EXPLICIT))
								{
									sl = RxResponse.getSl(bytes);
									place = "getRxSl";
									Log.d(DATA_TAG, place);
									
									packetString = "";
									place = "rx response";
									Log.d(DATA_TAG, place);
									dataType = RxResponse.getDataType(bytes);
									place = "getRxDataType";
									Log.d(DATA_TAG, place);
									packetRun = RxResponse.getRun(bytes);
									place = "getRxRun";
									Log.d(DATA_TAG, place);
									dataId = RxResponse.getDataId(bytes);
									place = "getRxDataId";
									Log.d(DATA_TAG, place);
									//Log.d(DATA_LOG, dataId);
									rxData = RxResponse.getData(bytes);
									place = "getRxData";
									Log.d(DATA_TAG, place);
									//data.append("Data Type: " + dataType + ", Data Id: " + dataId + ", Data: " + rxData + "\n");
								
									if(dataType.equalsIgnoreCase(SENSOR_READING) || dataType.equalsIgnoreCase(NETWORK_TEST))
									{
										tracker = dataTrackers.get(sl);
										place = "created tracker";
										Log.d(DATA_TAG, place);
										int intDataId = Integer.parseInt(dataId, 16);
										place = "parsed dataId";
										Log.d(DATA_TAG, place);
										intPacketRun = Integer.parseInt(packetRun, 16);
										checked = false;
										if(tracker.getRun() != intPacketRun)
												//tracker.getDataIdsSize() > 0xff && tracker.getLastDataId() > intDataId)
										{
											place = "tracker tracker run was not equal to intPacketRun";
											Log.d(DATA_TAG, place);
											
											if(intPacketRun < tracker.getRun())
											{
												earlierRun = true;
												String lookFor;
												if(intPacketRun < 10)
												{
													lookFor = "0" + intPacketRun + dataId;
												}
												else
												{
													lookFor = intPacketRun + dataId;
												}
												if(tracker.getMissingDataIds().contains(lookFor))
												{
													int removeIndex = tracker.getMissingDataIds().indexOf(lookFor);
													tracker.removeMissingDataId(removeIndex);
													Log.d(DATA_TAG, "removed " + lookFor + " from missingDataIds");
												}
												else
												{
													tracker.addExtraDataId(lookFor);
													Log.d(DATA_LOG, "added extra dataId: " + lookFor);
												}
											}
											else 
											{
												Log.d(DATA_LOG, "tracker run: " + tracker.getRun() + ", packet run: " + intPacketRun);
												String lookFor;
												ArrayList<String> temp = new ArrayList<String>();
												for(String t : tracker.getDataIds())
												{
													temp.add(t);
												}
												String checking = "";
												for(String t : temp)
												{
													checking += t;
												}
												Log.d(DATA_LOG, "temp: " + checking);
												for(int x = 1; x <= 0xff; x++)
												{
													lookFor = Integer.toHexString(x);
													if(lookFor.length() < 2)
													{
														lookFor = "0" + lookFor;
													}
														
													if(!temp.contains(lookFor))
													{
														int r = tracker.getRun();
														if(r < 10)
														{
															tracker.addMissingDataId("0" + r + lookFor);
															Log.d(DATA_LOG, "missing: " + r + lookFor);
														}
														else
														{
															tracker.addMissingDataId(r + lookFor);
															Log.d(DATA_LOG, "missing: " + r + lookFor);
														}
															
													}
													else
													{
														temp.remove(temp.indexOf(lookFor));
														if(temp.contains(lookFor))
														{
															int r = tracker.getRun();
															while(temp.contains(lookFor))
															{
																if(r < 10)
																{
																	tracker.addExtraDataId("0" + r + lookFor);
																}
																else
																{
																	tracker.addExtraDataId(r + lookFor);
																}
																Log.d(DATA_LOG, "added extra dataId: " + r + "" + lookFor);
																temp.remove(temp.indexOf(lookFor));
															}
														}
													}
												}
												tracker.clearDataIds();
												//tracker.addRunId(intPacketRun);
												tracker.setRun(intPacketRun);
												checked = true;
												Log.d(DATA_LOG, "cleared everything");
											}
										}
										place = "made it through loop";
										Log.d(DATA_TAG, place);
										
										if(!earlierRun)
										{
											tracker.addDataId(dataId);
										}
										earlierRun = false;
										tracker.setLastRun(intPacketRun);
										
										if(tracker.getRunIds().size() == 0)
										{
											tracker.addRunId(intPacketRun);
											tracker.setLastRun(intPacketRun);
										}
										
										if(!tracker.getRunIds().contains(intPacketRun))
										{
											tracker.addRunId(intPacketRun);
										}
										place = "setLastDataId";
										Log.d(DATA_TAG, place);
										dataTrackers.put(sl, tracker);
										place = "put new tracker in hashmap";
										Log.d(DATA_TAG, place);
										ni = tracker.getNi();
										place = "got ni from tracker";
										Log.d(DATA_TAG, place);
										place = "sensor reading";
										Log.d(DATA_TAG, place);
										val = RxResponse.getPotentiometerValue(rxData) + "";
										timeStamp = RxResponse.getTimeStamp(rxData);
										place = "got val and timeStamp";
										Log.d(DATA_TAG, place);
										processedFile.write("NI, " + ni + ", SL, " + sl + ", Coord Run, " + tracker.getRun() + 
												", Router Run, " + Integer.parseInt(packetRun, 16) + ", Data Id, " + dataId + ", " +
												"Potentiometer Value, " + val + ", " + "Timestamp, " + timeStamp);
										dataString = "NI, " + ni + ", SL, " + sl + ", Coord Run, " + tracker.getRun() + 
												", Router Run, " + Integer.parseInt(packetRun, 16) + ", Data Id, " + dataId + ", " +
												"Potentiometer Value, " + val + ", " + "Timestamp, " + timeStamp;
										if(dataType.equalsIgnoreCase(NETWORK_TEST))
										{
											place = "network test";
											Log.d(DATA_TAG, place);
											routerNextMillis = RxResponse.getMillis(rxData);
											long latency = coordMillis - routerNextMillis;
											latencyString = latency + "";
											place = "got millis";
											Log.d(DATA_TAG, place);
											processedFile.write(", Latency Millis, " + latency);
											dataString += ", Latency Millis: " + latency;
											processedFile.write(", Coord Millis, " + coordMillis + ", Router Millis, " + routerNextMillis);
											dataString += ", Coord Millis: " + coordMillis + ", Router Millis: " + routerNextMillis;
										}
										place = "write to processedFile";
										Log.d(DATA_TAG, place);
										processedFile.write("\n");
										dataString += "\n";
									}
								}
								else if(dataType.equals(DONE_SENDING_DATA))
								{
									throughput = Integer.parseInt(RxResponse.getThroughputRun(bytes), 16) * 0xff + 
											Integer.parseInt(RxResponse.getThroughputDataId(bytes), 16);
								}
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (Throwable t) {
								Log.d(DATA_TAG, "error");
								if(intPacketRun < 10)
								{
									tracker.addErrorDataId("0" + intPacketRun + dataId);
								}
								else
								{
									tracker.addErrorDataId(intPacketRun + dataId);
								}
								Log.d(DATA_TAG, "sent to error list");
							}
						}
						}
							trackers = new String[dataTrackers.size()];
							dataTrackers.keySet().toArray(trackers);
							Log.d(DATA_TAG, "keySet");
							
							trackerLength = trackers.length;
							for(int n = 0; n < trackerLength; n++)
							{
								tracker = dataTrackers.get(trackers[n]);
								Log.d(DATA_TAG, "got tracker");
								
								if(tracker.getUsedStatus())
								{
									
									String sl = tracker.getSl();
									String ni = tracker.getNi();
									
									Log.d(DATA_TAG, "got ni and sl: " + ni + sl);
									
									try {
										file = new File(dirs, "Statistics from " + ni + ", " + sl);
										outStream = new FileOutputStream(file, true);
										statistics = new OutputStreamWriter(outStream);
									} catch (FileNotFoundException e1) {
										// TODO Auto-generated catch block
										e1.printStackTrace();
									}
									
									for(int g : tracker.getRunIds())
									{
										Log.d(DATA_TAG, g + "");
									}
									for(String g : tracker.getDataIds())
									{
										Log.d(DATA_TAG, g + "");
									}
									Log.d(DATA_TAG, "highest run: " + tracker.getHighestRun());
									Log.d(DATA_TAG, "highest id: " + tracker.getHighestDataId());
									
									String lookFor;
									
									Log.d(DATA_TAG, "removed missing ids");
									
									int highestDataId = tracker.getHighestDataId();
									
									if(!checked)// && tracker.getLastRun() == tracker.getHighestRun())
									{
										for(int x = 1; x <= highestDataId; x++)
										{
			//								Log.d(DATA_TAG, "started for loop");
											lookFor = Integer.toHexString(x);
											if(lookFor.length() < 2)
											{
												lookFor = "0" + lookFor;
											}
			//								Log.d(DATA_TAG, "lookFor = " + lookFor);
			//								Log.d(DATA_TAG, "checking dataIds");
											if(!tracker.getDataIds().contains(lookFor))
											{
												int r = tracker.getHighestRun();
												if(r < 10)
												{
													tracker.addMissingDataId("0" + r + lookFor);
												}
												else
												{
													tracker.addMissingDataId(r + lookFor);
												}
												Log.d(DATA_LOG, "missing: " + r + lookFor);
											}
											else
											{
												//Log.d(DATA_TAG, "creating temp");
												ArrayList<String> temp = new ArrayList<String>();
												for(String t : tracker.getDataIds())
												{
													temp.add(t);
												}
												if(temp.indexOf(lookFor) != -1)
												{
			//										Log.d(DATA_TAG, temp.indexOf(lookFor) + "");
													temp.remove(temp.indexOf(lookFor));
			//										Log.d(DATA_TAG, "removed lookFor from temp");
			//										Log.d(DATA_TAG, temp.indexOf(lookFor) + "");
													while(temp.indexOf(lookFor) != -1)
													{
			//											Log.d(DATA_TAG, "in while loop");
														int r = tracker.getRun();
														if(intPacketRun < 10)
														{
															tracker.addExtraDataId("0" + r + lookFor);
														}
														else
														{
															tracker.addExtraDataId(r + lookFor);
														}
														Log.d(DATA_LOG, "added extra dataId: " + r + lookFor);
			//											Log.d(DATA_TAG, "about to remove lookFor from temp again");
														temp.remove(temp.indexOf(lookFor));
			//											Log.d(DATA_TAG, "removed lookFor from temp again");
													}
												}
											}
			//								Log.d(DATA_TAG, "end of loop");
										}
									
										Log.d(DATA_TAG, "found missing data");
									}
									
									for(int x = 1; x <= tracker.getHighestRun(); x++)
									{
										if(!tracker.getRunIds().contains(x))
										{
											tracker.addMissingRunId(x);
										}
									}
									
									Log.d(DATA_TAG, "found missing runs");
									Log.d(DATA_TAG, "throughput from packet: " + throughput);
									
									//if(throughput == 0)
									{
										throughput = ((tracker.getHighestRun() - 1) * 0xff + tracker.getHighestDataId());
									}
									Log.d(DATA_TAG, "calculated throughput: " + throughput);
									throughput += tracker.getExtraDataIdsSize();
									
									Log.d(DATA_TAG, "calculated throughput");
									Log.d(DATA_LOG, "calculations: ");
									Log.d(DATA_LOG, "highest run: " + tracker.getHighestRun() + "");
									Log.d(DATA_LOG, "highest id: " + Integer.toHexString(tracker.getHighestDataId()));
									Log.d(DATA_LOG, "extras size: " + tracker.getExtraDataIdsSize() + "");
									Log.d(DATA_LOG, "run ids: " );
									for(int f : tracker.getRunIds())
									{
										Log.d(DATA_LOG, f + "");
									}
									
									Log.d(DATA_TAG, "throughput = " + throughput);
									Log.d(DATA_TAG, "run = " + tracker.getHighestRun());
									Log.d(DATA_TAG, "lastDataId = " + tracker.getHighestDataId());
									try {
										statistics.write("Statistics from NI: " + ni + ", SL: " + sl + ":\n");
										statistics.write("Throughput: " + throughput);
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									
									goodput = throughput - tracker.getMissingDataIdsSize() - tracker.getExtraDataIdsSize() -
											(tracker.getMissingRunIdsSize() * 0xff) - tracker.getErrorDataIdsSize();
									
									
									Log.d(DATA_TAG, "errors size: " + tracker.getErrorDataIdsSize());
									Log.d(DATA_TAG, "missing ids size: " + tracker.getMissingDataIdsSize());
									Log.d(DATA_TAG, "missing run size: " + tracker.getMissingRunIdsSize());
									Log.d(DATA_TAG, "extra ids size: " + tracker.getExtraDataIdsSize());
									
									try {
										statistics.write("\nGoodput: " + goodput + " / " +
												throughput + " = " + (double)goodput / (double)throughput);
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									
									try {
										statistics.write("\n" + tracker.getErrorDataIdsSize() + " Packets with Errors:\n");
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									for(String p : tracker.getErrorDataIds())
									{
										try {
											statistics.write(p + ", ");
										} catch (IOException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
									}
									
									try {
										statistics.write("\n" + tracker.getMissingRunIdsSize() + " Missing Runs:\n");
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									for(Integer p : tracker.getMissingRunIds())
									{
										try {
											statistics.write(p + ", ");
										} catch (IOException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
									}							
									
									try {
										statistics.write("\n" + tracker.getMissingDataIdsSize() + " Missing Packets:\n");
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									for(String m : tracker.getMissingDataIds())
									{
										try {
											statistics.write(m + ", ");
										} catch (IOException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
									}
									
									try {
										statistics.write("\n" + tracker.getExtraDataIdsSize() + " Extra Packets:\n");
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									for(String s : tracker.getExtraDataIds())
									{
										try {
											statistics.write(s + ", ");
										} catch (IOException e) {
											// TODO Auto-generated catch block
											e.printStackTrace();
										}
									}
									
									try {
										dataFile.close();
									} catch (IOException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
									Log.d(DATA_TAG, "closed dataFile");
									
									try {
										statistics.close();
									} catch (IOException e) {
										e.printStackTrace();
									}
									Log.d(DATA_TAG, "closed statistics file");
								}
							}
					}catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (Throwable t) {
						Log.d(DATA_TAG, "error");
						Log.d(DATA_TAG, t.toString());
						try {
							statistics.write("\n Calculation Error: " + t.toString());
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
			}
			
			try {
				processedFile.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			Log.d(DATA_TAG, "closed processedFile");
			
			try {
				unknown.close();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			Log.d(DATA_TAG, "closed unknown");
			
			try {
				onlineXbees.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
			Log.d(DATA_TAG, "closed online xbees");
			publishProgress();
			return null;
		}
		
		@Override
		//this method is called when doInBackground finishes
		protected void onPostExecute(Void unused)
		{
			if(activity != null)
			{
				markAsDone();
			}
		}
		
		//removes the activity reference
		void detach()
		{
			activity = null;
		}
		
		//sets activity to reference a new activity
		void attach(ProcessData activity)
		{
			this.activity = activity;
		}
		
		//returns how much progress has been made
		int getProgress()
		{
			return progress;
		}
		
	}

}
