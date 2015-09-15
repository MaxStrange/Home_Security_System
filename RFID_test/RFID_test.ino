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
