#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <linux/watchdog.h>
#include "serialib.h"

#define WATCHDOGDEV "/dev/watchdog"

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO4"                         // ttyS0 for linux
#endif

int EpsInit(){
   printf("CDH process: Executing EPS initialization \n"); 
   return 0;
}

int WatchDogInit(){
    printf("CDH process: Executing WatchDog initialization \n");
    int fd;         /* File handler for watchdog */
    int interval = 8;      /* Watchdog timeout interval (in secs) */
    fd = open(WATCHDOGDEV, O_RDWR);

    if (-1 == fd) {
      //fprintf(stderr, "Error: %s\n", strerror(errno));
      //exit(EXIT_FAILURE);
    }

    if (ioctl(fd, WDIOC_SETTIMEOUT, &interval) != 0) {
        //fprintf(stderr,"Error: Set watchdog interval failed\n");
        //exit(EXIT_FAILURE);
    }
    /* Check if last boot is caused by watchdog */
   // if (ioctl(fd, WDIOC_GETBOOTSTATUS, &bootstatus) == 0) {
      //fprintf(stdout, "Last boot is caused by : %s\n",
      //(bootstatus != 0) ? "Watchdog" : "Power-On-Reset");
    //} else {
      //fprintf(stderr, "Error: Cannot read watchdog status\n");
      //exit(EXIT_FAILURE);
    //}
    return fd;
}

void GlobalStarInit(){
    printf("CDH process: Executing Global Star radio initialization \n");
}

int MemoryInit(){
    printf("CDH process: Shared memory initialization \n");
    return 0;
}

int nodesEjectedCheck(){
    printf("CDH process: Checking if nodes have been ejected\n");
    int ejected = 0;
    //set lock on memory
    //read from memory
    printf("CDH process: Reading values from shared memory with ADC process \n");
    //if memory returns true nodes have been ejected so set ejecteed to 1;
    ejected = 1;
    return 1;
}

int EpsCheck(){
    //return current power reading
    printf("CDH process: Checking EPS \n");
    return 1;
}

void LowPowerMode(){
    printf("CDH process: Entering Low Power Mode \n");
}

void GlobalStarSendHealth(){
    printf("CDH process: Sending Global Star Health Beacon message \n ");
}

void GlobalStarSendData(){
    printf("CDH process: Sending Global Star Downlink with WSN data\n");
    //read data from data text file
    //send that data line by line?
    //Kick the dog in the loop of sends
    ioctl(fd, WDIOC_KEEPALIVE, NULL);
    fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");
}

void HealthBeacon(int sig){
    printf("CDH process: Executing Health Beacon function \n");
    int status = EpsCheck();
    GlobalStarSendHealth();
    if(status < 1){
        LowPowerMode();
    }
    signal(SIGALRM, HealthBeacon); //reinstall the handler
    alarm(10);  //reset the timer so we get called again in 5 seconds
}

int main(){

    //EPS Initialization
    EpsInit();

    //ADC process Initializaiton
    printf("CDH process: Spawning ADC process \n");
    system("./ADCprocess &");

    //Watchdog Initialization
    int fd = WatchDogInit();

    //Health Beacon Initialization
    signal(SIGALRM, HealthBeacon);
    alarm(10);

    //Initialize Shared Memory
	//initialize variables to false
    MemoryInit();

    //Kick the dog
    ioctl(fd, WDIOC_KEEPALIVE, NULL);
    fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");

    //HOw TO GIVE ALL PROCESSING POWER TO ADC PROCESS UNTIL NODES HAVE BEEN EJECTED - how?
    while(!nodesEjectedCheck()){  //this is an active wait - how to make it passive?
        printf("CDH process: Waiting for ADC process to eject nodes \n");
        //Kick the dog
        ioctl(fd, WDIOC_KEEPALIVE, NULL);
        fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");
    }

    //Serial Initialization
    printf("CDH process: Initializing serial interface with onboard arduino for RF comm \n");
    serialib LS;
    int Ret;
    char Buffer[128];

    // Open serial port
    Ret=LS.Open(DEVICE_PORT,115200);
    if (Ret!=1) {
        printf ("CDH process: Error while opening port. Permission problem ?\n");
        return Ret;
    }
    printf ("CDH process: Serial port opened successfully !\n");

    //Kick the dog
    ioctl(fd, WDIOC_KEEPALIVE, NULL);
    fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");

    while(1){
    	//RF Initialization - Serial Write to command arduino to execute it's function?
    	//write to serial
    	printf("CDH process: Sending RF init command to arduino\n");
    	Ret=LS.WriteString("1\n");  //command "1" will map to RF network initialization
    	if (Ret!=1) {
            printf ("CDH process: Error while writing data to arduino\n");
    	}
      	printf ("CDH process: Write operation to arduino was successful \n");
    	/* //do we need to read for the RF mesh network initialization?
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

    	//Kick the dog
    	ioctl(fd, WDIOC_KEEPALIVE, NULL);
    	fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");

    	//RF network poll of all nodes
    	//write to serial
    	printf("CDH process: Sending RF init command to arduino\n");
    	Ret=LS.WriteString("2\n");  //command "1" will map to RF network initialization
    	if (Ret!=1) {
            printf ("CDH process: Error while writing data to arduino\n");
    	}
    	printf ("CDH process: Write operation to arduino was successful \n");
    	//Serial Reads
    	while(Buffer != ":: Done requesting data from WSNs.") {
            ioctl(fd, WDIOC_KEEPALIVE, NULL);
            fprintf(stdout, "CDH process: Kick watchdog through IOCTL\n");
            // Read a string from the serial device                 // Read a maximum of 128 charact$
            Ret=LS.ReadString(Buffer,'\n',128,5000);                        // The final character o$
            if(Ret > 0){                                          // If a string has been read from, $
                printf ("CDH process: String read from serial port : %s\n",Buffer);
                //write buffer to text file
            }
    	}

    	//Global star downlink to relay data from nodes
    	GlobalStarSendData();
    }
    return 0;
}


