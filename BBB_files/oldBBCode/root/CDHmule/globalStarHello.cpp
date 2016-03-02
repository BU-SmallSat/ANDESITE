#include "serialib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO2"                         
#endif
    
#define currentFile "helloTest.txt"
#define ENS "0-453221"
const int mesBufSize = 128;
#define char messageBuffer[mesBufSize];
#define int sysBufSize = 100;
#define char systemBuffer[sysBufSize];
#define int ensSize = 8;    
#define char ens[ensSzie];
#define int fNameSizeBufSize = 3;
#define char fileNameSize[fNameSizeBufSize];
#define int fNameBufSize = 100;    
#define char fileName[fNameBufSize];
#define fSizeBufSize = 6;
#define char fileSize[fSizeBufSize];
#define string messageString = "";

class GlobalStar
{
public:
    string ESN;

    GlobalStar();
    void SendMessage(string fileName);

}


GlobalStar globalstar = new GlobalStar();

globalstar.sendmessage()
char[] fillBuffer(int bufferSize, int input){
    char[] Buffer = new Char[bufferSize];
    for(int ii = 1; ii <= bufferSize; ii++){
        mod = input%(10*ii);
        if(ii = 1){
            div = 1;
        }
        else{
            div = 10*(ii-1);
        }
        buffer[size-ii] = mod/div;
        size -= mod;
    }
    return buffer; 
}


void GlobalStar::SendMessage(string fileName)
{   
    // Open the file
    // read the file
    // Send the file contents

	//Serial Initialization
    printf("CDH process: Initializing serial interface with globalstar radio \n");
    serialib LS;
    int Ret;

    // Open serial port
    Ret=LS.Open(DEVICE_PORT,38400);
    if (Ret!=1) {
        printf ("CDH process: Error while opening port. Permission problem ?\n");
        return Ret;
    }
    printf ("CDH process: Serial port opened successfully !\n");
    
    //fill ens buffer
    ens = ENS;

    //fill fileNameSize buffer
    size = fileNameSize.size();
    if(size < 0){
        printf("error in size reading\n");
    }
    else if(size < fNameSizeBufSize){
        fileNameSize = fillBuffer(fNameSizeBufSize, size);
        
        //fill fileName buffer
        fileName = currentFile;
    }
    else{
        printf("file is too big to send\n");
    }

    //fill fileSize buffer
    streampos fsize = 0;
    ifstream file( currentFile, ios::binary);
    fsize = file.tellg();
    file.seekg( 0, ios::end);
    fsize = file.tellg() - fsize;
    file.close();
    
    if(fsize < 0){
        printf("error in size reading\n");
    }
    else if(fsize < fSizeBufSize){
        fileSize = fillBuffer(fSizeBufSize, fsize);

        //fill payloadBuffer
        string line;
        ifstream myfile ("example.txt");
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                cout << line << '\n';
                mesesageString += line;
            }
            myfile.close();
        }
        else cout << "Unable to open file"; 
    }
    else{
        printf("file is too big to send\n");
    }

    //format string for globalstar message
    sprintf(messageBuffer, "%s%s%s", ens,fileNameSize, fileName, fileSize, messageString, CRC)

    //RF Initialization - Serial Write to command arduino to execute it's function?
    //write to serial
    printf("CDH process: Sending message to globalstar radio\n");
    Ret=LS.WriteString(messageBuffer);  //command "1" will map to RF network initialization
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
