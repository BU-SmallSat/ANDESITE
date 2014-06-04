#include "Arduino.h"
#include "Common.h"

int Common::getChecksum( byte packet[], int length)
{
    int sum = 0x0;
		
    for (int x = 3; x <length - 1; x++) { 
        sum += packet[x];
    }
			
    int checksum = 0xFF - (byte)sum;
    return checksum;
}
		
int Common::getLSB( int packetLength )
{
    int LSB = packetLength - 4;
    return LSB;
}

void Common::toByteArray(int val, byte b[2])
{
    b[1] = (byte) (val & 0xFF);
    b[0] = (byte) ((val >> 8) & 0xFF);
}

int Common::toInt(byte b[2])
{
    int val = ((int) b[0]) << 8;
    val |= ((int) b[1]);
    return val;
}