const unsigned int GREEN = 8;
const unsigned int RED = 6;
const unsigned int PIR = 4;

int pirState = LOW;
int val = 0;

void setup(void)
{
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(PIR, INPUT);
}

void loop(void)
{
  val = digitalRead(PIR);
  if (val == HIGH)
  {
    //Motion has been detected! Intruder alert!
    
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, LOW);
    
    if (pirState == LOW)
    {
      //We have just now noticed the intruder
      pirState = HIGH;
    }
  }
  else
  {
    //Motion not detected. All clear.
    
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, HIGH);
    
    if (pirState == HIGH)
    {
      //We have just now stopped detected motion.
      pirState = LOW;
    }
  }
}
