
// This version of readRX is geared towards training the voles in two chambers, removing RF components from previous versions.
// It is designed to drive the door motor when a lever or button is pressed, turn on a light and buzzer, and retract levers when not in use.


// Pin Names
const int buzz = 2; //buzzer
const int doorLvr = A0; // door lever
const int foodLvr = A1;
const int closeButton = A2;
const int openButton = A3;
const int buttonLED = 30; //red
const int rightLED = 31; //blue
const int leftLED = 32; //green

//LABJACK PINS
const int openOut = 22;
const int closeOut = 23;
const int doorLvrOut = 24;
const int foodLvrOut = 25;

//Door driver pins
const int slpDoor = 5; // reset and sleep driver pin
const int stpDoor = 6; // stepper motor pwm
const int dirDoor = 7; // direction driver pin
//Door lever driver pins
const int slpDoorLvr = 8; // reset and sleep driver pin
const int stpDoorLvr = 9; // stepper motor pwm
const int dirDoorLvr = 10; // direction driver pin
//Door lever driver pins
const int slpFoodLvr = 11; // reset and sleep driver pin
const int stpFoodLvr = 12; // stepper motor pwm
const int dirFoodLvr = 13; // direction driver pin

//const int led = 9; // LED indicating IR obstruction
//const int rec1 = 52; // IR receiver 1
//const int rec2 = 53; // IR receiver 2
//const int ir1 = 8; // IR blaster 1
//const int ir2 = 7; // IR blaster 2

// Important Global Variables
long t1 = 0; // milli counter 1
long t2 = 0; // milli counter 2
long rt = 0; // seconds elapsed

int adoor = 0; // 1 is open, 0 is closed
long taop = 0; // time that the door opens, for closing later with the antennas
int mtrpos =0;
long irtrig = 0;

void setup()  
{
  Serial.begin(9600);

pinMode(buzz, OUTPUT);

//Lever Setup
pinMode(doorLvr,INPUT_PULLUP);
pinMode(foodLvr, INPUT_PULLUP);
pinMode(closeButton, INPUT_PULLUP);
pinMode(openButton, INPUT_PULLUP);
pinMode(rightLED,OUTPUT);
pinMode(leftLED,OUTPUT);
pinMode(buttonLED,OUTPUT);

//Labjack setup
pinMode(openOut, OUTPUT);
pinMode(closeOut, OUTPUT);
pinMode(doorLvrOut, OUTPUT);
pinMode(foodLvrOut, OUTPUT);
digitalWrite(openOut, LOW);
digitalWrite(closeOut, LOW);
digitalWrite(doorLvrOut, LOW);
digitalWrite(foodLvrOut, LOW);

//Door Driver Setup
pinMode(slpDoor, OUTPUT);
digitalWrite(slpDoor,LOW);
pinMode(stpDoor, OUTPUT);
digitalWrite(stpDoor,LOW);
pinMode(dirDoor, OUTPUT);

//Door Lever Driver Setup
pinMode(slpDoorLvr, OUTPUT);
digitalWrite(slpDoorLvr,LOW);
pinMode(stpDoorLvr, OUTPUT);
digitalWrite(stpDoorLvr,LOW);
pinMode(dirDoorLvr, OUTPUT);

////IR Setup
//pinMode(rec1, INPUT); 
//pinMode(rec2, INPUT);
//pinMode(ir1, OUTPUT);
//digitalWrite(ir1,LOW);
//pinMode(ir2, OUTPUT);
}

void loop(){
  t2 = millis(); // gets current milli output
//  Serial.print ("T1: ");
//  Serial.println(t1);
//  Serial.print ("T2: ");
//  Serial.println(millis());
  rt = abs(abs(t2)-abs(t1))+rt; // total milli counter
//  Serial.println(rt);
  t1 = t2; // resets t1 for new calculations
  int doorRead = digitalRead(doorLvr); //  right lever switch
  int foodRead = digitalRead(foodLvr); // left lever switch
  int openRead = digitalRead(openButton);
  int closeRead = digitalRead(closeButton);
  
// DOOR and IR code
if(adoor==0){ // Open the door if closed and retract the lever
//  Serial.println(doorRead);
  long acloset = rt - taop;
//  Serial.println(acloset);
  if(doorRead==0){ //if switch is pressed
    // OPEN DOOR
    digitalWrite(rightLED, HIGH);
    digitalWrite(doorLvrOut, HIGH);
    delay(50);
    digitalWrite(doorLvrOut, LOW);
    delay(2000);
    openDoor();
    // RETRACT LEVERS
//    lvrRetract(slpDoorLvr, stpDoorLvr, dirDoorLvr);
//    lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
    taop = rt;
  }
  if(foodRead == 0){
    digitalWrite(foodLvrOut, HIGH);
    delay(30);
    digitalWrite(foodLvrOut, LOW);
    digitalWrite(leftLED, HIGH);
    // RETRACT LEVERS
//    lvrRetract(slpDoorLvr, stpDoorLvr, dirDoorLvr);
//    lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
//    tone(buzz,880,2000); // tone plays when door opens
    digitalWrite(leftLED,LOW);
    taop = rt;
  }
  if(openRead==0){ //if open button is pressed
    adoor = 1; 
    // OPEN DOOR
    digitalWrite(rightLED, HIGH);
    openDoor();
    // RETRACT LEVERS
//    lvrRetract(slpDoorLvr, stpDoorLvr, dirRightLvr);
//    lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
    taop = rt;
  }
//  if (acloset >30000){ // if 2min has passed, door opens
////    tone(buzz,880,1000); // tone plays when door opens
//    adoor = 1;
//
//    digitalWrite(led,HIGH); //Turn on led
//    delay(10000); //keep led on for 10 seconds
//    
//    //Open Door
//    openDoor();
//
//    //Retract the lever
//    lvrRetract(slpLvrDoor, stpLvrDoor, dirLvrDoor);
//    Serial.print("OPEN XX ");
//    Serial.println(taop);
//    digitalWrite(led,LOW); //Turn off led
//    taop = rt;
//  }
}

  if(adoor == 1){ // close the door if open and extrude the lever
//    if (doorRead==0){ // if switch is pressed, door opens
//        tone(buzz,680,1000); // tone plays when door opens
//        taop = rt;
//      }
    long aopent = rt-taop; // intitial timer for how long the door is open
//    Serial.println(aopent);
    if(closeRead == 0){
//     if(aopent>30000){ // adjust timer values here, aopent is for how long the door stays open after pressing lever. 
       // CLOSE THE DOOR
       closeDoor();
  //    // EXTRUDE THE LEVERS
//       lvrExtrude(slpRightDoor, stpRightDoor, dirRightDoor);
//       lvrExtrude(slpLeftDoor, stpLeftDoor, dirLeftDoor);
      Serial.print("CLOSED XX ");  // serial feedback
  //    Serial.println(rt);
      taop = rt;
    } // end of door timing loop
  } // end of open door loop
} // void loop end

void openDoor(){
  //set open output to high
  digitalWrite(openOut, HIGH);
  delay(50);
  digitalWrite(openOut, LOW);
  
  adoor = 1; // sets door status to open
  //Open Door
  digitalWrite(dirDoor, HIGH);
 
  digitalWrite(slpDoor, HIGH);
  for(int k=1; k<=350; k++){
      digitalWrite(stpDoor,HIGH);
      delay(2);
      digitalWrite(stpDoor,LOW);
      delay(2);
  }
  
  digitalWrite(dirDoor, LOW);  // turns drivers off
  digitalWrite(slpDoor, LOW);
  
  tone(buzz,880,2000); // tone plays when door opens
  digitalWrite(rightLED,LOW);
}

void closeDoor(){
  // Set close output to high
  digitalWrite(closeOut, HIGH);
  delay(50);
  digitalWrite(closeOut, LOW);
  
  digitalWrite(dirDoor, LOW);
  digitalWrite(slpDoor, HIGH);
  mtrpos = 0;

  //Close the door
  for(int k=1; k<=350; k++){
      digitalWrite(stpDoor,HIGH);
      delay(2);
      digitalWrite(stpDoor,LOW);
      delay(2);
  }
  adoor = 0; // sets door status to closed
  digitalWrite(dirDoor, LOW);  // turns drivers off
  digitalWrite(slpDoor, LOW);
}

void lvrExtrude(int slp, int stp, int dir){
  //Extrude the lever
  digitalWrite(dir,LOW);
  digitalWrite(slp,HIGH);
  for(int k=1; k<=60; k++){ // part where motor actually moves, 450 steps at a time
        digitalWrite(stp,HIGH);
        delay(1);
        digitalWrite(stp,LOW);
        delay(1);
        }
  digitalWrite(slp, LOW);
}

void lvrRetract(int slp, int stp, int dir){
  //Retract the lever
  digitalWrite(dir,HIGH);
  digitalWrite(slp,HIGH);
  for(int k=1; k<=60; k++){ // part where motor actually moves, 450 steps at a time
        digitalWrite(stp,HIGH);
        delay(1);
        digitalWrite(stp,LOW);
        delay(1);
        }
  digitalWrite(slp, LOW);
}
