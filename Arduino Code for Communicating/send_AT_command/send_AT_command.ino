#include <ATCommand.h>
#include <Common.h>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  sendCommand(0x01, 0x0000, 0x00FF, 0xFFFE, 0x02, 'M', 'Y');
  //getATResponse(2);
  
  delay(1000);
}

void sendCommand(byte frameID, long SH, long SL, int MY, byte options, char c1, char c2)
{
  ATCommand command(frameID, SH, SL, MY, options, c1, c2);
  int commandLength = command.getCommandLength();
  byte commandPacket[commandLength];
  for(int i = 0; i < commandLength; i++) {
    commandPacket[i] = command.getATCommand(i);
  }
  printPacket(commandPacket, commandLength);
}

void getATResponse(int registerLength)
{
  int responseLength = 8 + registerLength;
  byte response[responseLength];
  
  for(int i = 0; i < responseLength; i++) {
    response[i] = Serial.read();
  }
  printPacket(response, responseLength);
}

void printPacket(byte packet[], int packetLength)
{  
  for(int i = 0; i < packetLength; i++) {
    if(packet[i] < 0x10) {
      Serial.print('0');
    }
    Serial.print(packet[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
}

