/**
This sketch is for the PIR sender. It waits for a signal from
the receiver node and then starts sending its current
PIR signal.
*/
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

/**Pin declarations*/
const unsigned int RED = 6;
const unsigned int GREEN = 8;
const unsigned int PIR = 4;

/**Radio code*/
const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
const uint64_t talking_pipes[3] = { 0xF88145FA54, 0xF88145FAB1, 0xF88145FA23 };
const uint64_t listening_pipes[3] = { 0xE77034E943, 0xE77034E901, 0xE77034E91A };

/**PIR code*/
bool system_armed = false;
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
  radio.setRetries(15,15);
  radio.openWritingPipe(talking_pipes[0]);
  radio.openReadingPipe(1, listening_pipes[1]);
  radio.startListening();
}

void loop(void)
{
  while (! system_armed)
  {
    check_messages();//wait around until armed
  }
  
  /**Poll the PIR line to check for motion*/
  check_for_motion();
  
  /**Regardless of what the PIR says, send something*/
  send_state();
  
  delay(1000);
}

void check_messages(void)
{
  bool arm_the_system = system_armed;
  
  uint8_t pipe_number;
  if (radio.available(&pipe_number))
  {
    bool done = false;
    while (!done)
    {
      done = radio.read(&arm_the_system, sizeof(bool));
    }
  }
  
  system_armed = arm_the_system;
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
  bool intruder_alert = (pirState == HIGH);
  bool ok = radio.write( &intruder_alert, sizeof(int) );
  radio.startListening();
}
