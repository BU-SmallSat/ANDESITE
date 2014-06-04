#include "Arduino.h"
#include "Shared.h"

void Shared::breakUp(long address, byte array[], int arrayLength)
{
	for(int i = arrayLength; i > 0; i--) {
		array[i - 1] = (byte) ((address >> ((arrayLength * 8) - (8 * i))) & 0xFF);
	}
}

byte Shared::getChecksum(byte packet[], int length)
{
	int sum = 0x0;

	for (int x = 3; x <length - 1; x++) { 
		sum += packet[x];
	}

	byte checksum = 0xFF - (byte)(sum & 0xFF);
	return checksum;
}

byte Shared::getLSB(int packetLength)
{
	byte LSB = (byte) packetLength - 4;
	return LSB;
}

byte Shared::getStart()
{
	return START;
}

int Shared::getMsbLengthIndex()
{
	return MSB_LENGTH_INDEX;
}

int Shared::getLsbLengthIndex()
{
	return LSB_LENGTH_INDEX;
}

/*String Shared::byteToHex(byte b)
{
        char hexArray[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
        char hexChars[2];
        int v = b & 0xFF;
	hexChars[0] = hexArray[(v >>> 4)];
        hexChars[1] = hexArray[(v & 0x0F)];
	return hexChars[0] + "" + hexChars[1];
}

String Shared::byteToHex(byte b[], bLength)
{
        char hexArray[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
        char hexChars[bLength * 2];
	int v;
	for ( int j = 0; j < bLength; j++ ) {
	    v = b[j] & 0xFF;
	    hexChars[j * 2] = hexArray[(v >>> 4)];
	    hexChars[j * 2 + 1] = hexArray[(v & 0x0F)];
	}
        String temp = "";
        for(int x = 0; x < bLength * 2; x++)
        {
            temp += hexChars[x];
        }
            
	return temp;
    
}*/