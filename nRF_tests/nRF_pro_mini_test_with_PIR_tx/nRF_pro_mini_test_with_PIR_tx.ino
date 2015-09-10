/**
This is the sketch for the SENDER. Sends a packet to a receiver and blinks the green LED if it gets a response; red otherwise.
*/
#include <SPI.h>
#include "RF24.h"

/**Pin declarations*/
const unsigned int RED = 6;
const unsigned int GREEN = 8;
const unsigned int PIR = 4;

/**Radio code*/
const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
byte pipes[][6] = { "1Node", "2Node" };

/**PIR code*/
int pirState = LOW;
int val = 0;

void setup(void)
{
  /**Pin sets*/
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(PIR, INPUT);
    
  /**Radio*/
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
}

void loop(void)
{
  /**Poll the PIR line to check for motion*/
  check_for_motion();
  
  /**Regardless of what the PIR says, send something*/
  send_state();
  
  delay(1000);
}

void check_for_motion(void)
{
  val = digitalRead(PIR);
  if (val == HIGH)
  {
    //Motion has been detected
    
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    
    if (pirState == LOW)
    {
      //We just now detected motion
      pirState = HIGH;
    }
  }
  else
  {
    //There is no motion - all is clear
    
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
    
    if (pirState == HIGH)
    {
      //We just now stopped detecting motion
      pirState = LOW;
    }
  }
}

void send_state(void)
{
  radio.stopListening();  //Stop listening so we can write
  int state = pirState;
  radio.write( &state, sizeof(int) );
  radio.startListening();      // Now, continue listening

  unsigned long started_waiting_at = millis();
  bool timeout = false;

  while ( ! radio.available() )
  {
    if (millis() - started_waiting_at > TIMEOUT )
    {
      timeout = true;
      break;
    }
  }

  // Describe the results
  if ( timeout )
  {
    //be sad... but actually do something to indicate a lost connection. Turn on an error LED maybe. Figure out later.
  }
  else
  {
    // check for a time stamp response
    unsigned long got_time_of_receipt;
    radio.read( &got_time_of_receipt, sizeof(unsigned long) );
 }
}
