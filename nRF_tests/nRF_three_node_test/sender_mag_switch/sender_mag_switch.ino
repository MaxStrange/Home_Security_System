/**
This is the sketch for the sender that is connected to a magnetic switch.
It waits for the system_arm broadcast, and then starts sending either the ALL CLEAR
signal or the INTRUDER_ALERT signal.
*/
#include <SPI.h>
#include "RF24.h"

/**Pin declarations*/
const unsigned int GREEN = 8;
const unsigned int SWITCH = 3;

/**Radio code*/
const int TIMEOUT = 200;//ms to wait for timeout
RF24 radio(9,10);  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10 
byte talking_pipes[][6] = { "1Node", "2Node" };
byte broadcast_pipes[][6] = { "Node1", "Node2" };

/**Switch code*/
bool system_armed = false;
int switch_state = HIGH;
int val = HIGH;

void setup(void)
{
  Serial.begin(9600);
  
  /**Pin sets*/
  pinMode(GREEN, OUTPUT);
  pinMode(SWITCH, INPUT);
  
  /**Radio*/
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(talking_pipes[1]);
  radio.openReadingPipe(1, broadcast_pipes[0]);
  radio.startListening();
}

void loop(void)
{
  while (! system_armed)
  {
    check_messages();//wait around for the message to enter armed mode
  }
  
  Serial.println("ARMED");
  
  /**Poll the switch line to check for intruder*/
  check_for_entry();
  
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
    radio.read(&arm_the_system, sizeof(bool));
    //Serial.print("Received: "); Serial.println(arm_the_system);
  }
  
  system_armed = arm_the_system;
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
  bool intruder_alert = (switch_state == LOW);
  radio.write( &intruder_alert, sizeof(bool) );
  radio.startListening();
}
