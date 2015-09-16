/**
This is the v0.1 HSS_node_4send. It tests going to sleep
and then waking up to read the RFID.
*/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <SoftwareSerial.h>


SoftwareSerial RFID(4, 5); // RX and TX

int key_a[14] = { 2, 55, 67, 48, 48, 53, 54, 56, 51, 54, 65, 67, 51, 3 };
int key_b[14] = { 2, 55, 67, 48, 48, 49, 57, 68, 56, 66, 51, 48, 69, 3 };
int key_c[14] = { 2, 55, 67, 48, 48, 53, 54, 57, 53, 55, 57, 67, 54, 3 };
int incoming_key[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // used for read comparisons

volatile boolean ready_to_read = false;
volatile boolean access_flag = false;

void setup(void)
{
  RFID.begin(9600);
  Serial.begin(9600);
  
  attachInterrupt(1, read_in_queue_ISR, LOW);
}

void loop(void)
{
  if (ready_to_read)
  {
    delay(250);
    read_incoming();
    ready_to_read = false;
    Serial.print("Access granted: "); Serial.println(access_flag ? "true" : "false");
    attachInterrupt(1, read_in_queue_ISR, LOW);
  }
  else
  {
    Serial.println("Going to sleep.");
    //Go to sleep and wake up when you get a signal from the reader
    set_sleep_mode(SLEEP_MODE_STANDBY);//standby works - but no deeper - only takes 6 clock cycles to boot back up
    sleep_enable();
    sleep_mode();
    Serial.println("Waking up.");
  }
}

void read_in_queue_ISR(void)
{
  ready_to_read = true;
  detachInterrupt(1);
}

void read_incoming(void)
{
  if (RFID.available())
  {
    delay(100);
    
    for (int i = 0; i < 14; i++)
    {
      incoming_key[i] = RFID.read();
    }
    RFID.flush();//stops multiple reads
    
    bool access_granted = check_incoming();
    if (access_granted)
    {
      Serial.println("Accepted.");
      access_flag = true;
    }
    else
    {
      Serial.println("Rejected.");
      access_flag = false;
    }
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
