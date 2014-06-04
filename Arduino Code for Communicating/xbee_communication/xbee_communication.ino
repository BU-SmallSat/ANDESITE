#include <SoftwareSerial.h>

//Declare pins for Serial communication
SoftwareSerial out(2, 3);

int data;

//Declare constant 64-bit addresses of xbees
  int SH_COORD = 0x0013A200;
  int SL_COORD = 0x407B6C7D;
  int SH_ROUTER_ZERO = 0x0013A200;
  int SL_ROUTER_ZERO = 0x4078EFB8;
  int SH_ROUTER_ONE = 0x0013A200;
  int SL_ROUTER_ONE = 0x409BB9E7;

void setup()
{
  Serial.begin(9600);
  out.begin(9600);
}

void loop()
{
  uint8_t payload[] = {'H', 'i'};
  transmitPacket(payload, sizeof(payload));
  
  if(out.available() > 4)
  {
    data = out.read();
    if(data == 0x7E)
    {
      Serial.print(data, HEX);
      Serial.print(", ");
      data = out.read();
      while(data != 0xFFFF)
      {
        Serial.print(data, HEX);
        Serial.print(", ");
        data = out.read();
      }
  //Serial.print(out.read(), HEX);
  Serial.println();}
  }
}

void transmitPacket(uint8_t *payload, int payloadLength)
{
  uint8_t packetFrame[] = {0x7E, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 
                      0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 
                      0x00};//0x48, 0xAB};
  int packetLength = payloadLength + sizeof(packetFrame) + 1;
  uint8_t packetToSend[packetLength];
  int n;
  for(n = 0; n < sizeof(packetToSend); n++)
  {
    packetToSend[n] = packetFrame[n];
  }
  
  for(int i = 17; i < packetLength - 1; i++)
  {
    packetToSend[i] = payload[i-17];
  }
  //int packetLength = sizeof(packetToSend);
  int lsb =  packetLength - 4;
  packetToSend[2] = lsb;
  int checksum = getChecksum(packetToSend, packetLength);
  packetToSend[packetLength-1] = checksum;
  
  for(int x = 0; x < packetLength; x++)
  {
    out.write((byte)packetToSend[x]);
  }
}

int getChecksum(uint8_t *packet, int length)
{
  int sum = 0x0;
  for(int x = 3; x < length - 1; x++)
  {
    sum += packet[x];
  }
  int checksum = 0xFF - (byte)sum;
  return checksum;
}
