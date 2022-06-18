#include <IRremote.h>

const int RECV_PIN = 7;


void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  enableLEDFeedback();
}

void loop(){
  if (IrReceiver.decode())
  {
    uint8_t data = IrReceiver.decodedIRData.command;
    Serial.write(data);
    Serial.write("\r\n");
    //Serial.println(data);
    //IrReceiver.printIRResultShort(&Serial); // optional use new print version
    IrReceiver.resume(); // Enable receiving of the next value
    //SendData(data);
    delay(250);
  }
}

void SendData(uint16_t command)
{
  char* p = (char*)command;
  Serial.write(p,2);
}
