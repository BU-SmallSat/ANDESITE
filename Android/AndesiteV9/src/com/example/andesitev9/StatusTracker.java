package com.example.andesitev9;

import java.util.ArrayList;
import java.util.Collections;

public class StatusTracker {
	
	private String sl, ni;
	private boolean sendStatus, secondRadio, secondRadioSetStatus, destRadioSetStatus,
		resetClock;
	
	public StatusTracker(String sl, String ni, boolean sendStatus)
	{
		this.sl = sl;
		this.ni = ni;
		this.sendStatus = sendStatus;
		this.secondRadio = false;
		this.secondRadioSetStatus = false;
		this.destRadioSetStatus = false;
		this.resetClock = false;
	}
	
	public void setClockStatus(boolean clock)
	{
		this.resetClock = clock;
	}
	
	public boolean getClockStatus()
	{
		return this.resetClock;
	}
	
	public void setSecondRadioStatus(boolean second)
	{
		this.secondRadioSetStatus = second;
	}
	
	public boolean getSecondRadioStatus()
	{
		return this.secondRadioSetStatus;
	}
	
	public void setDestRadioStatus(boolean dest)
	{
		this.destRadioSetStatus = dest;
	}
	
	public boolean getDestRadioStatus()
	{
		return this.destRadioSetStatus;
	}
	
	public void setSecondStatus(boolean radio)
	{
		this.secondRadio = radio;
	}
	
	public boolean getSecondStatus()
	{
		return this.secondRadio;
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

}
