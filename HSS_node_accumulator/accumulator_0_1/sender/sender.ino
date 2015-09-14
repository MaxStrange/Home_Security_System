/**
This is the sender node for the 0.1 version of the accumulator.
*/
#include <SPI.h>
#include "RF24.h"

/**Constants**/
const uint8_t THREAT_SIGNAL = 0x1BA0;
const uint8_t DISARM_SIGNAL = 0x01E1;

/**Pin declarations**/
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
byte node_ids[][6] = { "cmltr", "sensr" };

void setup(void)
{
  Serial.begin(115200);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(node_ids[1]);
  radio.startListening();
}

void loop(void)
{
  Serial.println("Starting to send.");
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending disarm.");
  send_signal(DISARM_SIGNAL);
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending disarm.");
  send_signal(DISARM_SIGNAL);
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending threat.");
  send_signal(THREAT_SIGNAL);
  delay(1000);
  Serial.println("Sending disarm.");
  send_signal(DISARM_SIGNAL);
  delay(1000);
}

void send_signal(uint8_t to_send)
{
  radio.stopListening();
  radio.write(&to_send, sizeof(uint8_t));
  radio.startListening();
}
