/*
Green/Red LED example.
*/

const unsigned int GREEN = 8;
const unsigned int RED = 6;

void setup()
{
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
}

void loop()
{
  digitalWrite(GREEN, HIGH);
  digitalWrite(RED, HIGH);
  delay(1000);
  digitalWrite(GREEN, LOW);
  digitalWrite(RED, LOW);
  delay(1000);
}
