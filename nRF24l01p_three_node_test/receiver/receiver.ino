/**
This sketch is for the receiver in the three node topology. It listens to the other two nodes
and reacts accordingly (lights its red LED when intruder alert, green otherwise). It doesn't
send any response back though (at least not in this version).
*/
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

/**Pin declarations*/
const unsigned int RED = 8;
const unsigned int GREEN = 6;

/**Radio code*/
const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
const uint64_t talking_pipes[3] = { 0xF88145FA54, 0xF88145FAB1, 0xF88145FA23 };
const uint64_t listening_pipes[3] = { 0xE77034E943, 0xE77034E901, 0xE77034E91A };

bool alert_mode_is_on = false;//Whether an intruder has been detected

void setup(void)
{
  /**Pin sets*/
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
    
  /**Radio*/
  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(talking_pipes[2]);
  radio.openReadingPipe(1, talking_pipes[0]);
  radio.openReadingPipe(2, talking_pipes[1]);
  radio.startListening();
}

void loop(void)
{
  display_alert_status();
  check_messages();
}

void check_messages(void)
{
  bool intruder_alert = alert_mode_is_on;
  
  uint8_t pipe_number;//in case you want it
  if (radio.available(&pipe_number))
  {
    //Dump the payloads until we've gotten everything
    bool done = false;
    while (!done)
    {
      //Fetch the payload and see if this was the last one
      done = radio.read(&intruder_alert, sizeof(bool));
    }
  }
  
  alert_mode_is_on = intruder_alert;
}

void display_alert_status(void)
{
  if (alert_mode_is_on)
  {
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
  }
  else
  {
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
  }
}
