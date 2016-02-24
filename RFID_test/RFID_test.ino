#include <SoftwareSerial.h>
SoftwareSerial RFID(6, 7);
int i;
void setup(void)
{
  RFID.begin(9600);
  Serial.begin(9600);
}

void loop(void)
{
  if (RFID.available())
  {
    i = RFID.read();
    Serial.print(i, DEC);
    Serial.print(" ");
  }
}

/*
63 100 94 230 166 102 230 76 102 166 201 201 202 214 242 0
63 100 94 230 166 102 166 76 153 166 76  153 202 214 242 0 
63 100 94 230 166 166 204 151 204 201 62 234 202 214 242 0 
*/
