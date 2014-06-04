package com.example.andesitev9;

import android.annotation.SuppressLint;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;

@SuppressLint("WorldReadableFiles")
public class DataTracker {
	
	private String sl, ni;
	private ArrayList<String> dataIds, missingDataIds, errorDataIds, extraDataIds;
	private int run, lastRun;
	private boolean sendStatus, beenUsed, secondRadio;
	private ArrayList<Integer> runIds, missingRunIds;
	
	public DataTracker(String sl, String ni, boolean sendStatus)
	{
		this.sl = sl;
		this.ni = ni;
		dataIds = new ArrayList<String>(255);
		missingDataIds = new ArrayList<String>(255);
		errorDataIds = new ArrayList<String>(255);
		run = 1;
		lastRun = 0;
		this.sendStatus = sendStatus;
		runIds = new ArrayList<Integer>();
		missingRunIds = new ArrayList<Integer>();
		extraDataIds = new ArrayList<String>();
		beenUsed = false;
		this.secondRadio = false;
	}
	
	public void setSecondStatus(boolean radio)
	{
		this.secondRadio = radio;
	}
	
	public boolean getSecondStatus()
	{
		return this.secondRadio;
	}
	
	public void setLastRun(int x)
	{
		this.lastRun = x;
		beenUsed = true;
	}
	
	public int getLastRun()
	{
		return this.lastRun;
	}
	
	public boolean getSendStatus()
	{
		return this.sendStatus;
	}
	
	public void setSendStatus(boolean sendStatus)
	{
		this.sendStatus = sendStatus;
	}
	
	public String getSl()
	{
		return this.sl;
	}
	
	public String getNi()
	{
		return this.ni;
	}
	
	public void addExtraDataId(String s)
	{
		extraDataIds.add(s);
	}
	
	public String getExtraDataId(int x)
	{
		return extraDataIds.get(x);
	}
	
	public ArrayList<String> getExtraDataIds()
	{
		return extraDataIds;
	}
	
	public int getExtraDataIdsSize()
	{
		return extraDataIds.size();
	}
	
	public void clearExtraDataIds()
	{
		runIds.clear();
	}
	
	public void addRunId(int r)
	{
		runIds.add(r);
	}
	
	public int getRunId(int x)
	{
		return runIds.get(x);
	}
	
	public ArrayList<Integer> getRunIds()
	{
		return runIds;
	}
	
	public int getRunIdsSize()
	{
		return runIds.size();
	}
	
	public void clearRunIds()
	{
		runIds.clear();
	}
	
	public void addDataId(String id)
	{
		dataIds.add(id);
		beenUsed = true;
	}
	
	public String getDataId(int x)
	{
		return dataIds.get(x);
	}
	
	public ArrayList<String> getDataIds()
	{
		return dataIds;
	}
	
	public int getDataIdsSize()
	{
		return dataIds.size();
	}
	
	public void clearDataIds()
	{
		dataIds.clear();
	}
	
	public void addMissingDataId(String missing)
	{
		missingDataIds.add(missing);
	}
	
	public String getMissingDataId(int x)
	{
		return missingDataIds.get(x);
	}
	
	public void removeMissingDataId(int x)
	{
		missingDataIds.remove(x);
	}
	
	public ArrayList<String> getMissingDataIds()
	{
		return missingDataIds;
	}
	
	public int getMissingDataIdsSize()
	{
		return missingDataIds.size();
	}
	
	public void clearMissingDataIds()
	{
		missingDataIds.clear();
	}
	
	public void addMissingRunId(int missing)
	{
		missingRunIds.add(missing);
	}
	
	public int getMissingRunId(int x)
	{
		return missingRunIds.get(x);
	}
	
	public void removeMissingRunId(int x)
	{
		missingRunIds.remove(x);
	}
	
	public ArrayList<Integer> getMissingRunIds()
	{
		return missingRunIds;
	}
	
	public int getMissingRunIdsSize()
	{
		return missingRunIds.size();
	}
	
	public void clearMissingRunIds()
	{
		missingRunIds.clear();
	}
	
	public void addErrorDataId(String error)
	{
		errorDataIds.add(error);
	}
	
	public ArrayList<String> getErrorDataIds()
	{
		return errorDataIds;
	}
	
	public String getErrorDataId(int x)
	{
		return errorDataIds.get(x);
	}
	
	public int getErrorDataIdsSize()
	{
		return errorDataIds.size();
	}
	
	public void clearErrorDataIds()
	{
		errorDataIds.clear();
	}
	
	public void setRun(int x)
	{
		run = x;
	}
	
	public int getRun()
	{
		return run;
	}
	
	public int getHighestRun()
	{
		return Collections.max(runIds);
	}
	
	public int getHighestDataId()
	{
		int size = dataIds.size();
		ArrayList<Integer> intDataIds = new ArrayList<Integer>();
		for(int x = 0; x < size; x++)
		{
			intDataIds.add(Integer.parseInt(dataIds.get(x), 16));
		}
		return Collections.max(intDataIds);
	}
	
	public boolean getUsedStatus()
	{
		return this.beenUsed;
	}
	
	public void setUsedStatus(boolean b)
	{
		this.beenUsed = b;
	}

}
