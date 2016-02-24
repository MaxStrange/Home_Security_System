/**
!!Freezes if the card is held too long in front of the
reader!!

Code to make sure that sleeping and waking up
with the RFID works indefinitely.

Processor goes to sleep, and wakes up whenever a card
is ready to be read. It reads the card once, then goes
back to sleep.

Attach IDLA20's TIR (FUTURE) to a NOT and then to pin 2 on Arduino.
Attach IDLA20's D1 to pin RX on Arduino (make sure not to do this until after you have loaded this code).
*/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

const int LED = 13;
const int KEY_LENGTH = 16;
int key_a[KEY_LENGTH] = { 63, 100, 94, 230, 166, 102, 230, 76, 102, 166, 201, 201, 202, 214, 242, 0 };
int key_b[KEY_LENGTH] = { 63, 100, 94, 230, 166, 102, 166, 76, 153, 166, 76, 153, 202, 214, 242, 0 };
int key_c[KEY_LENGTH] = { 63, 100, 94, 230, 166, 166, 204, 151, 204, 201, 62, 234, 202, 214, 242, 0 };
int incoming_key[KEY_LENGTH] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

void setup(void)
{
  pinMode(LED, OUTPUT);
  pinMode(2, INPUT);//card present signal is read on this pin
  
  Serial.begin(9600);  
}

void loop(void)
{
//  Serial.println("Sleep.");
  delay(1000);
  attachInterrupt(0, read_key_ISR, LOW);
  go_to_sleep();
  
  
  //Serial.println("Read RFID");
  if (read_incoming())
  {
    digitalWrite(LED, HIGH);
    //Serial.println("Access granted.");
    delay(1000);
    digitalWrite(LED, LOW);
  }
}

void read_key_ISR(void)
{
  detachInterrupt(0);
}

void go_to_sleep(void)
{
  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();
  sleep_mode();
}

boolean read_incoming(void)
{
  delay(200);
  if (Serial.available())
  {
    for (int i = 0; i < KEY_LENGTH; i++)
    {
      incoming_key[i] = Serial.read();
    }
    Serial.flush();
    
    if (check_incoming())
      return true;
  }
  return false;
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

boolean compare_keys(int aa[16], int bb[16])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < 16 ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == 16)
  {
    ff = true;
  }
  return ff;
}
