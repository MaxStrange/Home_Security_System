/**
This is the sketch for the SENDER. Sends a packet to a receiver and blinks the green LED if it gets a response; red otherwise.
*/


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

const unsigned int RED = 6;
const unsigned int GREEN = 8;

const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };  // Radio pipe addresses for the 2 nodes to communicate.
typedef enum { role_ping_out = 1, role_pong_back } role_e;  // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};  // The debug-friendly names of those roles
role_e role = role_ping_out;  // The role of the current running sketch

void setup(void)
{
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  
  Serial.begin(57600);
  printf_begin();

  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);

  radio.printDetails();
}

void loop(void)
{
  //
  // Ping out role.  Repeatedly send the current time
  //

  if (role == role_ping_out)
  {
    radio.stopListening();  //Stop listening so we can write
    
    unsigned long time = millis();      // Take the time, and send it.  This will block until complete
    printf("Now sending %lu...",time);
    bool ok = radio.write( &time, sizeof(unsigned long) );
    
    if (ok)
      printf("ok...");
    else
      printf("failed.\n\r");

    radio.startListening();      // Now, continue listening

    // Wait here until we get a response, or timeout (200ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > TIMEOUT )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      printf("Failed, response timed out.\n\r");
      digitalWrite(RED, HIGH);
      delay(500);
      digitalWrite(RED, LOW);
      delay(500);
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );

      // Spew it
      printf("Got response %lu, round-trip delay: %lu\n\r", got_time, millis() - got_time);
      digitalWrite(GREEN, HIGH);
      delay(500);
      digitalWrite(GREEN, LOW);
      delay(500);
    }

    // Try again 1s later
    delay(1000);
  }
}
