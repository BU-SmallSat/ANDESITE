#ifndef XbeeInfo_h
#define XbeeInfo_h

#include "Arduino.h" // need to include Arduino header file in libraries!

class XbeeInfo
{
	private:
		//data fields( variables ); the underscore indicates local variable
		long _sl; // Serial low ID
		long _sh; // Serial high ID
		unsigned int _my; //  MY ID
	
	public:
		// Constructors
                XbeeInfo(long sh, long sl, unsigned int my);
                XbeeInfo();

		// Getter/accessor functions- to access variables
		long getSl();
		long getSh();
		unsigned int getMy();

		// Setter/mutator functions- to change variables
                void setSh( long sh);
                void setSl( long sl);
		void setMy( unsigned int my );
};
#endif
