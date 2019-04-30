#include <Servo.h>

// Pin Names
const int buzz = 2; // buzzer
const int doorLvr = A0; // door lever
const int foodLvr = A1;
const int foodLED = 31; //blue
const int doorLED = 32; //green
const int dispLED = 33;

//Transmit Pins
const int dispenseOut = 23; //high when pellet should be dispensed, connects to 3 on other Arduino

//Receive Pins
const int dispenseIn = 24; //high when pellet is dispensed, connects to 7 on other Arduino
const int retrieveIn = 25; //high when pellet is retrieved, connects to 8 on other Arduino

////Stepper Motor/////
//Door lever driver pins
const int slpDoorLvr = 8; // reset and sleep driver pin
const int stpDoorLvr = 9; // stepper motor pwm
const int dirDoorLvr = 10; // direction driver pin
//Door lever driver pins
const int slpFoodLvr = 11; // reset and sleep driver pin
const int stpFoodLvr = 12; // stepper motor pwm
const int dirFoodLvr = 13; // direction driver pin

/////Servo Motor/////
const int doorServoPin = 3;
const int foodServoPin = 4;

// USER DEFINED VARIABLES
// Section I
const int startTone = 880;  // tone frequency
const int time1 = 2000; // 2 seconds
//Section II
const int time2 = 10000;
//Section III
const int pelletTone = 500;
const int time3 = 1000;
//Section IV
const int time4 = 2000;
//Section V
const long time5 = 60000; // ints are 16-bit (--32,768 to 32,767)


// Important Global Variables
Servo doorServo; // servo objects
Servo foodServo;
long t1 = 0; // milli counter 1
long t2 = 0; // milli counter 2
long dt = 0; // elapsed time
int lvrStatus = 0;

void setup(){
  Serial.begin(9600);
  
  pinMode(buzz, OUTPUT);

  //Lever Setup
  pinMode(doorLvr,INPUT_PULLUP);
  pinMode(foodLvr, INPUT_PULLUP);
  pinMode(foodLED,OUTPUT);
  pinMode(doorLED,OUTPUT);
  pinMode(dispLED, OUTPUT);
  
  //Labjack setup
  pinMode(dispenseOut, OUTPUT);
  pinMode(dispenseIn, INPUT);
  pinMode(retrieveIn, INPUT);
  digitalWrite(dispenseIn, LOW);
  digitalWrite(retrieveIn, LOW);
  digitalWrite(dispenseOut, LOW);
  
  //Door Lever Driver Setup
  pinMode(slpDoorLvr, OUTPUT);
  digitalWrite(slpDoorLvr,LOW);
  pinMode(stpDoorLvr, OUTPUT);
  digitalWrite(stpDoorLvr,LOW);
  pinMode(dirDoorLvr, OUTPUT);
  
  //Food Lever Driver Setup
  pinMode(slpFoodLvr, OUTPUT);
  digitalWrite(slpFoodLvr,LOW);
  pinMode(stpFoodLvr, OUTPUT);
  digitalWrite(stpFoodLvr,LOW);
  pinMode(dirFoodLvr, OUTPUT);

  //Servo Setup
  doorServo.attach(doorServoPin);
  foodServo.attach(foodServoPin);
  doorServo.write(0); // Start servos retracted
  foodServo.write(0);
  
  delay(10000);
}

void loop(){
  int doorRead = digitalRead(doorLvr); //  right lever switch
  int foodRead = digitalRead(foodLvr); // left lever switch
  int dispenseRead = digitalRead(dispenseIn); //input from other Arduino, reads high when pellet is dispensed
  int retrieveRead = digitalRead(retrieveIn); //input from other Arduino, reads high when pellet is picked up
//  Serial.println(doorRead);
//  Serial.println(foodRead);

  ///// STAGE I /////
  // play startTone for a duration of time1
  tone(buzz, startTone);
//  Serial.println("StartTone");
  delay(time1);
  noTone(buzz);

  ///// STAGE II /////
  // Turn on lights
  digitalWrite(foodLED,HIGH);
  digitalWrite(doorLED,HIGH);
  // Extrude levers
  doorServo.write(150);
  foodServo.write(150);
  Serial.println("LvrIn" + String(millis()));
//  lvrExtrude(slpFoodLvr, stpFoodLvr, dirFoodLvr);
//  lvrExtrude(slpDoorLvr, stpDoorLvr, dirDoorLvr);
  // Start timer
  t1 = millis();
  dt = 0;
  while((dt < time2)){ // while less than time2 has passed
    t2 = millis();
    dt = abs(t2 - t1);
//    Serial.println(dt);
    doorRead = digitalRead(doorLvr);
    foodRead = digitalRead(foodLvr);
//    Serial.println("DOOR: " + String(doorRead));
//    Serial.println("FOOD: " + String(foodRead));
    if (doorRead == 0){// if door lever is pressed
      Serial.println("Dpress" + String(millis()));
      break;
    }
    if (foodRead == 0){ // if food lever is pressed
      Serial.println("Fpress" + String(millis()));
      break;
    }
  }

  ///// STAGE III /////
  digitalWrite(dispenseOut,HIGH); //inform other Arduino to dispense pellet
  t1 = millis();
  dt = 0;
  tone(buzz, pelletTone);
//  Serial.println("PelletTone");
  while(dt < time3){
    t2 = millis();
    dt = abs(t2 - t1);
//    Serial.println(dt);
    dispenseRead = digitalRead(dispenseIn);
    retrieveRead = digitalRead(retrieveIn);
    if (dispenseRead == 1){ //if pellet is dispensed
      Serial.println("Disp" + String(millis()));
      digitalWrite(dispenseOut, LOW);
      digitalWrite(dispLED, HIGH);
    }
    if (retrieveRead == 1){ //if pellet is retrieved
      Serial.println("Retr" + String(millis()));
      digitalWrite(dispLED, LOW);
    }
  }
  noTone(buzz);
  digitalWrite(dispenseOut,LOW);

  ///// STAGE IV /////
  t1 = millis();
  dt = 0;
  while(dt < time4){
    t2 = millis();
    dt = abs(t2 - t1);
//    Serial.println(dt);
    dispenseRead = digitalRead(dispenseIn);
    retrieveRead = digitalRead(retrieveIn);
    doorRead = digitalRead(doorLvr);
    foodRead = digitalRead(foodLvr);
    if (dispenseRead == 1){ //if pellet is dispensed
      Serial.println("Disp" + String(millis()));
      digitalWrite(dispenseOut,LOW);
      digitalWrite(dispLED,HIGH);
    }
    if (retrieveRead == 1){ //if pellet is retrieved
      Serial.println("Retr" + String(millis()));
      digitalWrite(dispLED,LOW);
    }
    if (doorRead == 0){// if door lever is pressed
      Serial.println("Dpress" + String(millis()));
    }
    if (foodRead == 0){ // if food lever is pressed
      Serial.println("Fpress" + String(millis()));
    }
  }
  digitalWrite(foodLED,LOW);
  digitalWrite(doorLED,LOW);
  //Retract levers
  doorServo.write(0);
  foodServo.write(0);
  Serial.println("LvrOut" + String(millis()));
//  lvrRetract(slpFoodLvr, stpFoodLvr, dirFoodLvr);
//  lvrRetract(slpDoorLvr, stpDoorLvr, dirDoorLvr);
  

  ///// STAGE V /////
  t1 = millis();
  dt = 0;
  while(dt < time5){
    t2 = millis();
    dt = abs(t2 - t1);
//    Serial.println(dt);
    dispenseRead = digitalRead(dispenseIn);
    retrieveRead = digitalRead(retrieveIn);
    if (dispenseRead == 1){ //if pellet is dispensed
      Serial.println("Disp" + String(millis()));
      digitalWrite(dispenseOut,LOW);
      digitalWrite(dispLED,HIGH);
    }
    if (retrieveRead == 1){ //if pellet is retrieved
      Serial.println("Retr" + String(millis()));
      digitalWrite(dispLED,LOW);
    }
  }
//  
} // void loop end

void lvrExtrude(int slp, int stp, int dir){
  if (lvrStatus == 0){ // Only execute if levers are retracted
    //Extrude the lever
    digitalWrite(dir,HIGH);
    digitalWrite(slp,HIGH);
    for(int k=1; k<=20; k++){ // part where motor actually moves, 450 steps at a time
          digitalWrite(stp,HIGH);
          delay(10);
          digitalWrite(stp,LOW);
          delay(10);
          }
    digitalWrite(slp, LOW);
    lvrStatus = 1;
  }
}

void lvrRetract(int slp, int stp, int dir){
  if (lvrStatus == 1){ // Only execute if levers are extruded
    //Retract the lever
    digitalWrite(dir,LOW);
    digitalWrite(slp,HIGH);
    for(int k=1; k<=20; k++){ // part where motor actually moves, 450 steps at a time
          digitalWrite(stp,HIGH);
          delay(10);
          digitalWrite(stp,LOW);
          delay(10);
          }
    digitalWrite(slp, LOW);
    lvrStatus = 0;
  }
}
