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
    IrReceiver.resume(); // Enable receiving of the next value
    delay(250);
  }
}
