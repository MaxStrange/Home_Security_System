String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

const int KEY_LENGTH = 16;
char key_a[KEY_LENGTH] = { 63, 100, 94, 230, 166, 102, 230, 76, 102, 166, 201, 201, 202, 214, 242, 0 };
char key_b[KEY_LENGTH] = { 63, 100, 94, 230, 166, 102, 166, 76, 153, 166, 76, 153, 202, 214, 242, 0 };
char key_c[KEY_LENGTH] = { 63, 100, 94, 230, 166, 166, 204, 151, 204, 201, 62, 234, 202, 214, 242, 0 };
char incoming_key[KEY_LENGTH] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

void setup()
{
   Serial.begin(9600);
   inputString.reserve(20);
}

void loop()
{
   serialEvent();
   
   if (stringComplete)
   {
     Serial.println(inputString);
     
     inputString.toCharArray(incoming_key, KEY_LENGTH);
     
     boolean known = true;
     for (int i = 0; i < KEY_LENGTH; i++)
     {
       if ((incoming_key[i] != key_a[i]) && (incoming_key[i] != key_b[i]) && (incoming_key[i] != key_c[i]))
       {
         known = false;
         break;
       }
     }
     
     if (known)
       Serial.println("KNOWN");
     else
       Serial.println("UNKOWN");
     
     inputString = "";
     stringComplete = false;
   }
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
  */
void serialEvent()
{
   while (Serial.available())
   {
     char inChar = (char)Serial.read();  
     inputString += inChar;
     
     if (inChar == 0)
     {
       stringComplete = true;
     }
   }
}
