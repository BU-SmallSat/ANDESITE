package com.example.andesitev9;

public class Potentiometer {
	private float value;
	private String timeStamp;
	
	public Potentiometer(float value, String timeStamp) {
		this.value = value;
		this.timeStamp = timeStamp;
	}
	
	public float getValue() {
		return this.value;
	}
	
	public String getTimeStamp() {
		return this.timeStamp;
	}
	
	public int getNumberParameters()
	{
		return 10;
	}
}
