/**
This is the 1send node. It is a purely sensor node with a mic
and PIR on it through a NOR gate.

It sleeps until it hears the arm signal, after which it
goes to sleep again, but with an interrupt to listen on its
NOR gate line to see if any intruders are present. If so, it
sends a threat signal to the accumulator node.

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
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;
const uint16_t ARM_SIGNAL = 0x1221;

/**Pin declarations**/
const int SENSORS = 3;//PIR and Mag switch through a NOR gate - so LOW when bad guy detected
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
byte node_ids[][6] = { "cmltr", "1send", "2send", "3send", "4send" };//Accumulator and four other nodes. Node 4send is the arm/disarm node

void setup(void)
{
  /**Pins**/
  pinMode(SENSORS, INPUT);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);//Retry 15 times with a delay of 15 microseconds between attempts
  radio.openWritingPipe(node_ids[1]);//open up a writing pipe to the accumulator node (the accumulator node reads pipes 1 through 4)
  radio.openReadingPipe(1, node_ids[4]);//arm/disarm node
  radio.startListening();
  
  attachInterrupt(0, check_messages_ISR, LOW);//Attach an interrupt on a LOW signal on Pin 2
}

void loop(void)
{
  //Just go to sleep - interrupts take care of everything
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
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
  radio.write(&THREAT_SIGNAL, sizeof(uint16_t));
  radio.startListening();
}

void check_messages_ISR(void)
{
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
