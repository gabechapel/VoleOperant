
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

int doorOpen = 1;

void setup()  {
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
}

void loop(){
  int doorRead = digitalRead(doorLvr); //  right lever switch
  int foodRead = digitalRead(foodLvr); // left lever switch
  int openRead = digitalRead(openButton);
  int closeRead = digitalRead(closeButton);
  
// DOOR CODE
  if(doorRead==0){ //if door lever is pressed
    // OPEN DOOR
    digitalWrite(rightLED, HIGH);
    digitalWrite(doorLvrOut, HIGH);
    delay(30);
    digitalWrite(doorLvrOut, LOW);
    delay(2000);
    openDoor();
    // RETRACT LEVERS
//    lvrRetract(slpDoorLvr, stpDoorLvr, dirDoorLvr);
//    lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
  }
  
  if(foodRead == 0){ //if food lever is pressed
    digitalWrite(foodLvrOut, HIGH);
    delay(30);
    digitalWrite(foodLvrOut, LOW);
    digitalWrite(leftLED, HIGH);
    // RETRACT LEVERS
//    lvrRetract(slpDoorLvr, stpDoorLvr, dirDoorLvr);
//    lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
//    tone(buzz,880,2000); // tone plays when door opens
    digitalWrite(leftLED,LOW);
  }
  
  if(openRead==0){ //if open button is pressed
    // OPEN DOOR
    digitalWrite(rightLED, HIGH);
    openDoor();
    // RETRACT LEVERS
//    lvrRetract(slpDoorLvr, stpDoorLvr, dirRightLvr);
//    lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
  }
  
  if(closeRead == 0){ //if close button is pressed
    if(doorOpen == 1){ // Limit the output pulse to when the button is first pressed
        // Set close output to high
        digitalWrite(closeOut, HIGH);
        delay(30);
        digitalWrite(closeOut, LOW);
    }
     // CLOSE THE DOOR
     closeDoor();
//    // EXTRUDE THE LEVERS
//       lvrExtrude(slpRightDoor, stpRightDoor, dirRightDoor);
//       lvrExtrude(slpLeftDoor, stpLeftDoor, dirLeftDoor);
    }else if(closeRead == 1){ //if close button is released
      digitalWrite(dirDoor, LOW);  // turns drivers off
      digitalWrite(slpDoor, LOW);
    }

    
} // void loop end

void openDoor(){
  //set open output to high
  digitalWrite(openOut, HIGH);
  delay(30);
  digitalWrite(openOut, LOW);
  
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
  digitalWrite(dirDoor, LOW);
  digitalWrite(slpDoor, HIGH);
  digitalWrite(stpDoor,HIGH);
  delay(2);
  digitalWrite(stpDoor,LOW);
  delay(2);
  doorOpen = 0; // sets door status to closed
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

