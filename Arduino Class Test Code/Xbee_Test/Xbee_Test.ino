#include <Time.h>
#include <Xbee.h>

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  byte data[] = {0x5A, 0xFE, 0xDA, 0x7A};
  byte txCommand[19 + sizeof(data) + 1];
  Xbee().txCommandPacket(txCommand, 0x00031F9A, 0xFF34039B, 0xFFFE, 
    0x01, 0x01, 0x03, data, sizeof(data));
  printPacket(txCommand, sizeof(txCommand));
  
  byte txDataPacket[28];
  Xbee().txDataPacket(txDataPacket, 0x00031F9A, 0xFF34039B, 0xFFFE, 
    0x02, getTime(), 0xDA7A);
  printPacket(txDataPacket, sizeof(txDataPacket));
  
  byte atLocal1[8];
  char command[] = {'M', 'Y'};
  Xbee().atLocalCommand(atLocal1, 0x01, command);
  printPacket(atLocal1, sizeof(atLocal1));
  
  char parameters[] = {'Z'};
  byte atLocal2[7 + sizeof(parameters) + 1];
  Xbee().atLocalCommand(atLocal2, 0x02, command, parameters);
  printPacket(atLocal2, sizeof(atLocal2));
  
  byte atRemote1[19];
  Xbee().atRemoteCommand(atRemote1, 0x00031F9A, 0xFF34039B, 0xFFFE, 
    0x01, 0x00, command);
  printPacket(atRemote1, sizeof(atRemote1));
  
  byte atRemote2[18 + sizeof(parameters) + 1];
  Xbee().atRemoteCommand(atRemote2, 0x00031F9A, 0xFF34039B, 0xFFFE, 
    0x01, 0x00, command, parameters);
  printPacket(atRemote2, sizeof(atRemote2));
  
  Serial.println();
}

void printPacket(byte packet[], int packetLength)
{
  for (int i = 0; i < packetLength; i++) {
     if(packet[i] < 0x10) {
       Serial.print('0');
     }
     Serial.print(packet[i], HEX);
     Serial.print(" ");
   }
   Serial.println();
}

String getTime()
{
  time_t t = now();
  String time = String(year(t) % 100);
  if(month(t) < 10) {
    time += '0';
  }
  time += String(month(t));
  if(day(t) < 10) {
    time += '0';
  }
  time += String(day(t));
  if(hour(t) < 10) {
    time += '0';
  }
  time += String(hour(t));
  if(minute(t) < 10) {
    time += '0';
  }
  time += String(minute(t));
  if(second(t) < 10) {
    time += '0';
  }
  time += String(second(t));
  return time;
}
