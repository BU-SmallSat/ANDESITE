#ifndef XbeeInfo_h
#define XbeeInfo_h

#include "Arduino.h" // need to include Arduino header file in libraries!

class XbeeInfo
{
	private:
		//data fields( variables ); the underscore indicates local variable
		long _sl; // Serial low ID
		long _sh; // Serial high ID
		long _my; //  MY ID
		String _ni; // Node ID
	
	public:
		// Constructor
		XbeeInfo(long sh, long sl, String ni);
        XbeeInfo(long sh, long sl, long my, String ni);
        XbeeInfo(long sh, long sl, long my);

		// Getter/accessor functions- to access variables
		long getSl();
		long getSh();
		long getMy();
		String getNi();

		// Setter/mutator functions- to change variables
		void setMy( long my );
		void setNi( String ni );
        void setSh(long sh);
        void setSl(long sl);
};
#endif
