/**
This is the 4send node. It is the node responsible for sending
the arm/disarm signal to the rest of the nodes. It also
has sensors (PIR and magnetic switch) through a NOR
gate.

It sleeps until an interrupt on pin 2 wakes it, at which
point it finds out why by checking the software serial.
If the software serial is available, it is because there
is an RFID key waiting to be scanned. It scans the key
and checks it against known keys. If it matches, it
broadcasts the disarm signal. Otherwise it ignores it.

If it wakes up and the software serial is not waiting,
it checks the nRF to see if it received something.
If it received the disarm signal, it broadcasts the
disarm signal itself and then disarms and goes back to
sleep.

If it wakes up and finds none of the other reasons to be
why, it must be because the arm switch has been pushed.
It broadcasts the arm signal and arms itself.

Once armed, it sleeps and waits for interrupts on pin 2
as described above AND on pin 3, where it is waiting
for sensor input. If it receives an interrupt on pin 3,
it sends a threat signal to the accumulator node.
*/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "RF24.h"

/**Constants**/
const unsigned int TIMES_TO_SEND = 6;//The number of times to try to send a signal before giving up
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;
const uint16_t ARM_SIGNAL = 0x1221;

/**Pin declarations**/
const int SENSORS = 3;//Mag switch NOR'd with a PIR signal
const int RFID_RX = 4;//RFID information comes in on this pin
const int RFID_TX = 5;//Makes softwareserial happy, but is actually not used
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Radio code**/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
//Accumulator and four other nodes. Node 4send is the arm/disarm node. The fifth is a private channel from node4 to accmltr
byte node_ids[][6] = { "cmltr", "1send", "2send", "3send", "4send" , "5xxxx" };

/**RFID code**/
SoftwareSerial RFID(RFID_RX, RFID_TX);
int key_a[14] = { 2, 55, 67, 48, 48, 53, 54, 56, 51, 54, 65, 67, 51, 3 };
int key_b[14] = { 2, 55, 67, 48, 48, 49, 57, 68, 56, 66, 51, 48, 69, 3 };
int key_c[14] = { 2, 55, 67, 48, 48, 53, 54, 57, 53, 55, 57, 67, 54, 3 };
int incoming_key[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // used for read comparisons

/**State**/
volatile boolean maybe_read_a_key = false;
volatile boolean disarm_flag = false;
volatile boolean arm_switch_flag = false;
volatile boolean arm_signal_heard_flag = false;

void setup(void)
{
  /**Pins**/
  pinMode(SENSORS, INPUT);
  
  /**Radio**/
  radio.begin();
  radio.setRetries(15, 15);//Retry 15 times with a delay of 15 microseconds between attempts
  radio.openWritingPipe(node_ids[4]);//open up a broadcasting pipe - all other nodes listen to this pipe
  
  /**RFID**/
  RFID.begin(9600);
  
  Serial.begin(115200);
}

void loop(void)
{
  if (maybe_read_a_key)
  {//woke up and found that an RFID might be waiting.
    delay(250);//fully wake up and get an accurate reading on the RFID
    
    Serial.println("Woke up and going to try to read a key.");
    
    for (int i = 0; i < 20; i++)
    {//Check several times to see if the software serial is ready with a known key
      if (read_incoming())  //a known key means disarm the system
      {
        Serial.println("Read a known key. Broadcasting the disarm signal.");
        broadcast_signal(DISARM_SIGNAL);
        arm_switch_flag = false;//If the software serial is what woke us up, it wasn't the arm switch - and even if it was, we obviously don't want to arm
        break;//Stop trying to read the key
      }
    }
    
    maybe_read_a_key = false;//clear the flag
  }
  
  if (disarm_flag)
  {//disarm the system - broadcast the disarm signal and then disarm
    Serial.println("Broadcasting the disarm signal.");
    broadcast_signal(DISARM_SIGNAL);
    disarm_flag = false;//clear the flag
  }
  
  if (arm_switch_flag)
  {//arm the system - broadcast the arm signal and then arm yourself
    Serial.println("The arm switch was thrown. Going to give you 30 seconds to leave the area before broadcasting the arm signal.");
    delay(30000);//delay half a minute to give people time to leave the area before telling the system to arm
    
    Serial.println("Broadcasting the arm signal.");
    broadcast_signal(ARM_SIGNAL);
    arm_switch_flag = false;//clear the flag
    Serial.println("Attaching my own sensor interrupt.");
    attachInterrupt(1, sensor_ISR, LOW);//arm yourself too
  }
  
  if (arm_signal_heard_flag)
  {//arm this particular node - the system has been broadcasted to already by the rPi
    Serial.println("Raspberry pi sent arm signal. Going to arm now.");
    arm_signal_heard_flag = false;//clear the flag
    attachInterrupt(1, sensor_ISR, LOW);//arm yourself
  }
  
  //All done with work that needed to be done. Put the RFID/nRF interrupt back on and go to sleep.
  Serial.println("All done with work. Going to sleep.");
  attachInterrupt(0, arm_disarm_ISR, LOW);
  set_sleep_mode(SLEEP_MODE_STANDBY);//standby works - but no deeper - only takes 6 clock cycles to boot back up
  sleep_enable();
  sleep_mode();
}

void arm_disarm_ISR(void)
{
  /**Can't tell in an interrupt if the software serial is ready - set a flag and check it when done with the ISR**/
  maybe_read_a_key = true;
  
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
  
  detachInterrupt(0);//Don't interrupt again until flags have been dealt with - this should also serve to debounce the arm switch
  detachInterrupt(1);
}

void sensor_ISR(void)
{
  //If this ISR is fired, it is because we are armed and either the mag switch is open or the PIR saw something
  radio.stopListening();
  radio.openWritingPipe(node_ids[5]);//open the private channel between this node and the accumulator
  write_to_radio(&THREAT_SIGNAL, sizeof(uint16_t));
  radio.openWritingPipe(node_ids[4]);//reopen the broadcast channel - close the other channel
}

void broadcast_signal(uint16_t signal)
{
  radio.stopListening();
  write_to_radio(&signal, sizeof(uint16_t));
}

boolean read_incoming(void)
{
  if (RFID.available())
  {
    delay(100);
    
    for (int i = 0; i < 14; i++)
    {
      incoming_key[i] = RFID.read();
    }
    RFID.flush();//stops multiple reads
    
    return check_incoming();
  }
}

boolean check_incoming(void)
{
  if (compare_keys(key_a, incoming_key))
    return true;
  else if (compare_keys(key_b, incoming_key))
    return true;
  else if (compare_keys(key_c, incoming_key))
    return true;
  else
    return false;
}

boolean compare_keys(int aa[14], int bb[14])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < 14 ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == 14)
  {
    ff = true;
  }
  return ff;
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
