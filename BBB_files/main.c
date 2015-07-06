#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serialib.h"
#include <signal.h>

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO4"                         // ttyS0 for linux
#endif

void HealthBeacon(int sig) 
{
  printf("kick the dog\r\n");
  
  if(adc_check()){
  	printf("adc is stable");
  }
  
  if(eps_check()){
  	printf("eps is sufficient ");
  }

  /* reset the timer so we get called again in 5 seconds */
  alarm(5);
}

bool adc_check(){
	return TRUE;
}

bool eps_check()){
	return TRUE;
}


int main()
{
    serialib LS;                                                            // Object of the serialib class
    int Ret;                                                                // Used for return values
    char Buffer[128];

    // Open serial port

    Ret=LS.Open(DEVICE_PORT,115200);                                        // Open serial link at 115200 bauds
    if (Ret!=1) {                                                           // If an error occured...
        printf ("Error while opening port. Permission problem ?\n");        // ... display a message ...
        return Ret;                                                         // ... quit the application
    }
    printf ("Serial port opened successfully !\n");

    // Write the AT command on the serial port

    Ret=LS.WriteString("AT\n");                                             // Send the command on the serial port
    if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...
        return Ret;                                                         // ... quit the application.
    }
    printf ("Write operation is successful \n");

    // Read a string from the serial device 								// Read a maximum of 128 characters with a timeout of 5 seconds
    Ret=LS.ReadString(Buffer,'\n',128,5000);                 				// The final character of the string must be a line feed ('\n')            
                                                                      
    if (Ret>0)                                                              // If a string has been read from, print the string
        printf ("String read from serial port : %s",Buffer);
    else
        printf ("TimeOut reached. No data received !\n");                   // If not, print a message.

    // Close the connection with the device

    LS.Close();

    return 0;
}