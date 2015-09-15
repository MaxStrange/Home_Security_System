/**
This sketch reads a single card key in to its buffer and
then checks it against its known tags.
*/
#include <SoftwareSerial.h>
SoftwareSerial RFID(6, 7); // RX and TX
 
int data1 = 0;
int ok = -1;
 
int tag1[14] = { 2, 55, 67, 48, 48, 53, 54, 56, 51, 54, 65, 67, 51, 3 };
int tag2[14] = { 2, 55, 67, 48, 48, 49, 57, 68, 56, 66, 51, 48, 69, 3 };
int tag3[14] = { 2, 55, 67, 48, 48, 53, 54, 57, 53, 55, 57, 67, 54,3 };
int newtag[14] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // used for read comparisons
 
void setup()
{
  RFID.begin(9600);    // start serial to RFID reader
  Serial.begin(9600);  // start serial to PC 
}
 
boolean comparetag(int aa[14], int bb[14])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < 14 ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == 14)
  {
    ff = true;
  }
  return ff;
}
 
void checkmytags() // compares each tag against the tag just read
{
  ok = 0; // this variable helps decision-making,
  // if it is 1 we have a match, zero is a read but no match,
  // -1 is no read attempt made
  if (comparetag(newtag, tag1) == true)
  {
    ok++;
  }
  if (comparetag(newtag, tag2) == true)
  {
    ok++;
  }
  if (comparetag(newtag, tag3) == true)
  {
    ok++;
  }
}
 
void readTags()
{
  ok = -1;
 
  if (RFID.available() > 0) 
  {
    // read tag numbers
    delay(100); // needed to allow time for the data to come in from the serial buffer.
 
    for (int z = 0 ; z < 14 ; z++) // read the rest of the tag
    {
      data1 = RFID.read();
      newtag[z] = data1;
    }
    RFID.flush(); // stops multiple reads
 
    // do the tags match up?
    checkmytags();
  }
 
  // now do something based on tag type
  if (ok > 0) // if we had a match
  {
    Serial.println("Accepted"); 
    ok = -1;
  }
  else if (ok == 0) // if we didn't have a match
  {
    Serial.println("Rejected"); 
    ok = -1;
  }
}
 
void loop()
{
  readTags();
}

