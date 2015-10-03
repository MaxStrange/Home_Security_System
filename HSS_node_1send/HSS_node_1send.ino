/**
This is the 1send node. It is a purely sensor node with a mic
and a PIR that has been NOT'ed.

It sleeps until it hears the arm signal, after which it
goes to sleep again, but with an interrupt to listen on its
NOR gate line to see if any intruders are present. If so, it
sends a threat signal to the accumulator node.

It also listens on pin 2 for nRF and microphone interrupts. If it wakes up
because of an interrupt on that pin, it checks the amplitude of the noise
to see if it was a loud noise that woke it up. If it was, it sends a threat signal.

If it ever receives the disarm signal, it goes back to sleeping
and waiting for the arm signal.
*/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <SPI.h>
#include "RF24.h"

/**Constants**/
const unsigned int TIMES_TO_SEND = 6;//The number of times to try to send a message before giving up
const unsigned int THRESHOLD_AMPLITUDE = 50;//needs to be fiddled with
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;
const uint16_t ARM_SIGNAL = 0x1221;

/**Pin declarations**/
const int PIR = 3;
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;
const int AMPLITUDE = A0;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
//Accumulator and four other nodes. Node 4send is the arm/disarm node. The fifth is a private channel from node4 to accmltr
byte node_ids[][6] = { "cmltr", "1send", "2send", "3send", "4send" , "5xxxx" };

void setup(void)
{
  /**Pins**/
  pinMode(PIR, INPUT);
  pinMode(AMPLITUDE, INPUT);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);//Retry 15 times with a delay of 15 microseconds between attempts
  radio.openWritingPipe(node_ids[1]);//open up a writing pipe to the accumulator node (the accumulator node reads pipes 1 through 4)
  radio.openReadingPipe(1, node_ids[4]);//arm/disarm node
  radio.startListening();
  
  attachInterrupt(0, check_wake_up_ISR, LOW);//Attach an interrupt on a LOW signal on Pin 2
}

void loop(void)
{
  //Just go to sleep - interrupts take care of everything
  set_sleep_mode(SLEEP_MODE_STANDBY);//PWR_DOWN probably takes too long to catch the amplitude signal when waking up
  sleep_enable();
  sleep_mode();
}

void arm_system(void)
{
  attachInterrupt(1, sensor_ISR, LOW);//Attach an interrupt on a LOW signal on Pin 3
}

void disarm(void)
{
  detachInterrupt(1);//Don't check sensors until armed again
}

void sensor_ISR(void)
{
  //Bad guys are present! Send a threat signal!
  radio.stopListening();
  write_to_radio(&THREAT_SIGNAL, sizeof(uint16_t));
  radio.startListening();
}

void check_wake_up_ISR(void)
{
  /**Check the amplitude of the audio line to see if that was what woke us up**/
  if (analogRead(AMPLITUDE) > THRESHOLD_AMPLITUDE)
  {
    //Bad guys! Warn the others!
    radio.stopListening();
    write_to_radio(&THREAT_SIGNAL, sizeof(uint16_t));
    radio.startListening();
  }
  
  /**Now check the radio, regardless of if it was noise that woke you up - to see if it wants you to disarm.**/
  
  /**Check what happened to trigger the interrupt**/
  bool tx, fail, rx = false;
  radio.whatHappened(tx, fail, rx);
  
  if (!rx)
    return;//If it wasn't rx, I don't care what it was.
  
  /**If rx, get what we received**/
  uint16_t signal;
  uint8_t reading_pipe_number;
  if (radio.available(&reading_pipe_number))
    radio.read(&signal, sizeof(uint16_t));
    
  if (signal == DISARM_SIGNAL)
  {
    disarm();
  }
  else if (signal == ARM_SIGNAL)
  {
    arm_system();
  }
}

boolean write_to_radio(const void * to_write, uint8_t len)
{
  for (int i = 0; i < TIMES_TO_SEND; i++)
  {
    if (radio.write(to_write, len))
      return true;
  }
  return false;
}
