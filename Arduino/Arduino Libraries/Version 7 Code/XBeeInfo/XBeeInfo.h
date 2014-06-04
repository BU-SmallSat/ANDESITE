#ifndef XBeeInfo_h
#define XBeeInfo_h

#include "Arduino.h" // need to include Arduino header file in libraries!

class XBeeInfo
{
	private:
		//data fields( variables ); the underscore indicates local variable
		long _sl; // Serial low ID
		long _sh; // Serial high ID
		int _my; //  MY ID
		String _ni; // Node ID
	
	public:
		// Constructor
		XBeeInfo(long sh, long sl, String ni);
        XBeeInfo(long sh, long sl, String ni, int my);
        XBeeInfo(long sh, long sl, int my);

		// Getter/accessor functions- to access variables
		long getSL();
		long getSH();
		int getMY();
		String getNI();

		// Setter/mutator functions- to change variables
		void setMY( int my );
		void setNI( String ni );
};
#endif
