#include <SPI.h>
#include "RF24.h"

/**Pin declarations*/
const unsigned int RED = 6;
const unsigned int GREEN = 8;
const unsigned int PIR = 4;
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Constants**/
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;

/**Radio code*/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
byte node_ids[][6] = { "cmltr", "1send", "2send" };

/**PIR code*/
int pir_state = LOW;
int val = 0;

void setup(void)
{
  /**Pin sets*/
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(PIR, INPUT);
    
  /**Radio*/
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(node_ids[1]);
  radio.startListening();
}

void loop(void)
{
  /**Poll the PIR line to check for motion*/
  check_for_motion();
  
  send_state();
  
  delay(250);
}

void check_for_motion(void)
{
  val = digitalRead(PIR);
  if (val == HIGH)
  {
    //Motion has been detected
    
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    
    if (pir_state == LOW)
    {
      //We just now detected motion
      pir_state = HIGH;
    }
  }
  else
  {
    //There is no motion - all is clear
    
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
    
    if (pir_state == HIGH)
    {
      //We just now stopped detecting motion
      pir_state = LOW;
    }
  }
}

void send_state(void)
{
  if (pir_state)
  {
    radio.stopListening();  //Stop listening so we can write
    radio.write( &THREAT_SIGNAL, sizeof(uint16_t) );
    radio.startListening();
  }
}
