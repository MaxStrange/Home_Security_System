/**
This sketch is to test the relay to make sure that it is
switched on when supposed to and vice versa. It will
light an LED whenever the PIR sensor senses motion and
unlight it whenever not.
*/

const int PIR = 3;
const int SPEAKER = 8;

void setup()
{
  pinMode(PIR, INPUT);
  pinMode(SPEAKER, OUTPUT);
}

void loop()
{
  if (digitalRead(PIR))
  {//light the LED
    digitalWrite(SPEAKER, HIGH);
  }
  else
  {//dim the LED
    digitalWrite(SPEAKER, LOW);
  }
}
