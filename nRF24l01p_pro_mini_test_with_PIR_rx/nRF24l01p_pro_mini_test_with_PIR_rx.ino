/**
This is the RECEIVER node. It waits for a signal from the sender that tells it that the PIR has sensed motion.
Then it outputs some info on the Serial port.
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };  // Radio pipe addresses for the 2 nodes to communicate.

void setup(void)
{
  Serial.begin(57600);
  
  radio.begin();
  radio.setRetries(15,15);
  
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  
  //radio.openWritingPipe(pipes[1]);
  //radio.openReadingPipe(1,pipes[0]);
  
  radio.startListening();
}

void loop(void)
{
  if ( radio.available() )
  {
    // Dump the payloads until we've gotten everything
    unsigned long got_item;
    bool done = false;
    while (!done)
    {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_item, sizeof(unsigned long) );
    }
    Serial.print("Got something: "); Serial.println(got_item);
  }
}
