/**
This sketch is to test using a PIR sensor with an interrupt to wake it up from sleep mode.
Since it uses the Arduino's external interrupts, it requires that the pin used be the
one specified (3).
*/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>



/**Pins*/
const unsigned int LED = 13;                // choose the pin for the LED
const unsigned int PIR = 3;               // choose the input pin (for PIR sensor)

/**Interrupts*/
const unsigned int PIR_INTERRUPT = 1;//meaning, pin 3's external interrupt

/**Variables*/
int pir_state = HIGH;             // we start, assuming no motion detected (with an inverted signal)
volatile bool motion_flag = false; //Flag to let the Arduino know that motion has been detected



void setup(void)
{
  pinMode(LED, OUTPUT);      // declare LED as output
  pinMode(PIR, INPUT);     // declare sensor as input
  
  attachInterrupt(PIR_INTERRUPT, pir_interrupt, LOW);//attach interrupt 1 (pin 3) - must use LOW as the wake up
}


 
void loop(void)
{
  if (motion_flag)
  {
    /**Deal with intruder*/
    
    delay(1000);//Keep the LED on for a second
    motion_flag = false;
    
    /**Now that intruder is dealt with, turn on interrupt again to make sure to catch the next bad guy*/
    attachInterrupt(PIR_INTERRUPT, pir_interrupt, LOW);
  }
  
  sleep_now();//turn off the LED and save power until a PIR signal wakes you up!
}

void pir_interrupt(void)
{
  if (motion_flag == false)
  {
    digitalWrite(LED, HIGH);
    motion_flag = true;
    detachInterrupt(PIR_INTERRUPT);//detach the interrupt or else it will just keep firing now that the signal is LOW
  }
  //ignore motion unless it is new to us
}

void sleep_now(void)
{
  set_sleep_mode(SLEEP_MODE_STANDBY);//PWR_DOWN is better, but may not work - figure out later
  sleep_enable();//Enables the sleep bit
  digitalWrite(LED, LOW);//turn off the LED to show that you are going to sleep
  sleep_mode();//Actually go to sleep
  
  sleep_disable();//First thing to do when you wake up!
}
