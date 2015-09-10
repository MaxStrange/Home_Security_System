/**
This is the RECEIVER node. It waits for a signal from the sender that tells it that the PIR has sensed motion.
Then it outputs some info on the Serial port.
*/

#include <SPI.h>
#include "RF24.h"

/**Pin setup*/
const unsigned int RED = 8;
const unsigned int GREEN = 6;

/**PIR*/
int pirState = LOW;

/**Radio*/
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
byte pipes[][6] = { "1Node", "2Node" };


void setup(void)
{
  /**Pin directions*/
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  /**Radio*/
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  
  Serial.begin(115200);
}

void loop(void)
{
  if ( radio.available() )
  {
    int got_state;
    while (radio.available())
    {
        radio.read( &got_state, sizeof(int) );
    }
    
    pirState = got_state;
    Serial.println(got_state);
    
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
