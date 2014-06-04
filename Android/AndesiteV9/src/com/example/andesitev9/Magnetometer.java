package com.example.andesitev9;

public class Magnetometer {

	public static final double ZERO = 1/2;
	public static final double GAUSSCONVERSION = 2/ZERO;
	private double x, y, z, timeStamp;
	//One gauss = 100 microtesla
	
	public Magnetometer(double timeStamp, double x, double y, double z)
	{
		this.x = x;
		this.y = y;
		this.z = z;
		this.timeStamp = timeStamp;
	}

	private double toGauss(double volt)
	{
		return (volt-ZERO) * GAUSSCONVERSION;
	}
	
	public double getTimeStamp()
	{
		return this.timeStamp;
	}
	
	public double getX()
	{
		return this.x;
	}
	
	public double getY()
	{
		return this.y;
	}
	
	public double getZ()
	{
		return this.z;
	}
}
