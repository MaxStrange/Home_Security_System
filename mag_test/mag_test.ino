/**
This sketch is to test the magnetic switches just to make sure that they work.
Very simple: just turns on the onboard LED when the switch is connected. Turns it
off when not. Circuit diagram is a pull-down resistor coupled with the switch.
So the signal is pulled down when the switch is not connected and it is HIGH
when it is connected.
*/

const unsigned int LED = 13;
const unsigned int SWITCH = 2;

void setup() 
{
  pinMode(LED, OUTPUT);
  pinMode(SWITCH, INPUT);  
}

void loop()
{
  if (digitalRead(SWITCH) == HIGH)
    digitalWrite(LED, HIGH);
  else
    digitalWrite(LED, LOW);
}
