////////////////////////////////////////////
////// MOST CURRENT VERSION 1/13/20 ///////
//////////////////////////////////////////
#include <Firmata.h>

int bottle1 = A0; //Left bottle lick input for Arduino
int bottle2 = A1; //Right bottle lick input for Arduino
int read1;
int read2;
byte outLeft = 2; //Left bottle lick output for Bonsai
byte outRight = 3;//Right bottle lick output for Bonsai
long startTime;
bool started = false; //Lick flag
int leftValue;
int rightValue;
int count = 0;

void setup() {
  Serial.begin(57600);
  startTime = millis();
  started = false;
  
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
  Firmata.begin(57600);
  
  pinMode(bottle1, INPUT);
  pinMode(bottle2, INPUT);
  pinMode(outLeft, OUTPUT);
  pinMode(outRight, OUTPUT);
}

void loop() {
  while(count <75){
    read1 = analogRead(bottle1); //read bottle inputs
    read2 = analogRead(bottle2);
    if ((read1 >120)){ //if the vole starts licking the left bottle
//      long time1 = millis() - startTime; //Get start time
      leftValue = 500;
//      String left = "LEFT: ";
//      String result1 = left + time1;
      Serial.println("HERE 1");
    }
      if ((read2 >120)){ //if the vole starts licking the left bottle
//      long time2 = millis() - startTime; //Get start time
      rightValue = 500;
//      String right = "RIGHT: ";
//      String result2 = right + time2;
      Serial.println("HERE 2");
    }
  count += 1;
  }
  Firmata.sendAnalog(outLeft, leftValue);
  Firmata.sendAnalog(outRight, rightValue);
  leftValue = 0;
  rightValue = 0;
  count = 0;
  Serial.println(read2);
}

void analogWriteCallback(byte pin, int value)
{
  pinMode(pin, OUTPUT);
  analogWrite(pin, value);
}
