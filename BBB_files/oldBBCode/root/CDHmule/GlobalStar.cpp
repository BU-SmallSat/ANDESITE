#include "serialib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include GlobalStar.h
static unsigned short   crc_tab16[256];

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO2"                         
#endif

void GlobalStar::GlobalStar(){}
}

unsigned short update_crc_16(char[] message, size) {
    static unsigned short   crc_tab16[256];
    int P_16 = 0xA001;
    int i, j;
    unsigned short crc, c;
    unsigned short tmp, short_c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = (unsigned short) i;

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_16;
            else                      crc =   crc >> 1;

            c = c >> 1;
        }

        crc_tab16[i] = crc;
    }

    
    for(int ii = 0; ii < size; ii++){
        c = message[ii];
        short_c = 0x00ff & (unsigned short) c;
        tmp =  crc       ^ short_c;
        crc = (crc >> 8) ^ crc_tab16[ tmp & 0xff ];
    }

    return crc;

} 

void GlobalStar::SendFile(string name)
{   

    string  = "0-453221";
    int fileNameSize= 0;    
    string fileName= "";
    int fileSize= 0;
    string fileString = "";
    unsigned short CRC = 0;
    char[] messageBuffer;

    // Determine the size of the fil
    // Format the header as a string with sprintf
    // Create a buffer that is the size of the file plus the size of the header plus two bytes for CRC
    // Copy the header into the start of the buffer
    // Read the file into the buffer, at the end of the header
    // Calculate the CRC of the buffer
    // Put the CRC into the last two bytes of the buffer
    // Open the file
    // read the file
    // Send the file contents

    fileNameSize = name.size();

    if(fileNameSize < 0){
        printf("error in size reading\n");
    }
    else if(fileNameSize < FNSIZE){
        
        //fill fileName buffer
        fileName = name;
    }
    else{
        printf("file name is too big to send\n");
        printf("truncate file name and re-attempt\n");
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
    else if(fsize < FSIZE){
        fileSize = fsize;

        //fill payloadBuffer
        string line;

        //format header for globalstar message
        char * headerBuffer;
        int headerSize;
        headerSize = sprintf(headerBuffer, "%s%03d%s%06d", ESN,fileNameSize, fileName, fileSize);
        int bufferSize = headerSize + CRCSIZE + fileSize;
        messageBuffer = new char[bufferSize];
        copy(headerBuffer, headerBuffer+headerSize, messageBuffer);

        //read in from compressed file
        //append messagebuffer

        update_crc_16

        //calculate CRC
        //append CRC
    }
    else{
        printf("file is too big to send\n");
        printf("split message and re-attempt\n");
    }




}
