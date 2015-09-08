/**
This is the RECEIVER node. It waits for a signal from the sender that tells it that the PIR has sensed motion.
Then it outputs some info on the Serial port.
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

/**Pin setup*/
const unsigned int RED = 8;
const unsigned int GREEN = 6;

/**PIR*/
int pirState = LOW;

/**Radio*/
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };  // Radio pipe addresses for the 2 nodes to communicate.


void setup(void)
{
  /**Pin directions*/
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  /**Radio*/
  radio.begin();
  radio.setRetries(15,15);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
}

void loop(void)
{
  //if data is available
  if ( radio.available() )
  {
    // Dump the payloads until we've gotten everything
    int got_state;
    bool done = false;
    while (!done)
    {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_state, sizeof(int) );

 	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
    }
    
    pirState = got_state;
    
    if (pirState == HIGH)
    {
      //Intruder alert!
      digitalWrite(RED, HIGH);
      digitalWrite(GREEN, LOW);
    }
    else
    {
      //All clear
      digitalWrite(RED, LOW);
      digitalWrite(GREEN, HIGH);
    }

    radio.stopListening();
    unsigned long time_stamp = millis();
    radio.write( &time_stamp, sizeof(unsigned long) ); 
    radio.startListening();
  }
}
