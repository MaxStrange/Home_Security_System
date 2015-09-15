/**
This is the 0.2 version of the accumulator node. It should be
paired with the sender sketch found in the same folder as this sketch.

This version tests the countdowns and modes along with sleeping in between each interrupt from the radio.

The node hangs out until an interrupt from the nRF comes in. If it received a signal to enter
the all_clear mode, it enters all_clear regardless of what state it is currently in (effectively resetting it). If it receives
a danger signal, it ups its current danger level (all_clear -> alert -> intruder_detected). While in alert mode, it counts
down. If it doesn't get the next danger signal in that time, it resets to all_clear. While in intruder_detected mode, it
counts down from a different value. If it receives a danger signal during that time, it resets the counter. If it doesn't,
it resets to all_clear mode. Again, if at any time it receives the disarm signal, it enters the all_clear mode.
*/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <SPI.h>
#include "RF24.h"

/**Typedefs**/
typedef enum { ALL_CLEAR_MODE, ALERT_MODE, INTRUDER_DETECTED_MODE } alert_level_type;

/**Constants**/
const unsigned long ALARM_COUNTDOWN = 5000;//30,000 milliseconds (30 seconds)//The time the alarm sounds for
const unsigned long INTRUDER_COUNTDOWN = 7000;//45,000 milliseconds (45 seconds)//The amount of time to wait for a second threat signal before reset
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;

/**Pin declarations**/
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
byte node_ids[][6] = { "cmltr", "sensr" };

/**State**/
volatile alert_level_type danger_level = ALL_CLEAR_MODE;
volatile unsigned long countdown_timer = 0;

void setup(void)
{
  Serial.begin(115200);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);//Retry 15 times with a delay of 15 microseconds between attempts
  radio.openReadingPipe(1, node_ids[1]);
  radio.startListening();
  
  attachInterrupt(0, check_messages_ISR, LOW);//Attach an interrupt on a LOW signal on Pin 2
}

void loop(void)
{
  detachInterrupt(0);
  printAlertLevel();
  attachInterrupt(0, check_messages_ISR, LOW);
  
  delay(1000);
  if ((countdown_timer > 0) || (countdown_timer > INTRUDER_COUNTDOWN))
  {
    countdown_timer -= 1000;
  }
  else
  {
    countdown_timer = 0;
    danger_level = ALL_CLEAR_MODE;
    
    //when in all clear mode, go to sleep
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    Serial.println("Sleeping.");
    sleep_mode();
    Serial.println("Just woke up!");
  } 
}

void check_messages_ISR(void)
{
  Serial.println("ISR");
  bool tx, fail, rx;
  radio.whatHappened(tx, fail, rx);
  
  uint16_t signal;
  uint8_t reading_pipe_number;
  if (rx && radio.available(&reading_pipe_number))
  {
    radio.read(&signal, sizeof(uint16_t));
  }
    
  if (signal == THREAT_SIGNAL)
  {//upgrade the threat level
    switch (danger_level)
    {
      case ALL_CLEAR_MODE://start counting down while you wait for the next signal
        danger_level = ALERT_MODE;
        countdown_timer = INTRUDER_COUNTDOWN;
        break;
      case ALERT_MODE://start sounding the alarm
        danger_level = INTRUDER_DETECTED_MODE;
        countdown_timer = ALARM_COUNTDOWN;
        break;
      case INTRUDER_DETECTED_MODE:
        countdown_timer = ALARM_COUNTDOWN;
        break;
    }
  }
    
  if (signal == DISARM_SIGNAL)
  {//regardless of where we are, reset the system
    countdown_timer = 0;
    danger_level = ALL_CLEAR_MODE;
  }
}

void printAlertLevel(void)
{
  Serial.print("Danger level: ");
  switch (danger_level)
  {
    case ALL_CLEAR_MODE:
      Serial.println("ALL CLEAR");
      break;
    case ALERT_MODE:
      Serial.println("ALERT");
      break;
    case INTRUDER_DETECTED_MODE:
      Serial.println("INTRUDER DETECTED");
      break;
  }
  Serial.print("Countdown timer: ");
  Serial.println(countdown_timer / 1000);
}
