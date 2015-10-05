/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Example for Getting Started with nRF24L01+ radios. 
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two 
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting 
 * with the serial monitor and sending a 'T'.  The ping node sends the current 
 * time to the pong node, which responds by sending the value back.  The ping 
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "RF24.h"

/**Set up nRF24L01 radio on SPI bus plus pins 9 & 10**/
RF24 radio(9,10);

byte addresses[][6] = { "1Node", "2Node" };

bool role = 1;

bool radioNumber = 0;//change to zero for one of the radios and one for the other


void setup(void)
{
  Serial.begin(115200);
 
  Serial.println(F("RF24/examples/GettingStarted")); 
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node")); 
  
  radio.begin();
  radio.setRetries(15, 15);
  
  if (radioNumber)
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
  }
  else
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
  }
  
  radio.startListening();
}

void loop(void)
{
  if (role == 1)
  {
    radio.stopListening();
    
    Serial.println(F("Now sending."));
    
    unsigned long time = micros();
    if (!radio.write( &time, sizeof(unsigned long) ))
    {
      Serial.println(F("failed to send."));
    }
    
    radio.startListening();

    unsigned long started_waiting_at = micros();
    bool timeout = false;
    
    while ( ! radio.available() )
    {
      if (micros() - started_waiting_at > 500000 )
      {
        timeout = true;
        break;
      }
    }

    // Describe the results
    if ( timeout )
    {
      Serial.println(F("Failed, response timed out."));
    }
    else
    {
      unsigned long got_time;
      radio.read( &got_time, sizeof(unsigned long) );
      
      unsigned long time = micros();
      
      Serial.print(F("Sent: ")); Serial.print(time);
      Serial.print(F(", Got response ")); Serial.print(got_time);
      Serial.print(F(", Round-trip delay ")); Serial.print(time - got_time);
      Serial.println(F(" microseconds"));
    }

    // Try again 1s later
    delay(1000);
  }

  
  /*Pong back role.  Receive each packet, dump it out, and send it back*/
  

  if ( role == 0 )
  {
    unsigned long got_time;
    
    if ( radio.available() )
    {
      uint8_t pipe_num;
      while (radio.available(&pipe_num))
      {
        radio.read( &got_time, sizeof(unsigned long));
      }

      radio.stopListening();
      radio.write( &got_time, sizeof(unsigned long) );
      radio.startListening();
      
      Serial.print(F("Sent response: ")); Serial.println(got_time);
    }    
  }

  //
  // Change roles
  //

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == 0 )
    {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      role = 1;
    }
    else if ( c == 'R' && role == 1 )
    {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      role = 0;
      radio.startListening();
    }
  }
}

