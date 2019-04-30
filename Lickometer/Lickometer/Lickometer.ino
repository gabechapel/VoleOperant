int bottle1 = A0; //Left bottle lick input for Arduino
int bottle2 = A1; //Right bottle lick input for Arduino
int read1;
int read2;
int outLeft = 2; //Left bottle lick output for Bonsai
int outRight = 3;//Right bottle lick output for Bonsai
long startTime;
bool started = false; //Lick flag
void setup() {
  // put your setup code here, to run once:
  pinMode(bottle1, INPUT);
  pinMode(bottle2, INPUT);
  pinMode(outLeft, OUTPUT);
  pinMode(outRight, OUTPUT);
  
  Serial.begin(9600);
  startTime = millis();
  started = false;
}

void loop() {
  read1 = analogRead(bottle1); //read bottle inputs
  read2 = analogRead(bottle2);
  if ((read1 >150)&&(started == false)){ //if the vole starts licking the left bottle
    long time1 = millis() - startTime; //Get start time
    digitalWrite(outLeft,HIGH); //set pin high for Bonsai
    String left = "LEFT: ";
    String result1 = left + time1;
    Serial.println(result1);
    started = true; // flip flag
  }
    if ((read2 >150)&&(started == false)){ //if the vole starts licking the left bottle
    long time2 = millis() - startTime; //Get start time
    digitalWrite(outRight,HIGH); //set pin high for Bonsai
    String right = "RIGHT: ";
    String result2 = right + time2;
    Serial.println(result2);
    started = true; //flip flag
  }
  if((read1 == 0)&&(read2 == 0)&&(started == true)){// if the vole stops licking the bottles
    digitalWrite(outLeft,LOW); //set pins low for Bonsai
    digitalWrite(outRight,LOW);
    started = false; // flip flag
//    Serial.println("STOP");
  }
}
