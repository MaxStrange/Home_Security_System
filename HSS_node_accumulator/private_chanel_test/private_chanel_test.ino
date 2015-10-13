/**
Tests the private channel between the accumulator and 4send.
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
const unsigned int NUMBER_OF_NODES = 5;//Number of nodes in the system including this one

/**Pin declarations**/
const int SENSORS = 3;//PIR and Mag switch through a NOR gate - so LOW when bad guy detected
const int SPEAKER = 8;
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
//Accumulator and four other nodes. Node 4send is the arm/disarm node. The fifth is a private channel from node4 to accmltr
byte node_ids[][6] = { "cmltr", "1send", "2send", "3send", "4send" , "5send" };


void setup() 
{
    /**Pins**/
  pinMode(SENSORS, INPUT);
  pinMode(SPEAKER, OUTPUT);
  digitalWrite(SPEAKER, HIGH);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);//Retry 15 times with a delay of 15 microseconds between attempts
  radio.openReadingPipe(1, node_ids[1]);
  radio.openReadingPipe(2, node_ids[2]);
  radio.openReadingPipe(3, node_ids[3]);
  radio.openReadingPipe(4, node_ids[4]);//arm/disarm node broadcast channel
  radio.openReadingPipe(5, node_ids[5]);//private channel to arm/disarm node
  radio.startListening();
  
  attachInterrupt(0, check_messages_ISR, LOW);//Attach an interrupt on a LOW signal on Pin 2
  
  Serial.begin(115200);
}

void loop() 
{
  //nothing to do
}

void check_messages_ISR(void)
{
  Serial.println("Check messages.");
  bool tx, fail, rx = false;
  radio.whatHappened(tx, fail, rx);
  
  if (!rx)
  {
    Serial.println("Not rx, returning.");
    return;
  }
  
  uint16_t signal;
  uint8_t reading_pipe_number;
  if (radio.available(&reading_pipe_number))
  {
    radio.read(&signal, sizeof(uint16_t));
  }
  
  if (signal == DISARM_SIGNAL)
  {
    Serial.println("Disarm signal received.");
    digitalWrite(SPEAKER, HIGH);
    return;
  }
  else if (signal == ARM_SIGNAL)
  {
    Serial.println("Arm signal received.");
    digitalWrite(SPEAKER, HIGH);
    return;
  }
  else if (signal == THREAT_SIGNAL)
  {
    Serial.println("Threat received!");
    digitalWrite(SPEAKER, LOW);
    return;
  }
  else
  {
    Serial.println("Gobblydee gook received.");
  }
}
