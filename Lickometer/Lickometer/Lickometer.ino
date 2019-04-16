int bottle1 = A0;
int bottle2 = A1;
int read1;
int read2;
long startTime;
bool started = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(bottle1, INPUT);
  pinMode(bottle2, INPUT);
  
  Serial.begin(9600);
  startTime = millis();
  started = false;
}

void loop() {
  // put your main code here, to run repeatedly:
  read1 = analogRead(bottle1);
  read2 = analogRead(bottle2);
  if ((read1 >50)&&(started == false)){ //if the vole starts licking the left bottle
    long time1 = millis() - startTime; //Get start time
    String left = "LEFT: ";
    String result1 = left + time1;
    Serial.println(result1);
    started = true;
  }
    if ((read2 >50)&&(started == false)){ //if the vole starts licking the left bottle
    long time2 = millis() - startTime; //Get start time
    String right = "RIGHT: ";
    String result2 = right + time2;
    Serial.println(result2);
    started = true;
  }
  if((read1 == 0)&&(read2 == 0)&&(started == true)){// if the vole stops licking the bottles
    started = false;
//    Serial.println("STOP");
  }
}
