const unsigned int ledPin = 13;                // choose the pin for the LED
const unsigned int inputPin = 4;               // choose the input pin (for PIR sensor)

int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
 
void setup()
{
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
 
  Serial.begin(9600);
}
 
void loop()
{
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH)
  {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    
    if (pirState == LOW)
    {
      // we have just turned on
      Serial.println("Motion detected!");

      pirState = HIGH;
    }
    
  } 
  else
  {
    digitalWrite(ledPin, LOW); // turn LED OFF
 
    if (pirState == HIGH)
    {
      // we have just turned off

      Serial.println("Motion ended!");

      pirState = LOW;
    }
  }
}

