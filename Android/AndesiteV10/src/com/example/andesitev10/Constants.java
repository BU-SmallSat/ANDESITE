package com.example.andesitev10;

public class Constants {
	
	// Activity int constants for intents.
	public static final int
			DETUMBLE_ACTIVITY = 1,
			DEPLOY_ACTIVITY = 2,
			SCIENCE_ACTIVITY = 3,
			TRANSMIT_ACTIVITY = 4,
			IDLE_ACTIVITY = 5;

	// Int constants for pins for radios using uart
	public static final int
			UART_1_RX = 3,
			UART_1_TX = 4,
			UART_2_RX = 5,
			UART_2_TX = 6;
	
	// Int constants for baud rates
	public static final int IOIO_XBEE_BAUD = 19200;
	
	// Int constants for xbee addressing
	public static final int
			COORD_SH = 0,
			COORD_SL = 0,
			COORD_MY = 0xFFFE,
			BROADCAST_SH = 0,
			BROADCAST_SL = 0xFFFF,
			BROADCAST_MY = 0xFFFE;
	
	// Int constants for data types
	public static final int
			SENSOR_READING = 1,
			SECOND_RADIO_ADDRESS = 2;
	
	// Int constant for xbee radio information
	public static final int
			NUM_RADIOS = 2,
			START_BIT = 0x7E;
	
	// Int constants for xbee frame types
	public static final int
			RX_EXPLICIT = 0x91;
			
}
