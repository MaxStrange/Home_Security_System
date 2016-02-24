/**
This sketch reads a single card key in to its buffer and
then checks it against its known tags.
*/
/**
This sketch reads a single card key in to its buffer and
then checks it against its known tags.
*/
//#include <SoftwareSerial.h>

//SoftwareSerial RFID(6, 7); // RX and TX

int key_a[16] = { 63, 100, 94, 230, 166, 102, 230, 76, 102, 166, 201, 201, 202, 214, 242, 0 };
int key_b[16] = { 63, 100, 94, 230, 166, 102, 166, 76, 153, 166, 76, 153, 202, 214, 242, 0 };
int key_c[16] = { 63, 100, 94, 230, 166, 166, 204, 151, 204, 201, 62, 234, 202, 214, 242, 0 };
int incoming_key[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }; // used for read comparisons

volatile boolean ready_to_read = false;

void setup(void)
{
  //RFID.begin(9600);
  Serial.begin(9600);
  
  attachInterrupt(0, read_in_queue_ISR, RISING);
}

void loop(void)
{
  if (ready_to_read)
  {
    read_incoming();
    ready_to_read = false;
    attachInterrupt(0, read_in_queue_ISR, RISING);
  }
}

void read_in_queue_ISR(void)
{
  ready_to_read = true;
  detachInterrupt(0);
}

void read_incoming(void)
{
  delay(200);//necessary - tested!
  if (Serial.available())///(RFID.available())
  {    
    for (int i = 0; i < 16; i++)
    {
      incoming_key[i] = Serial.read();//RFID.read();
    }
    Serial.flush();
    //RFID.flush();//stops multiple reads
    
    bool access_granted = check_incoming();
    if (access_granted)
    {
      Serial.println("Accepted.");
    }
    else
    {
      Serial.println("Rejected.");
    }
  }
}

boolean check_incoming(void)
{
  if (compare_keys(key_a, incoming_key))
    return true;
  else if (compare_keys(key_b, incoming_key))
    return true;
  else if (compare_keys(key_c, incoming_key))
    return true;
  else
    return false;
}

boolean compare_keys(int aa[16], int bb[16])
{
  boolean ff = false;
  int fg = 0;
  for (int cc = 0 ; cc < 16 ; cc++)
  {
    if (aa[cc] == bb[cc])
    {
      fg++;
    }
  }
  if (fg == 16)
  {
    ff = true;
  }
  return ff;
}
