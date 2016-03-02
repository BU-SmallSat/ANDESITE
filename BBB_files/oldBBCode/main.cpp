#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "serialib.h"

#include <linux/watchdog.h>


#define WATCHDOGDEV "/dev/watchdog"

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO4"                         // ttyS0 for linux
#endif

int adc_check(){
  return 1;
}

int eps_check(){
  return 0;
}

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

    int fd;         /* File handler for watchdog */
    int interval = 8;      /* Watchdog timeout interval (in secs) */
    fd = open(WATCHDOGDEV, O_RDWR);

    /*
    if (-1 == fd) {
      fprintf(stderr, "Error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    */

    if (ioctl(fd, WDIOC_SETTIMEOUT, &interval) != 0) {
      /*fprintf(stderr,"Error: Set watchdog interval failed\n");
      exit(EXIT_FAILURE);
      */
    }


    /* Check if last boot is caused by watchdog */
    /*
    if (ioctl(fd, WDIOC_GETBOOTSTATUS, &bootstatus) == 0) {
      fprintf(stdout, "Last boot is caused by : %s\n",
      (bootstatus != 0) ? "Watchdog" : "Power-On-Reset");
    } else {
      fprintf(stderr, "Error: Cannot read watchdog status\n");
      exit(EXIT_FAILURE);
    }
    //write to serial  
    Ret=LS.WriteString("AT\n");                                             // Send the command on the serial port
    if (Ret!=1) {                                                           // If the writting operation failed ...
        printf ("Error while writing data\n");                              // ... display a message ...
        return Ret;                                                         // ... quit the application.
    }
    printf ("Write operation is successful \n");
    */
    

    /*  //write to serial
    Ret=LS.WriteString("AT\n");                                             // Send the comman$
    if (Ret!=1) {                                                           // If the writting$
        printf ("Error while writing data\n");                              // ... display a m$
        return Ret;                                                         // ... quit the ap$
    }
    printf ("Write operation is successful \n");
    */




    while(1) {
      ioctl(fd, WDIOC_KEEPALIVE, NULL);
      fprintf(stdout, "Kick watchdog through IOCTL\n");

      // Read a string from the serial device                 // Read a maximum of 128 characters with a timeout of 5 seconds
      Ret=LS.ReadString(Buffer,'\n',128,5000);                        // The final character of the string must be a line feed ('\n')            
                                                                      
      if (Ret>0)                                                              // If a string has been read from, print the string
        printf ("String read from serial port : %s",Buffer);

    }

    // Close the connection with the device

    LS.Close();

    return 0;
}


