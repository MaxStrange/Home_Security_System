#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <SPI.h>
#include "RF24.h"

/**Constants**/
const unsigned int SECONDS_TO_CLEAR_AREA = 3;//The number of seconds the system will give people to clear the area before arming after the arm button has been pushed
const unsigned int TIMES_TO_SEND = 6;//The number of times to try to send a signal before giving up
const unsigned int MAX_TIMES_TO_SEND_SIGNAL = 15;//The maximum number of times to send the threat signal to the accumulator per disarm/arm cycle
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;
const uint16_t ARM_SIGNAL = 0x1221;

/**Pin declarations**/
const int SENSORS = 3;//Mag switch NOR'd with a PIR signal
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
//Accumulator and four other nodes. Node 4send is the arm/disarm node. The fifth is a private channel from node4 to accmltr
byte node_ids[][6] = { "cmltr", "1send", "2send", "3send", "4send" , "5send" };

/**State**/
volatile boolean maybe_read_a_key = false;
volatile boolean disarm_flag = false;
volatile boolean arm_switch_flag = false;
volatile boolean arm_signal_heard_flag = false;
volatile boolean system_is_armed = false;
volatile unsigned int sent_signal_times = 0;//Number of times this node has sent the threat signal to the accumulator 

void setup(void)
{  
  /**Pins**/
  pinMode(SENSORS, INPUT);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);//Retry 15 times with a delay of 15 microseconds between attempts
  radio.openWritingPipe(node_ids[4]);//open up a broadcasting pipe - all other nodes listen to this pipe
  
  Serial.begin(115200);
}

void loop(void)
{
  if (disarm_flag)
  {//disarm the system - broadcast the disarm signal and then disarm
    Serial.println("Disarm flag.");
    disarm_system();
    disarm_flag = false;//clear the flag
    Serial.println("Disarm flag cleared.");
  }
  
  if (arm_switch_flag)
  {//arm the system - broadcast the arm signal and then arm yourself
    Serial.println("Arm switch flag.");
    arm_switch_flag = false;//clear the flag
    if (!system_is_armed)
      arm_system();
    else
      disarm_system();
    Serial.println("Arm switch flag cleared.");
  }
  
  if (arm_signal_heard_flag)
  {//arm this particular node - the system has been broadcasted to already by the rPi
    Serial.println("Arm signal heard flag.");
    arm_signal_heard_flag = false;//clear the flag
    arm_self();
    Serial.println("Arm signal heard flag cleared.");
  }
  
  //All done with work that needed to be done. Put the RFID/nRF interrupt back on and go to sleep.
  Serial.println("Sleep.");
  delay(1000);
  attachInterrupt(0, arm_disarm_ISR, LOW);
  set_sleep_mode(SLEEP_MODE_STANDBY);//standby works - but no deeper - only takes 6 clock cycles to boot back up
  sleep_enable();
  sleep_mode();
}

void arm_disarm_ISR(void)
{
  detachInterrupt(0);//Don't interrupt again until flags have been dealt with - this should also serve to debounce the arm switch
  detachInterrupt(1);
  
  /**Now check the radio**/
  bool tx, fail, rx = false;
  radio.whatHappened(tx, fail, rx);
    
  /**If rx, get what we received**/
  uint8_t reading_pipe_number;
  if (rx && radio.available(&reading_pipe_number))
  {
    uint16_t signal;
    radio.read(&signal, sizeof(uint16_t));
    
    if (signal == DISARM_SIGNAL)
      disarm_flag = true;
    else if (signal == ARM_SIGNAL)
      arm_signal_heard_flag = true;
  }
  
  /**If not rx, was it the radio? If so, don't care. But if it wasn't, then it MIGHT have been the arm switch.**/
  if (!fail && !tx)
    arm_switch_flag = true;//if it wasn't the software serial, it was the arm switch - check that later
}

void sensor_ISR(void)
{
  if (sent_signal_times < MAX_TIMES_TO_SEND_SIGNAL)
  {
    detachInterrupt(0);
    detachInterrupt(1);
    Serial.println("sensor detected something. Sending threat.");
    //If this ISR is fired, it is because we are armed and either the mag switch is open or the PIR saw something
    radio.stopListening();
    radio.openWritingPipe(node_ids[5]);//open the private channel between this node and the accumulator
    if (write_to_radio(&THREAT_SIGNAL, sizeof(uint16_t)))
      sent_signal_times++;
    radio.openWritingPipe(node_ids[4]);//reopen the broadcast channel - close the other channel
    maybe_read_a_key = false;//Reading keys is not why you woke up.
    attachInterrupt(0, arm_disarm_ISR, LOW);
    attachInterrupt(1, sensor_ISR, LOW);
  }
}

void arm_self(void)
{
  Serial.println("Arm self.");
  
  system_is_armed = true;
  attachInterrupt(1, sensor_ISR, LOW);//arm yourself too
}

void arm_system(void)
{
  Serial.println("Arm system.");
  
  /*Delay for how ever many seconds, checking for a key every second*/
    boolean do_not_arm = false;
    for (int i = SECONDS_TO_CLEAR_AREA; i > 0; i--)
    {
      Serial.print("Countdown: "); Serial.println(i);
      delay(1000);
    }
    
    if (!do_not_arm)
    {
      broadcast_signal(ARM_SIGNAL);
      arm_self();
    }
}

void disarm_system(void)
{
  detachInterrupt(1);//detach sensor ISR
  Serial.println("Disarm system.");
  broadcast_signal(DISARM_SIGNAL);
  system_is_armed = false;
  sent_signal_times = 0;
}

void broadcast_signal(uint16_t signal)
{
  Serial.println("Broadcast signal.");
  radio.stopListening();
  write_to_radio(&signal, sizeof(uint16_t));
}

boolean write_to_radio(const void * to_write, uint8_t len)
{
  Serial.println("Write to radio.");
  
  for (int i = 0; i < TIMES_TO_SEND; i++)
  {
    Serial.print("Write: "); Serial.println(i);
    if (radio.write(to_write, len))//This is the line that is buggy. It hangs here eventually, but only when trying to disarm the system. And not necessarily on the first call to this from the disarm.
    {
      return true;
    }
    
    delay(10);
  }
  
  Serial.println("Returning false.");
  return false;
}
