/**
This is the sketch for the sender that is connected to a magnetic switch. It sends either the ALL CLEAR
signal or the INTRUDER_ALERT signal.
*/
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

/**Pin declarations*/
const unsigned int GREEN = 8;
const unsigned int SWITCH = 3;

/**Radio code*/
const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
const uint64_t talking_pipes[3] = { 0xF88145FA54, 0xF88145FAB1, 0xF88145FA23 };
const uint64_t listening_pipes[3] = { 0xE77034E943, 0xE77034E901, 0xE77034E91A };

/**Switch code*/
int switch_state = HIGH;
int val = HIGH;

void setup(void)
{
  /**Pin sets*/
  pinMode(GREEN, OUTPUT);
  pinMode(SWITCH, INPUT);
  
  /**Radio*/
  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(talking_pipes[1]);
  radio.openReadingPipe(2, listening_pipes[2]);
}

void loop(void)
{
  /**Poll the switch line to check for intruder*/
  check_for_entry();
  
  /**Regardless of what the PIR says, send something*/
  send_state();
  
  delay(1000);
}

void check_for_entry(void)
{
  val = digitalRead(SWITCH);
  if (val == LOW)
  {
    //Door is open
    
    digitalWrite(GREEN, LOW);
    
    if (switch_state == HIGH)
    {
      //We just now detected the intruder
      switch_state = LOW;
    }
  }
  else
  {
    //Door is closed - all is clear
    
    digitalWrite(GREEN, HIGH);
    
    if (switch_state == LOW)
    {
      //We just now stopped detecting alert
      switch_state = HIGH;
    }
  }
}

void send_state(void)
{
  radio.stopListening();  //Stop listening so we can write
  bool intruder_alert = (switch_state == LOW);
  bool ok = radio.write( &intruder_alert, sizeof(bool) );
  radio.startListening();
}
