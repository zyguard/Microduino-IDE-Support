/*
  [head,2byte,0xAA 0xBB] [type,1byte,TYPE_NUM] [data,16byte] [body,1byte,getChecksum()]
  Example:
  AA BB C8 DC 05 DC 05 D0 07 EF 03 DC 05 DC 05 DC 05 DC 05 E3
*/

#include <Microduino_Protocol.h>

ProtocolSer protocolB(&Serial1, 16);

uint16_t recData[8];
uint8_t recCmd;

void setup() {
  Serial.begin(9600);
  protocolB.begin(9600);  //9600/19200/38400
}

void loop() {
  
  if(protocolB.available())
  {
    protocolB.readWords(&recCmd, recData, 8);
    Serial.println("protocolB Received !");
    Serial.print("recCmd: ");
    Serial.print(recCmd);
    Serial.print("  Data:");
    for(uint8_t i=0; i<8; i++)
    {
      Serial.print(" ");
      Serial.print(recData[i]);
    }
    Serial.println();
  }
}
