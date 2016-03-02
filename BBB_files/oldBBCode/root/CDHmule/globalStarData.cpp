#include "serialib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO2"                         
#endif
    
#define currentFile "helloTest.txt"
#define ENS "0-453221"

int main(){    
    //Serial Initialization
    printf("CDH process: Initializing serial interface with onboard arduino for RF comm \n");
    serialib LS;
    int Ret;
    char messageBuffer[128];
    char systemBuffer[100];
    // Open serial port
    Ret=LS.Open(DEVICE_PORT,115200);
    if (Ret!=1) {
        printf ("CDH process: Error while opening port. Permission problem ?\n");
        return Ret;
    }
    printf ("CDH process: Serial port opened successfully !\n");

    //system call to compress the file we are sending
    sprintf(systemBuffer,"gzip -c %s > toSend.gz", fileName);
    system(systemBuffer);

    //format string for globalstar message
    sprintf(messageBuffer, "%s%s%s", ENS,fileNameSize, fileName, fileSize, payloadBuffer, CRC)

    //RF Initialization - Serial Write to command arduino to execute it's function?
    //write to serial
    printf("CDH process: Sending RF init command to arduino\n");
    Ret=LS.WriteString("1\n");  //command "1" will map to RF network initialization
    if (Ret!=1) {
        printf ("CDH process: Error while writing data to arduino\n");
    }
    printf ("CDH process: Write operation to arduino was successful \n");
    /* 
    //do we need to read for the RF mesh network initialization?
    //Serial Reads
    while(Buffer != ":: Done waiting for WSNs to (re)enter Transfer Mode.\n");
        ioctl(fd, WDIOC_KEEPALIVE, NULL);
        fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");
        // Read a string from the serial device                 // Read a maximum of 128 characters with a timeout of 5 seconds
        Ret=LS.ReadString(Buffer,'\n',128,5000);                        // The final character of the string must be a line feed ('\n')            
        if(Ret > 0){                                          // If a string has been read from, print the string
            printf ("CDH process: String read from serial port : %s\n",Buffer);
        }
    }
    */
}