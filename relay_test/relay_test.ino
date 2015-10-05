/**
This sketch is for testing the relay. It uses an NPN transistor to switch the relay. When the relay is
on the right line, it connects the speaker to a 5V supply line and thereby causes it to fire.
*/

const unsigned int RELAY = 8;

void setup(void)
{
  pinMode(RELAY, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop(void)
{
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  digitalWrite(RELAY, HIGH);
  delay(1000);
  digitalWrite(13, HIGH);
  digitalWrite(RELAY, LOW);
  delay(1000);
  digitalWrite(13, LOW);
}
