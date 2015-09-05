/**
This sketch is to test using a PIR sensor with an interrupt rather than through polling.
Since it uses the Arduino's external interrupts, it requires that the pin used be the
one specified (3).
*/

const unsigned int LED = 13;                // choose the pin for the LED
const unsigned int PIR = 3;               // choose the input pin (for PIR sensor)

int pir_state = LOW;             // we start, assuming no motion detected

bool motion_flag = false; //Flag to let the Arduino know that motion has been detected

void setup()
{
  pinMode(LED, OUTPUT);      // declare LED as output
  pinMode(PIR, INPUT);     // declare sensor as input
  
  attachInterrupt(1, pir_interrupt, RISING);//attach interrupt 1 (pin 3)
}
 
void loop()
{
  if (motion_flag)
  {
    delay(1000);
    digitalWrite(LED, LOW);
    motion_flag = false;
  }
}

void pir_interrupt(void)
{
  if (motion_flag == false)
  {
    digitalWrite(LED, HIGH);
    motion_flag = true;
  }
  //ignore motion unless it is new to us
}
