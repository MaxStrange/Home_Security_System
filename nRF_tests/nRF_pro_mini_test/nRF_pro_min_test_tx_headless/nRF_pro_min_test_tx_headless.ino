/**
This is a SENDER node. Sends a packet to headless receiver and blinks a green LED if it gets something back; red otherwise.
*/

#include <SPI.h>
#include "RF24.h"

const unsigned int RED = 8;
const unsigned int GREEN = 6;

const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
byte pipes[][6] = { "1Node", "2Node" };


void setup(void)
{
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
}

void loop(void)
{
  /**This is the sending node. **/
  
  radio.stopListening();  //Stop listening so we can write
  unsigned long time = millis();      // Take the time, and send it.  This will block until complete
  radio.write( &time, sizeof(unsigned long) );
  radio.startListening();      // Now, continue listening

  bool timeout = false;

  unsigned long started_waiting_at = millis();
  while ( ! radio.available() )
  {
    if (millis() - started_waiting_at > TIMEOUT)
    {
      timeout = true;
      break;
    }
  }

  // Describe the results
  if ( timeout )
  {
    digitalWrite(RED, HIGH);
    delay(500);
    digitalWrite(RED, LOW);
    delay(500);
  }
  else
  {
    unsigned long got_time;
    radio.read( &got_time, sizeof(unsigned long) );

    digitalWrite(GREEN, HIGH);
    delay(500);
    digitalWrite(GREEN, LOW);
    delay(500);
  }

  // Try again 1s later
  delay(1000);
}
