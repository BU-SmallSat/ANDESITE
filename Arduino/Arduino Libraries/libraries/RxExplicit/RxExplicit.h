#ifndef RXEXPLICIT_H
#define RXEXPLICIT_H

#include "Arduino.h"
#include "Common.h"
#include "Packet.h"

class RxExplicit
{
	public:
                // returns the data type byte
		byte getDataType(Packet & packet);
                
                // returns the receive options byte
		byte getReceiveOptions(Packet & packet);
                
                // returns the serial high address
		long getSh(Packet & packet);
                
                // returns the serial low address
		long getSl(Packet & packet);
                
                // returns the 16-bit address
		unsigned int getMy(Packet & packet);
                
                // returns the source endpoint byte
		byte getSourceEndpoint(Packet & packet);
                
                // returns the destination endpoint byte
		byte getDestEndpoint(Packet & packet);
                
                // returns the cluster id
		unsigned int getClusterId(Packet & packet);
                
                // returns the profile id
		unsigned int getProfileId(Packet & packet);
                
                // puts the RF data in the data packet
		void getData(Packet & packet, Packet & data);
                
                // finds the serial high, serial low, and 16-bit address
                void getSecondRadioAddress(Packet & packet, long & sh, long & sl, unsigned int & my);
	private:
                // indices of bytes within the packet
		#define SH_INDEX 4
		#define SL_INDEX 8
		#define MY_INDEX 12
		#define SOURCE_ENDPOINT_INDEX 14
		#define DEST_ENDPOINT_INDEX 15
		#define CLUSTER_ID_INDEX 16
		#define PROFILE_ID_INDEX 18
                #define OPTIONS_INDEX 20
                #define DATA_TYPE_INDEX 21
		#define DATA_INDEX 22
};
#endif