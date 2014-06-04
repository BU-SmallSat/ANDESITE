package com.example.andesitev9;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.HashMap;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.os.SystemClock;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import ioio.lib.api.AnalogInput;
import ioio.lib.api.IOIO;
import ioio.lib.api.IOIOFactory;
import ioio.lib.api.Uart;
import ioio.lib.api.exception.ConnectionLostException;
import ioio.lib.api.exception.IncompatibilityException;
import ioio.lib.util.BaseIOIOLooper;
import ioio.lib.util.IOIOLooper;
import ioio.lib.util.android.IOIOActivity;
import xbee.explicit.*;

public class NetworkTestV2 extends IOIOActivity{
	
	private Uart uartCoord = null, uartRouter;
	private OutputStream outCoord = null, outRouter = null;
	private InputStream inCoord= null, inRouter = null;
	private OutputStreamWriter  coordFile, routerFile, onlineXbees, routingTables, modemFile, unknown;
	private EditText current_xbees, status;
	private static final String REMOTE_COMMAND_RESPONSE = "97", RX_RESPONSE = "90", LOCAL_COMMAND_RESPONSE = "88",
			SENSOR_READING = "01", MILLIS_VALUE = "08", NETWORK_TEST = "09", RX_EXPLICIT = "91", ROUTE_RESPONSE = "8032",
			ADDRESS_RESPONSE = "8001", MODEM_STATUS = "8A", SECOND_RADIO_STRING = "10", DONE_SENDING_DATA = "07",
			IN_SYNC_STRING = "11", SET_SECOND_RADIO_RECEIVED = "14", SET_DEST_RADIO_RECEIVED = "17",
			NI_COORD = "C", NI_ROUTER_ZERO = "R0", NI_ROUTER_ONE = "R1", NI_ROUTER_TWO = "R2",
			BROADCAST = "Broadcast", REMOTE_STATUS_OK = "00", DATA_TAG = "DATA_TAG", DATA_LOG = "DATA_LOG",
			PACKET_LOG = "PACKET_LOG", MILLIS_VALUE_RESPONSE = "18";
	private static final int SEND_DATA_COMMAND = 0x2, STOP_DATA_COMMAND = 0x3, SECOND_RADIO = 0x10,
			ADDRESS_INFO = 0X12, IN_SYNC = 0x11, DEST_RADIO = 0x13, START_OVER = 0x15, MILLIS_VALUE_REQUEST = 0x8;
	private HashMap<String, XbeeInfo> sensorNodes = new HashMap<String, XbeeInfo>();
	private XbeeInfo xbeeBroadcast, xbeeCoord;
	private int shBroadcast = 0x0, slBroadcast = 0x0000FFFF, myBroadcast = 0xFFFE, shCoord = 0x0, slCoord = 0x0,
			remoteCommandOptions = 0x02, sentToAsync = 0, processAtAsync = 0, frameId = 0x01;
	private ArrayList<Byte> coordBytes, routerBytes;
	private boolean ioioReady = false, coordStart = false, discoverNodes = true, foundXbee = false, firstXbee = true,
			secondStatus = false, updatedAddress = false, secondRadioSent = false, readyToResetClock = false, sentCommand = false,
			retrieveClocks = false, allClocksInSync = false;
	private long coordNextMillis = 0, coordStartMillis = 0, tenMinutes = 600000, routesStartMinutes = 0L,
			niStartMinutes = 0L, passedMinutes = 0L, fiveMinutes = 300000, oneMinute = 60000, fourAndHalfMinutes = 270000,
			thirtySeconds = 30000, startMillis = 0L, tenSeconds = 10000;
	private String ni = "", secondRadioSl = null, xbeeString = "", dataType, dataId, rxData, place = "";
	private Communicate task = null;
	private char[] niChar = {'N', 'I'};
	private HashMap<String, StatusTracker> statusTrackers = new HashMap<String, StatusTracker>();
	private StatusTracker tracker, secondRadio = null;
	private final int ANALOG_SENSOR_PIN = 41;
	private AnalogInput analogInput = null;
	private float anInput;
	private File file, dirs;
	private FileOutputStream os;
	private int[] secondRadioPacket, destRadioPacket, requestNi;
	
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.network_test_2);
		current_xbees = (EditText)findViewById(R.id.current_xbees);
		status = (EditText)findViewById(R.id.status);
		
		coordBytes = new ArrayList<Byte>();
		routerBytes = new ArrayList<Byte>();
		
		//create XbeeInfo object with broadcast addresses
		xbeeBroadcast = new XbeeInfo(shBroadcast, slBroadcast, myBroadcast, BROADCAST);
		xbeeCoord = new XbeeInfo(shCoord, slCoord, myBroadcast, NI_COORD);
				
		requestNi = AtRemoteRequest.makePacket(xbeeBroadcast, frameId, remoteCommandOptions, niChar);
		
		secondRadioPacket = TxRequest.makePacket(xbeeCoord, 0x1, SECOND_RADIO);
		
		dirs = new File("/sdcard/Andesite/");
		
		try {
			file = new File(dirs, "Data from Coordinator");
			os = new FileOutputStream(file, true);
			coordFile = new OutputStreamWriter(os);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		try {
			file = new File(dirs, "Data from Router");
			os = new FileOutputStream(file, true);
			routerFile = new OutputStreamWriter(os);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		Log.d(DATA_TAG, "created dataFile");
		
		try {
			file = new File(dirs, "Online XBees");
			os = new FileOutputStream(file, true);
			onlineXbees = new OutputStreamWriter(os);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		Log.d(DATA_TAG, "created onlineXbees");
		
		try {
			file = new File(dirs, "Routing Tables");
			os = new FileOutputStream(file, true);
			routingTables = new OutputStreamWriter(os);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		Log.d(DATA_TAG, "created routingTables");
		
		try {
			file = new File(dirs, "Modem Status Updates");
			os = new FileOutputStream(file, true);
			modemFile = new OutputStreamWriter(os);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		Log.d(DATA_TAG, "created modemFile");
		
		try {
			file = new File(dirs, "Unknown Packet");
			os = new FileOutputStream(file, true);
			unknown = new OutputStreamWriter(os);
		} catch (FileNotFoundException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		Log.d(DATA_TAG, "created unknown file");
		
		//creates new task if there previously wasn't one
		if(task==null)
		{
			//task = new Communicate(this);
		}
		else
		{
			//if screen orientation was changed, causing a the previous activity to be
			//destroyed and new one to be created, task uses attach() to reference
			//the new activity
			//task.attach(this);
			//bar.setProgress(task.getProgress());
			//updateProgress(task.getProgress());
					
			/*if(task.getProgress() >= 100)
			{
				markAsDone();
			}*/
		}
	}
	
	@Override
	protected void onDestroy() 
	{
		super.onDestroy();
		
		runOnUiThread(new Runnable(){
			public void run()
			{
				Log.d(DATA_TAG, "runOnUiThread");
				
				try {
					coordFile.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
				try {
					routerFile.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
				try {
					onlineXbees.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
				try {
					modemFile.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
				
				try {
					routingTables.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				
				try {
					unknown.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
		
				try {
					if(inCoord != null)
					{
						inCoord.close();
					}
					Log.d(DATA_TAG, "closed in");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				try {
					if(inRouter != null)
					{
						inRouter.close();
					}
					Log.d(DATA_TAG, "closed in");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
		
				try {
					if(outCoord != null)
					{
						outCoord.close();
					}
					Log.d(DATA_TAG, "closed out");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				try {
					if(outRouter != null)
					{
						outRouter.close();
					}
					Log.d(DATA_TAG, "closed out");
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				
				if(analogInput != null)
				{
					try {
						analogInput.close();
					} finally {
				}
				}
				Log.d(DATA_TAG, "closed analogInput");
		
				if(uartCoord != null)
				{
					try {
						uartCoord.close();
					} finally {
					}
				}
				
				if(uartRouter != null)
				{
					try {
						uartRouter.close();
					} finally {
					}
				}
				Log.d(DATA_TAG, "closed uart");
			}
		});
	}
	
	@SuppressWarnings("deprecation")
	@Override
	//stops RotationAwareTask from referencing current activity
	public Object onRetainNonConfigurationInstance()
	{
		//task.detach();
		return(task);
	}
	
	//called when the RotationAwareTask finishes its thread
	void markAsDone()
	{
	}
	
	//AsyncTask class to run background thread to process the data
	private class Communicate extends AsyncTask<Byte[], Void, Void>
	{
		NetworkTestV2.Looper activity = null;
		String frameType = "", sl = "", packetString = "", routes = "", routeString = "", dataString = "";
		
		Communicate(NetworkTestV2.Looper activity)
		{
			attach(activity);
		}
		
		@Override
		//progress bar is set to reflect actual progress
		protected void onProgressUpdate(Void... unused)
		{
			if(activity != null)
			{
				//data.setText(dataString);
				current_xbees.setText(xbeeString);
				//no_frame.setText(packetString);
				//routing_entries.append(routeString);
			}
		}
		
		@Override
		//processes the files in the background thread
		protected Void doInBackground(Byte[]... params)
		{
			Byte[] receivedArray = (Byte[])params[0];
			ArrayList<Byte> received = new ArrayList<Byte>(receivedArray.length);
			for(Byte b : receivedArray)
			{
				received.add(b);
				packetString += Common.byteToHex(b) + ", ";
			}
			packetString += "\n";
			
			try{
				frameType = Common.getFrameType(received);
				place = "frame type";
				Log.d(DATA_TAG, place);
				Log.d(DATA_TAG, frameType);
				if(frameType.equalsIgnoreCase(REMOTE_COMMAND_RESPONSE))
				{
					String commandStatus = AtRemoteResponse.getStatus(received);
					Log.d(DATA_TAG, "Status = " + commandStatus);
					Log.d(DATA_TAG, "Constant = " + REMOTE_STATUS_OK);
					if(commandStatus.equalsIgnoreCase(REMOTE_STATUS_OK))
					{
						packetString = "";
						place = "remote_command_response";
						String at = AtRemoteResponse.getCommand(received);
						String parameters = AtRemoteResponse.getData(received);
						dataString = "Remote AT Command: " + at + ", Command Data: " + parameters + "\n";
						if(at.equalsIgnoreCase("NI"))
						{
							sl = AtRemoteResponse.getSl(received);
							String sh = AtRemoteResponse.getSh(received);
							String my = AtRemoteResponse.getMy(received);
							String ni = AtRemoteResponse.getNi(received);
							if(sensorNodes.containsKey(sl))
							{
								XbeeInfo xbeeInfo = sensorNodes.get(sl);
								if(!my.equalsIgnoreCase(Integer.toHexString(xbeeInfo.getMy())))
								{
									xbeeInfo.setMy(Integer.parseInt(my, 16));
									sensorNodes.put(sl, xbeeInfo);
								}
//								if(!secondStatusNi)
//								{
//									DataTracker dTracker = dataTrackers.get(sl);
//									dTracker.setSecondStatus(true);
//									dataTrackers.put(sl, dTracker);
//									secondStatusNi = true;
//									xbeeString += "NI: " + ni + ",  SL: " + sl + ", MY: " + my + "\n";
//									onlineXbees.write(ni + "," + sl + "\n");
//								}
							}
							else
							{
								sensorNodes.put(sl, new XbeeInfo(sh, sl, my, ni));
								statusTrackers.put(sl, new StatusTracker(sl, ni, false));
								xbeeString += "NI: " + ni + ",  SL: " + sl + ", MY: " + my + "\n";
								onlineXbees.write(ni + "," + sl + "\n");
								foundXbee = true;
								if(secondRadioSl != null && sl.equalsIgnoreCase(secondRadioSl))// && !secondStatusNi)
								{
									StatusTracker dTracker = statusTrackers.get(sl);
									dTracker.setSecondStatus(true);
									statusTrackers.put(sl, dTracker);
									secondStatus = true;
									Log.d(DATA_LOG, "set second radio status to true for: " + sl);
									statusTrackers.remove(sl);
									secondRadio = new StatusTracker(sl, ni, false);
								}
								xbeeString += "Status: " + statusTrackers.get(sl).getSecondStatus() + "\n";
							}
//							if(secondRadioSl != null && sl.equalsIgnoreCase(secondRadioSl) && !secondStatusNi)
//							{
//								DataTracker dTracker = dataTrackers.get(sl);
//								dTracker.setSecondStatus(true);
//								dataTrackers.put(sl, dTracker);
//								secondStatusNi = true;
//							}
						}
					}
					else
					{
						discoverNodes = true;
					}
				}
				else if(frameType.equalsIgnoreCase(LOCAL_COMMAND_RESPONSE))
				{
					packetString = "";
					place = "local command response";
					String at = AtLocalResponse.getCommand(received);
					String parameters = AtLocalResponse.getData(received);
					dataString = "Local AT Command: " + at + ", Command Data: " + parameters + "\n";
				}
				else if(frameType.equalsIgnoreCase(MODEM_STATUS))
				{
					packetString = "";
					long modemTime = System.currentTimeMillis();
					String modemStatus = ModemStatus.getStatus(received, modemTime);
					modemFile.write(modemStatus);
				}
				else if(frameType.equalsIgnoreCase(RX_EXPLICIT))
				{
					packetString = "";
					String clusterId = RxResponse.getClusterId(received);
					String dataType = RxResponse.getDataType(received);
					Log.d(DATA_TAG, "clusterId: " + clusterId);
					if(clusterId.equals(ROUTE_RESPONSE))
					{
						sl = RxResponse.getSl(received);
						place = "getRxSl";
						Log.d(DATA_TAG, place);
						packetString = "";
						if(statusTrackers.get(sl) != null)
						{
							ni = statusTrackers.get(sl).getNi();
						}
						else
						{
							ni = "C";
						}
						
					
						routes = RxResponse.getRoutingTableEntries(received);
						place = "got routes";
						Log.d(DATA_TAG, place);
						routeString += "NI: " + ni + ", SL: " + sl + ", Entries: " + routes + "\n";
						routingTables.write(routeString);
					}
					else if(dataType.equals(SECOND_RADIO_STRING))
					{
						sl = RxResponse.getSl(received);
						String my = RxResponse.getMy(received);
						String sh = RxResponse.getSh(received);
						secondRadioSl = sl;
						/*String[] trackers = new String[dataTrackers.size()];
						dataTrackers.keySet().toArray(trackers);*/
						if(statusTrackers.containsKey(sl))
						{
							StatusTracker dTracker = statusTrackers.get(sl);
							String ni = dTracker.getNi();
							dTracker.setSecondStatus(true);
							statusTrackers.put(sl, dTracker);
							secondStatus = true;
							statusTrackers.remove(sl);
							secondRadio = new StatusTracker(sl, ni, false);
						}
						xbeeString += "SECOND RADIO: " + sl + ", MY: " + my + "\n";
					}
					else if(dataType.equals(SET_SECOND_RADIO_RECEIVED))
					{
						sl = RxResponse.getSl(received);
						if(statusTrackers.containsKey(sl))
						{
							StatusTracker sTracker = statusTrackers.get(sl);
							sTracker.setSecondRadioStatus(true);
							statusTrackers.put(sl, sTracker);
						}
					}
					else if(dataType.equals(SET_DEST_RADIO_RECEIVED))
					{
						sl = RxResponse.getSl(received);
						if(statusTrackers.containsKey(sl))
						{
							StatusTracker sTracker = statusTrackers.get(sl);
							sTracker.setDestRadioStatus(true);
							statusTrackers.put(sl, sTracker);
						}
					}
					else if(dataType.equals(MILLIS_VALUE_RESPONSE))
					{
						sl = RxResponse.getSl(received);
						if(statusTrackers.containsKey(sl))
						{
							StatusTracker sTracker = statusTrackers.get(sl);
							if(RxResponse.getFirstDataByte(received).equals("01"))
							{
								sTracker.setClockStatus(true);
							}
							else if(RxResponse.getFirstDataByte(received).equals("02"))
							{
								sTracker.setClockStatus(false);
							}
							statusTrackers.put(sl, sTracker);
						}
					}
					
				}
				else
				{
					for(byte r : received)
					{
						unknown.write(Common.byteToHex(r));
					}
					unknown.write("\n");
				}
		}catch(Throwable t){
			
		}
			received.clear();
			place = "cleared bytes";
			Log.d(DATA_TAG, place);
			publishProgress();
			processAtAsync++;
			return null;
		}
		
		@Override
		//this method is called when doInBackground finishes
		protected void onPostExecute(Void unused)
		{
			if(activity != null)
			{
				//activity.markAsDone();
			}
		}
		
		//removes the activity reference
		void detach()
		{
			activity = null;
		}
		
		//sets activity to reference a new activity
		void attach(NetworkTestV2.Looper activity)
		{
			this.activity = activity;
		}
		
	}
	
class Looper extends BaseIOIOLooper{
		
	protected void setup() throws ConnectionLostException, InterruptedException{
		//uart uses pins 3 and 4 and baud rate of 19200
		uartCoord = ioio_.openUart(3, 4, 19200
				, Uart.Parity.NONE, Uart.StopBits.ONE);
		
		uartRouter = ioio_.openUart(5, 6, 19200, Uart.Parity.NONE, Uart.StopBits.ONE);
		
		//get the inputstream and outputstream from the uart
		inCoord = uartCoord.getInputStream();
		inRouter = uartRouter.getInputStream();
	
		outCoord = uartCoord.getOutputStream();
		outRouter = uartRouter.getOutputStream();
		
		//open the analog input pin to received voltage
		analogInput = ioio_.openAnalogInput(ANALOG_SENSOR_PIN);
		
		try {
			for(int s : secondRadioPacket)
			{
				outRouter.write((byte)s);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		SystemClock.sleep(50);
		
		ioioReady = true;
		startMillis = System.currentTimeMillis();
		
		Log.d(DATA_TAG, "IOIO started");
	}
		
	public void loop() throws ConnectionLostException, InterruptedException
	{		
		if(discoverNodes)
		{
			Log.d(DATA_LOG, "checking for nodes");
			checkForNodes();
			discoverNodes = false;
		}
		
		if(foundXbee == false && System.currentTimeMillis() - startMillis > tenSeconds)
		{
			discoverNodes = true;
		}
		
		if(!updatedAddress)
		{
			Log.d(DATA_TAG, "sent: " + sentToAsync + ", processed: " + processAtAsync);
		}
		
		if(secondStatus && !updatedAddress && processAtAsync == sentToAsync
				&& System.currentTimeMillis() - startMillis > tenSeconds && foundXbee)
		{
			Log.d(DATA_LOG, "sent: " + sentToAsync + ", process: " + processAtAsync);
			updateSensorsSecond();
			updatedAddress = true;
			setStatusText("Updating Sensors.");
			chooseDest();
		}
		
		if(updatedAddress && allDestSet() && allSecondSet() && !readyToResetClock)
		{
			Log.d(DATA_LOG, "can reset clock now");
			setStatusText("Reset clock now.");
			readyToResetClock = true;
		}
		
		if(coordStart && readyToResetClock && !retrieveClocks)
		{
			Log.d(DATA_LOG, "verifying all clocks are in sync");
			setStatusText("Verifying all clocks are in sync.");
			retrieveClockStatus();
			retrieveClocks = true;
		}
		
		/*if(allClocksReset() && !allClocksInSync && retrieveClocks)
		{
			Log.d(DATA_LOG, "all clocks in sync");
			setStatusText("All clocks are in sync.");
			allClocksInSync = true;
		}*/
		
		if(coordStart && retrieveClocks && !sentCommand && allClocksReset())
		{
			getRoutes(xbeeBroadcast);
			sendData();
			if(firstXbee)
			{
				routesStartMinutes = System.currentTimeMillis();
				niStartMinutes = System.currentTimeMillis();
				firstXbee = false;
			}
			setStatusText("Transmitted send command.");
			Log.d(DATA_LOG, "starting data-dumping");
			sentCommand = true;
		}
		
		passedMinutes = System.currentTimeMillis() - routesStartMinutes;
		if(passedMinutes >= fiveMinutes)
		{
			routesStartMinutes = System.currentTimeMillis();
			//getRoutes(xbeeBroadcast);
		}
		
		passedMinutes = System.currentTimeMillis() - niStartMinutes;
		if(passedMinutes >= fourAndHalfMinutes)
		{
			niStartMinutes = System.currentTimeMillis();
			//checkForNodes();
		}
		
		if(!coordStart)
		{
			resetClock();
		}
		try {
			int availableBytesCoord = inCoord.available();
			int availableBytesRouter = inRouter.available();
			byte[] coordBuff = new byte[availableBytesCoord];
			byte[] routerBuff = new byte[availableBytesRouter];			
			inCoord.read(coordBuff);
			inRouter.read(routerBuff);
			if(availableBytesCoord > 0)
			{
//				Log.d(DATA_LOG,""+availableBytes);
//				Log.d(DATA_LOG,"STARTING");
				for(byte b : coordBuff)
				{
					place = "for b";
//					Log.d(DATA_TAG, place);
					/*Log.d(DATA_TAG, "Lengths: " + Common.calcLength(receivedBytes) + ", "
							+ Integer.parseInt(Common.getPacketLength(receivedBytes), 16) + "\nSums: " +
							Common.byteToHex(Common.calcChecksum(receivedBytes)) + ", " + 
							Common.getPacketChecksum(receivedBytes));*/
					
					if(coordBytes.size() > 4 && Common.byteToHex(b).equals("7e") && 
						Common.calcLength(coordBytes) == Integer.parseInt(Common.getPacketLength(coordBytes), 16) &&
						Common.byteToHex(Common.calcChecksum(coordBytes)).equals(Common.getPacketChecksum(coordBytes)))
					{
						if(Common.getFrameType(coordBytes).equals(RX_EXPLICIT) && 
									(RxResponse.getClusterId(coordBytes).equals(ROUTE_RESPONSE)
											|| RxResponse.getClusterId(coordBytes).equals(ADDRESS_RESPONSE)))
						{
							Byte[] byteArray = new Byte[coordBytes.size()];
							coordBytes.toArray(byteArray);
							new Communicate(this).execute(byteArray);
							Log.d(DATA_TAG, "got route response");
//							Log.d(PACKET_LOG, "sending packet to async 1");
							sentToAsync++;
						}
						else if(Common.getFrameType(coordBytes).equals(RX_EXPLICIT) &&
								(RxResponse.getDataType(coordBytes).equals(NETWORK_TEST) ||
								RxResponse.getDataType(coordBytes).equals(SENSOR_READING) ||
								RxResponse.getDataType(coordBytes).equals(DONE_SENDING_DATA)))
						{
							Log.d(DATA_LOG, RxResponse.getDataType(coordBytes));
							place = "writing to file";
							for(byte r : coordBytes)
							{
								coordFile.write(Common.byteToHex(r));
							}
							coordFile.write(",");
							coordNextMillis = System.currentTimeMillis() - coordStartMillis;
							coordFile.write(coordNextMillis + "");
							coordFile.write("\n");
						}
						else
						{
							Byte[] byteArray = new Byte[coordBytes.size()];
							coordBytes.toArray(byteArray);
							new Communicate(this).execute(byteArray);
							Log.d(DATA_TAG, "got not rx response");
//							Log.d(PACKET_LOG, "sending packet to async 2");
							sentToAsync++;
						}
						
						coordBytes.clear();
//						dataAppend("");
					}
					coordBytes.add(b);
//					dataAppend(Common.byteToHex(b) + ", ");
					
//					Log.d(PACKET_LOG, Common.byteToHex(b));
					
				}
//				Log.d(DATA_LOG,"STOPPING");
				if(coordBytes.size() > 4 && 
						Common.calcLength(coordBytes) == Integer.parseInt(Common.getPacketLength(coordBytes), 16) &&
						Common.byteToHex(Common.calcChecksum(coordBytes)).equals(Common.getPacketChecksum(coordBytes)))
				{
					if(Common.getFrameType(coordBytes).equals(RX_EXPLICIT) && 
							(RxResponse.getClusterId(coordBytes).equals(ROUTE_RESPONSE)
									|| RxResponse.getClusterId(coordBytes).equals(ADDRESS_RESPONSE)))
					{
						Byte[] byteArray = new Byte[coordBytes.size()];
						coordBytes.toArray(byteArray);
						new Communicate(this).execute(byteArray);
						Log.d(DATA_TAG, "got route response");
	//					Log.d(PACKET_LOG, "sending packet to async 1");
						sentToAsync++;
					}
					else if(Common.getFrameType(coordBytes).equals(RX_EXPLICIT) &&
							(RxResponse.getDataType(coordBytes).equals(NETWORK_TEST) ||
							RxResponse.getDataType(coordBytes).equals(SENSOR_READING) ||
							RxResponse.getDataType(coordBytes).equals(DONE_SENDING_DATA)))
					{
						Log.d(DATA_LOG, RxResponse.getDataType(coordBytes));
						place = "writing to file";
						for(byte r : coordBytes)
						{
							coordFile.write(Common.byteToHex(r));
						}
						coordFile.write(",");
						coordNextMillis = System.currentTimeMillis() - coordStartMillis;
						coordFile.write(coordNextMillis + "");
						coordFile.write("\n");
					}
					else
					{
						Byte[] byteArray = new Byte[coordBytes.size()];
						coordBytes.toArray(byteArray);
						new Communicate(this).execute(byteArray);
						Log.d(DATA_TAG, "got not rx response");
	//					Log.d(PACKET_LOG, "sending packet to async 2");
						sentToAsync++;
					}
						coordBytes.clear();
	//					dataAppend("");
					}
			}
			
			if(availableBytesRouter > 0)
			{
//				Log.d(DATA_LOG,""+availableBytes);
//				Log.d(DATA_LOG,"STARTING");
				for(byte b : routerBuff)
				{
					place = "for b";
//					Log.d(DATA_TAG, place);
					/*Log.d(DATA_TAG, "Lengths: " + Common.calcLength(receivedBytes) + ", "
							+ Integer.parseInt(Common.getPacketLength(receivedBytes), 16) + "\nSums: " +
							Common.byteToHex(Common.calcChecksum(receivedBytes)) + ", " + 
							Common.getPacketChecksum(receivedBytes));*/
					
					if(routerBytes.size() > 4 && Common.byteToHex(b).equals("7e") && 
						Common.calcLength(routerBytes) == Integer.parseInt(Common.getPacketLength(routerBytes), 16) &&
						Common.byteToHex(Common.calcChecksum(routerBytes)).equals(Common.getPacketChecksum(routerBytes)))
					{
						if(Common.getFrameType(routerBytes).equals(RX_EXPLICIT) && 
								(RxResponse.getClusterId(routerBytes).equals(ROUTE_RESPONSE)
										|| RxResponse.getClusterId(routerBytes).equals(ADDRESS_RESPONSE)))
						{
							Byte[] byteArray = new Byte[routerBytes.size()];
							routerBytes.toArray(byteArray);
							new Communicate(this).execute(byteArray);
							Log.d(DATA_TAG, "got route response");
	//						Log.d(PACKET_LOG, "sending packet to async 3");
							sentToAsync++;
						}
						else if(Common.getFrameType(routerBytes).equals(RX_EXPLICIT) &&
								(RxResponse.getDataType(routerBytes).equals(NETWORK_TEST) ||
								RxResponse.getDataType(routerBytes).equals(SENSOR_READING) ||
								RxResponse.getDataType(routerBytes).equals(DONE_SENDING_DATA)))
						{
							Log.d(DATA_LOG, RxResponse.getDataType(routerBytes));
							place = "writing to file";
							for(byte r : routerBytes)
							{
								routerFile.write(Common.byteToHex(r));
							}
							routerFile.write(",");
							coordNextMillis = System.currentTimeMillis() - coordStartMillis;
							routerFile.write(coordNextMillis + "");
							routerFile.write("\n");
						}
						else
						{
							Byte[] byteArray = new Byte[routerBytes.size()];
							routerBytes.toArray(byteArray);
							new Communicate(this).execute(byteArray);
							Log.d(DATA_TAG, "got not rx response");
	//						Log.d(PACKET_LOG, "sending packet to async 4");
							sentToAsync++;
						}
							
							routerBytes.clear();
	//						dataAppend("");
						}
					routerBytes.add(b);
//					dataAppend(Common.byteToHex(b) + ", ");
					
//					Log.d(PACKET_LOG, Common.byteToHex(b));
					
				}
//				Log.d(DATA_LOG,"STOPPING");
				if(routerBytes.size() > 4 && 
						Common.calcLength(routerBytes) == Integer.parseInt(Common.getPacketLength(routerBytes), 16) &&
						Common.byteToHex(Common.calcChecksum(routerBytes)).equals(Common.getPacketChecksum(routerBytes)))
				{
					if(Common.getFrameType(routerBytes).equals(RX_EXPLICIT) && 
							(RxResponse.getClusterId(routerBytes).equals(ROUTE_RESPONSE)
									|| RxResponse.getClusterId(routerBytes).equals(ADDRESS_RESPONSE)))
					{
						Byte[] byteArray = new Byte[routerBytes.size()];
						routerBytes.toArray(byteArray);
						new Communicate(this).execute(byteArray);
						Log.d(DATA_TAG, "got route response");
//						Log.d(PACKET_LOG, "sending packet to async 3");
						sentToAsync++;
					}
					else if(Common.getFrameType(routerBytes).equals(RX_EXPLICIT) &&
							(RxResponse.getDataType(routerBytes).equals(NETWORK_TEST) ||
							RxResponse.getDataType(routerBytes).equals(SENSOR_READING) ||
							RxResponse.getDataType(routerBytes).equals(DONE_SENDING_DATA)))
					{
						Log.d(DATA_LOG, RxResponse.getDataType(routerBytes));
						place = "writing to file";
						for(byte r : routerBytes)
						{
							routerFile.write(Common.byteToHex(r));
						}
						routerFile.write(",");
						coordNextMillis = System.currentTimeMillis() - coordStartMillis;
						routerFile.write(coordNextMillis + "");
						routerFile.write("\n");
					}
					else
					{
						Byte[] byteArray = new Byte[routerBytes.size()];
						routerBytes.toArray(byteArray);
						new Communicate(this).execute(byteArray);
						Log.d(DATA_TAG, "got not rx response");
//						Log.d(PACKET_LOG, "sending packet to async 4");
						sentToAsync++;
					}
					routerBytes.clear();
//					dataAppend("");
				}
			}
		}catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}

	protected IOIOLooper createIOIOLooper(){
		return new Looper();
	}

	public void resetClock() throws InterruptedException, ConnectionLostException
	{
		anInput = analogInput.getVoltage();
		if(anInput > 3)
		{
			Log.d(DATA_TAG, "reseting clock");
			coordStartMillis = System.currentTimeMillis();
			coordStart = true;
		}
	}
	
	public void checkForNodes()
	{
		//request the node identifiers from each xbee on the network
		for(int n : requestNi)
		{
			try {
				outCoord.write((byte)n);
				Log.d(PACKET_LOG, Integer.toHexString(n));
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		Log.d(DATA_TAG, "sent out ni command");
	}
	
	public void sendData()
	{
		runOnUiThread(new Runnable() {
			public void run()
			{
				int frameId = 0x1;
				String[] trackers = new String[statusTrackers.size()];
				statusTrackers.keySet().toArray(trackers);
				for(String s : trackers)
				{
					if(!statusTrackers.get(s).getSecondStatus())
					{
						int[] packet = TxRequest.makePacket(sensorNodes.get(s), frameId, SEND_DATA_COMMAND);
						try {
							for(int p : packet)
							{
								outCoord.write((byte)p);
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				}
			}
		});
	}
	
	public void stopData()
	{
		runOnUiThread(new Runnable() {
			public void run()
			{
				int frameId = 0x2;
				String[] trackers = new String[statusTrackers.size()];
				statusTrackers.keySet().toArray(trackers);
				for(String s : trackers)
				{
					if(!statusTrackers.get(s).getSecondStatus())
					{
						int[] packet = TxRequest.makePacket(sensorNodes.get(s), frameId, STOP_DATA_COMMAND);
						try {
							for(int p : packet)
							{
								outCoord.write((byte)p);
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				}
			}
		});
	}

	public void getRoutes(XbeeInfo broadcastType)
	{
		int frameId = 0x1;
		int transactionNumber = 0x1;
		int startIndex = 0;
		int[] packet = TxRequest.getRoutingTables(broadcastType, frameId, transactionNumber, startIndex);
		int[] coordPacket = TxRequest.getRoutingTables(xbeeCoord, frameId, transactionNumber, startIndex);
		try {
			for(int p : packet)
			{
				outCoord.write((byte)p);
			}
			for(int c : coordPacket)
			{
				outCoord.write((byte)c);
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
		Log.d(DATA_TAG, "requesting routes");
	}

	public void updateSensorsSecond()
	{
		Log.d(DATA_LOG, "started update sensor my");
		String[] trackers = new String[statusTrackers.size()];
		statusTrackers.keySet().toArray(trackers);
		XbeeInfo xInfo;
		int[] addressInfo = new int[10];
		int length = addressInfo.length;
		
		addressing:
		for(int x = 0, t = 0; x < length; t++)
		{
			if(secondRadio != null)
			{
				xInfo = sensorNodes.get(secondRadioSl);
				int[] shArray = Common.breakUp(32, xInfo.getSh());
				int[] slArray = Common.breakUp(32, xInfo.getSl());
				int[] myArray = Common.breakUp(16, xInfo.getMy());
				for(int n = 0; n < 4; n++, x++)
				{
					addressInfo[x] = shArray[n];
				}
				for(int n = 0; n < 4; n++, x++)
				{
					addressInfo[x] = slArray[n];
				}
				for(int n = 0; n < 2; n++, x++)
				{
					addressInfo[x] = myArray[n];
				}
				break addressing;
			}
		}
		
		for(int x = 0; x < trackers.length; x++)
		{
			Log.d(DATA_LOG, trackers[x] + ": " + statusTrackers.get(trackers[x]).getSecondStatus());
		}
		
		for(int x = 0; x < trackers.length; x++)
		{
			if(!statusTrackers.get(trackers[x]).getSecondStatus())
			{
				Log.d(DATA_LOG, "sent update my to router: " + trackers[x]);
				int[] updatePacket = TxRequest.makePacket(sensorNodes.get(trackers[x]), 0x0, ADDRESS_INFO, addressInfo);
				try {
					for(int u : updatePacket)
					{
						outCoord.write((byte)u);
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}
	
	public void setStatusText(final String text)
	{
		runOnUiThread( new Runnable() {
			public void run() {
				status.setText(text);
			}
		});
	}

	public boolean allSecondSet()
	{
		String[] trackers = new String[statusTrackers.size()];
		statusTrackers.keySet().toArray(trackers);
		for(String t : trackers)
		{
			if(statusTrackers.get(t).getSecondRadioStatus() == false)
			{
				return false;
			}
		}
		return true;
	}
	
	public boolean allDestSet()
	{
		String[] trackers = new String[statusTrackers.size()];
		statusTrackers.keySet().toArray(trackers);
		for(String t : trackers)
		{
			if(statusTrackers.get(t).getDestRadioStatus() == false)
			{
				return false;
			}
		}
		return true;
	}
	
	public boolean allClocksReset()
	{
		String[] trackers = new String[statusTrackers.size()];
		statusTrackers.keySet().toArray(trackers);
		for(String t : trackers)
		{
			if(statusTrackers.get(t).getClockStatus() == false)
			{
				return false;
			}
		}
		return true;
	}

	public void chooseDest()
	{
		Log.d(DATA_LOG, "started choosing destination");
		String[] trackers = new String[statusTrackers.size()];
		statusTrackers.keySet().toArray(trackers);
		if(trackers.length == 1)
		{
			int[] data = {2};
			int[] packet = TxRequest.makePacket(sensorNodes.get(trackers[0]), 0x1, DEST_RADIO, data);
			String stringData = "";
			for(int p : packet)
			{
				stringData += Common.byteToHex((byte) p);
			}
			try {
				for(int p : packet)
				{
					outCoord.write((byte)p);
					Log.d(DATA_LOG, Common.byteToHex((byte)p));
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else if(trackers.length > 1)
		{
			
		}
	}

	public void retrieveClockStatus()
	{
		runOnUiThread(new Runnable() {
			public void run()
			{
				int frameId = 0x3;
				String[] trackers = new String[statusTrackers.size()];
				statusTrackers.keySet().toArray(trackers);
				for(String s : trackers)
				{
					if(!statusTrackers.get(s).getSecondStatus())
					{
						int[] packet = TxRequest.makePacket(sensorNodes.get(s), frameId, MILLIS_VALUE_REQUEST);
						try {
							for(int p : packet)
							{
								outCoord.write((byte)p);
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
					}
				}
			}
		});
	}
}
