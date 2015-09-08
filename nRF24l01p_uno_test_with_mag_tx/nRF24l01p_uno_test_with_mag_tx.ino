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
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };  // Radio pipe addresses for the 2 nodes to communicate.

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
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
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
  int state = ! switch_state;//The switch_state is opposite what the receiver expects//copy the state into a local variable - not important when polling, but when using interrupts - this will be critical
  bool ok = radio.write( &state, sizeof(int) );
  radio.startListening();//Must put the radio back in listening mode for this code to work
  

  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while (! radio.available() && ! timeout)
  {
    if (millis() - started_waiting_at > TIMEOUT)
      timeout = true;
  }
  
  if (timeout)
  {
  }
  else
  {
    unsigned long got_time_of_receipt;
    radio.read(&got_time_of_receipt, sizeof(unsigned long));
  }
}
