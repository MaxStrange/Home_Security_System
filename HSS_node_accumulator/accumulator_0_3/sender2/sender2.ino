#include <SPI.h>
#include "RF24.h"

/**Pin declarations*/
const unsigned int SENSORS = 3;
const int RADIO_PIN_1 = 9;
const int RADIO_PIN_2 = 10;

/**Constants**/
const uint16_t THREAT_SIGNAL = 0x1BA0;
const uint16_t DISARM_SIGNAL = 0x1151;

/**Radio code*/
RF24 radio(RADIO_PIN_1, RADIO_PIN_2);
byte node_ids[][6] = { "cmltr", "1send", "2send" };

/**Sensor code*/
int sensors_state = HIGH;
int val = HIGH;

void setup(void)
{
  Serial.begin(115200);
  
  /**Pin sets*/
  pinMode(SENSORS, INPUT);
  
  /**Radio*/
  radio.begin();
  radio.setRetries(15, 15);
  radio.openWritingPipe(node_ids[2]);
  radio.startListening();
  
  Serial.println("Starting. Press d at any time to disarm system.");
}

void loop(void)
{
  /**Poll the sensor line to check for intruder*/
  check_for_entry();
  
  send_state();
  
  if (Serial.available())
  {
    char c = toupper(Serial.read());
    if (c == 'D')
    {
      Serial.println("Sending disarm!");
      radio.stopListening();  //Stop listening so we can write
      radio.write( &DISARM_SIGNAL, sizeof(uint16_t) );
      radio.startListening();
    }
  }
  
  delay(250);
}

void check_for_entry(void)
{
  val = digitalRead(SENSORS);
  if (val == LOW)
  {
    //bad guys detected
        
    if (sensors_state == HIGH)
    {
      //We just now detected the intruder
      sensors_state = LOW;
    }
  }
  else
  {
    //all clear
    
    if (sensors_state == LOW)
    {
      //We just now stopped detecting alert
      sensors_state = HIGH;
    }
  }
}

void send_state(void)
{
  if (!sensors_state)
  {
    Serial.println("Sending threat!");
    radio.stopListening();  //Stop listening so we can write
    radio.write( &THREAT_SIGNAL, sizeof(uint16_t) );
    radio.startListening();
  }
}
