// Gabe there's comments on stuff that needs to be changed, mostly pin names and extra time trackers
// This version of readRX is geared towards adding directionality to previous versions. 
// It is designed to use 4 antennas and rfid readers, and can read 2 chips moving independently between 3 cages.
// To set it up, use a previous version of readRX to get the RFID tags for the chips being used.
// This only works when the tags are already known and input in the right spot

#include <SoftwareSerial.h>
SoftwareSerial rfid2 = SoftwareSerial(50,51); // Rx, Tx
//Mega can use pins 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, or 69 for Rx

// position/direcionality key
// 0 large cage
// 1 moving from large cage to small cage on a side
// 2 small cage on a side
// 3 moving from small cage to large cage on a side
// -1 moving from large cage to small cage on b side
// -2 small cage on b side
// -3 moving from small cage to large cage on b side

// Change these values for setup with RFID tags. (case sensitive)
int posim = 0; // initial male position
int posif = 2; // initial female position
String female = "002FBE716383"; //female tag
String male = "002FBE719373"; // male tag

// Pin Names
const int sw1 = 13; // lever switch add more lever
const int sw2 = xx;
const int sw3 = xx;
const int sw4 = xx;
const int stp1 = 12; // stepper motor pwm
const int dir1 = 11; // direction driver pin
const int slp1 = 10; // reset and sleep driver pin
const int stp2 = xx; // stepper motor pwm
const int dir2 = xx; // direction driver pin
const int slp2 = xx; // sleep and reset driver pin
const int led = 9; // LED indicating IR obstruction
const int rec1 = 52; // IR receiver 1
const int rec2 = 53; // IR receiver 2
const int rec3 = xx; // IR receiver 3
const int rec4 = xx; // IR receiver 4
const int ir1 = xx; // IR blaster 1
const int ir2 = xx; // IR blaster 2
const int ir3 = xx; // IR blaster 3
const int ir4 = xx; // IR blaster 4
// add pins for the lever motors too

// Important Global Variables
String msg1; // Output from antenna 1
String msg2; // Output from antenna 2
String msg3; // Output from antenna 3
String msg4; // output from antenna 4
int t1 = 0; // milli counter 1
int t2 = 0; // milli counter 2
long rt = 0; // seconds elapsed

int adoor = 0; // 1 is open, 0 is closed
int bdoor = 0; // 1 is open, 0 is closed
long taop = 0; // time that the door opens, for closing later with the antennas
long tbop = 0; // time that the door opens, for closing later with the antennas
int mtrposa = 0; // door a position tracker
int mtrposb = 0; // door b position tracker
long irtriga = 0; // a side ir tripped time
long irtrigb = 0; // b side ir tripped time


void setup()  
{
Serial.begin(9600);
rfid2.begin(9600); // Begin software serial port // msg2
Serial1.begin(9600); // Begin hardware serial port 1 // msg1
Serial2.begin(9600); // Begin hardware serial port 2 // msg3
Serial3.begin(9600); // begin hardware serial port 3  // msg4

pinMode(slp1, OUTPUT);
digitalWrite(slp1,LOW);
pinMode(slp2, OUTPUT);
digitalWrite(slp2,LOW);
pinMode(rec1, INPUT); 
pinMode(rec2, INPUT);
pinMode(rec3, INPUT);
pinMode(rec4, INPUT);
pinMode(ir1, OUTPUT);
digitalWrite(ir1,LOW);
pinMode(ir2, OUTPUT);
digitalWrite(ir2,LOW);
pinMode(ir3, OUTPUT);
digitalWrite(ir3,LOW);
pinMode(ir4, OUTPUT);
digitalWrite(ir4,LOW);
pinMode(led, OUTPUT);
pinMode(sw1, INPUT);
pinMode(sw2, INPUT);
pinMode(sw3, INPUT);
pinMode(sw4, INPUT);
pinMode(dir1, OUTPUT);
pinMode(dir2, OUTPUT);

pinMode(stp1, OUTPUT);
digitalWrite(stp1,LOW);
pinMode(stp2, OUTPUT);
digitalWrite(stp2,LOW);
  
  // initial positions of voles
  Serial.print(male); // male
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);

  Serial.print(female); // female
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  
}

void loop(){
  t2 = millis(); // gets current milli output
  rt = abs(abs(t2)-abs(t1))+rt; // total milli counter
  t1 = t2; // resets t1 for new calculations
  msg1 = ""; // Initialize ID message
  msg2 = "";
  msg3 = "";
  msg4 = "";
  while(Serial1.available()>0) {
    msg1 += char(Serial1.read());
    delay(1);
  }
  // parses serial input to get only the tag
  if(msg1.length() >= 13) {
     msg1=msg1.substring(1,13);
     // Serial.print(msg1);
     // Serial.print(",");
     // Serial.print("ONE");
     // Serial.print(",");
     // Serial.print(rt);
     // Serial.println(";");


  }
  
  while(rfid2.available()>0){
    msg2 += char(rfid2.read());
    delay(1);
  }
  delay(10); // Delay needed to avoid resetting too early
  // parses serial input to get only the tag
  if(msg2.length() >= 13) {
     msg2=msg2.substring(1,13);
//     Serial.print(msg2);
//     Serial.print(",");
//     Serial.print("TWO");
//     Serial.print(",");
//     Serial.print(rt);
//     Serial.println(";");
//     if(msg2 == "002FBE719373"){ // If male

//     }
//     if(msg2 == "002FBE716383"){ // If female
//     }
  }
  
//    }

while(Serial2.available()>0) {
    msg3 += char(Serial2.read());
    delay(1);
  }
  // parses serial input to get only the tag
  if(msg3.length() >= 13) {
     msg3=msg3.substring(1,13);
     // Serial.print(msg3);
     // Serial.print(",");
     // Serial.print("ONE");
     // Serial.print(",");
     // Serial.print(rt);
     // Serial.println(";");


  }

while(Serial3.available()>0) {
    msg4 += char(Serial3.read());
    delay(1);
  }
  // parses serial input to get only the tag
  if(msg4.length() >= 13) {
     msg4=msg4.substring(1,13);
     // Serial.print(msg4);
     // Serial.print(",");
     // Serial.print("ONE");
     // Serial.print(",");
     // Serial.print(rt);
     // Serial.println(";");


  }

// Directionality logic
// male
if (posim == 0 && msg1 == male){// if in cage 0 and activates antenna 1
  posim = 1; // position changes to in the tube by antenna 1
  Serial.print(msg1);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg1 = "";
}
if(posim == 1 && msg1 == male){// if by the tube in antenna 1, and antenna 1 reads again
  posim = 0; // sets position back to cage 0
  Serial.print(msg1);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg1 = "";
}
if(posim <= 1 && msg2 == male){ // if in 0 or 1 and antenna 2 reads, it means they going to 3
  posim = 2; // sets position to cage 2
  Serial.print(msg2);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim == 2 && msg2 == male){ // if in cage 2 and antenna 2 reads
  posim = 3; // sets position to in tube moving from antenna 2 to antenna 1
  Serial.print(msg2);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim >= 2 && msg1 == male){ // if in position 3 or 2, can reset to cage 0 if antenna 1 reads
  posim = 0;
  Serial.print(msg1);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg1 = "";
}
if(posim == 3 && msg2 == male){ // if in position 3 and antenna 2 reads
  posim = 2; // means its going back to cage 2
  Serial.print(msg2);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim == 0 && msg3 == male){ // if in position 3 and antenna 3 reads
  posim = -1; // means its going back to cage -2
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim == -1 && msg3 == male){ // if in position -1 and antenna 3 reads
  posim = 0; // means its going back to main cage
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim >= -1 && msg4 == male){ // if in position -1 and antenna 4 reads
  posim = -2; // means its going to cage -2
  Serial.print(msg4);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim <= -2 && msg3 == male){ // if in position -2 and antenna 3 reads
  posim = 0; // means its going back to cage 0
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim == -2 && msg4 == male){ // if in position -2 and antenna 4 reads
  posim = -3; // means its going back to cage 0
  Serial.print(msg4);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posim == -3 && msg3 == male){ // if in position -3 and antenna 3 reads
  posim = 0; // means its going back to cage 0
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posim);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}

// female - same position tracking as male, but starting in a different place
if (posif == 0 && msg1 == female){
  posif = 1;
  Serial.print(msg1);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg1 = "";
}
if(posif == 1 && msg1 == female){
  posif = 0;
  Serial.print(msg1);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg1 = "";
}
if(posif <= 1 && msg2 == female){
  posif = 2;
  Serial.print(msg2);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif == 2 && msg2 == female){
  posif = 3;
  Serial.print(msg2);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif >=2 && msg1 == female){
  posif = 0;
  Serial.print(msg1);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg1 = "";
}
if(posif == 3 && msg2 == female){
  posif = 2;
  Serial.print(msg2);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif == 0 && msg3 == female){ // if in position 3 and antenna 3 reads
  posif = -1; // means its going back to cage -2
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif == -1 && msg3 == female){ // if in position -1 and antenna 3 reads
  posif = 0; // means its going back to main cage
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif >= -1 && msg4 == female){ // if in position -1 and antenna 4 reads
  posif = -2; // means its going to cage -2
  Serial.print(msg4);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif <= -2 && msg3 == female){ // if in position -2 and antenna 3 reads
  posif = 0; // means its going back to cage 0
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif == -2 && msg4 == female){ // if in position -2 and antenna 4 reads
  posif = -3; // means its going back to cage 0
  Serial.print(msg4);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}
if(posif == -3 && msg3 == female){ // if in position -3 and antenna 3 reads
  posif = 0; // means its going back to cage 0
  Serial.print(msg3);
  Serial.print(" ");
  Serial.print(posif);
  Serial.print(" ");
  Serial.println(rt);
  msg2 = "";
}

// Time calculation again, because of antenna reset delays cause drift
t2 = millis(); // gets current milli output
rt = abs(abs(t2)-abs(t1))+rt; // total milli counter
t1 = t2; // resets t1 for new calculations

// DOOR and IR code
if(adoor ==0){
  int rts1 = digitalRead(sw1); //  vole lever switch
  int rts2 = digitalRead(sw2); // the other lever switch
  if (rts1==1){ // if lever 1 is pressed, door opens
    adoor = 1;
    taop = rt;
    // tone(buzzerpin,frequency,duration(ms)) put buzzer here if you want it to sound when lever is pressed.
    digitalWrite(dir1, LOW);
    digitalWrite(slp1, HIGH);
    for(int i =1;i<=225; i++){ // opens without checking IR status
        digitalWrite(stp1,HIGH);
        delay(2);
        digitalWrite(stp1,LOW);
        delay(2);
    }
    tone(5,440,1000); // tone indicating the door is fully open
    Serial.print("OPEN XX ");
    Serial.println(taop);
    digitalWrite(slp1,LOW); // driver turns off
    
    // put code to withdraw levers here!( should be pretty similar to the for loop above)
    
  }
  if (rts2==1){ // if lever 1 is pressed, door opens
    adoor = 1;
    taop = rt;
    // tone(buzzerpin,frequency,duration(ms)) put buzzer here if you want it to sound when lever is pressed.
    digitalWrite(dir1, LOW);
    digitalWrite(slp1, HIGH);
    for(int i =1;i<=225; i++){ // opens without checking IR status
        digitalWrite(stp1,HIGH);
        delay(2);
        digitalWrite(stp1,LOW);
        delay(2);
    }
    tone(5,440,1000); // tone indicating the door is fully open
    Serial.print("OPEN XX ");
    Serial.println(taop);
    digitalWrite(slp1,LOW); // driver turns off
    
    // put code to withdraw levers here!( should be pretty similar to the for loop above)
    
  }
}

if(bdoor ==0){
  int rts3 = digitalRead(sw3); //  vole lever switch
  int rts4 = digitalRead(sw4); // the other lever switch
  if (rts3==1){ // if lever 3 is pressed, door opens
    bdoor = 1;
    tbop = rt;
    // tone(buzzerpin,frequency,duration(ms)) put buzzer here if you want it to sound when lever is pressed.
    digitalWrite(dir2, LOW);
    digitalWrite(slp2, HIGH);
    for(int i =1;i<=225; i++){ // opens without checking IR status
        digitalWrite(stp2,HIGH);
        delay(2);
        digitalWrite(stp2,LOW);
        delay(2);
    }
    tone(5,440,1000); // tone indicating the door is fully open
    Serial.print("OPEN XX ");
    Serial.println(tbop);
    digitalWrite(slp2,LOW); // driver turns off
    
    // put code to withdraw levers here!( should be pretty similar to the for loop above)
    
  }
  if (rts4==1){ // if lever 4 is pressed, door opens
    bdoor = 1;
    tbop = rt;
    // tone(buzzerpin,frequency,duration(ms)) put buzzer here if you want it to sound when lever is pressed.
    digitalWrite(dir2, LOW);
    digitalWrite(slp2, HIGH);
    for(int i =1;i<=225; i++){ // opens without checking IR status
        digitalWrite(stp2,HIGH);
        delay(2);
        digitalWrite(stp2,LOW);
        delay(2);
    }
    tone(5,440,1000); // tone indicating the door is fully open
    Serial.print("OPEN XX ");
    Serial.println(tbop);
    digitalWrite(slp2,LOW); // driver turns off
    
    // put code to withdraw levers here!( should be pretty similar to the for loop above)
    
  }
}


if(adoor ==1){ // only runs this when the door is open, checks if voles are in opposite cages.
  long aopent = rt-taop; // intitial timer for how long the door is open
  long irtrigta = rt-irtriga; // ir trigger timer, can be adjusted independent of the door open time
  if(aopent>30000&&irtrigta>5000){ // adjust timer values here, aopent is for how long the door stays open after pressing lever. irtrigt is for how long it waits after IR is obstructed in door closing.
    if(posim ==2 && posif == 0){ // if in opposite cages
      digitalWrite(dir1, HIGH);
      digitalWrite(slp1, HIGH);
      for(int j =1;j<=15; j++){ //motor control, 15 samples of IR, each determines obstruction. motor moves after no obstruction is detected.
        digitalWrite(ir2,HIGH); // turns IR LEDs on
        digitalWrite(ir1,HIGH);
        
        // zero counter, doesn't need to be global as its contained to loop
        int ct2 = 0;
        int ct1 = 0;
        
        //int t1 = millis(); // use this for timing
        for(int i=1;i<128;i++){ // IR reading part
          delay(1); // delay needed to adjust samplint rate (1000 Hz)
          // Takes a short sample across the reciever
          // This is a method of reading the IR sensor quickly, as it only outputs a low status when IR blaster is first turned on. This is either because the wavelength drifts, or because the transistor gets saturated. Either way, 128 samples is better that 1.
          int r2 = digitalRead(rec2);  // IR reading
           //Serial.print(r); // not needed but can be commented in if problems
            if (r2 == 0){
              ct2 = ct2+1; // updates 0 counter
            }
          int r1 = digitalRead(rec1);  // IR reading
           //Serial.print(r); // not needed
            if (r1 == 0){
              ct1 = ct1+1; // updates 0 counter
            }
          
        }
        //Serial.println("");
        digitalWrite(ir2,LOW); // turns ir leds off
        digitalWrite(ir1,LOW);
        //int t2 = millis(); // use this for timing
        //int elaps = t2-t1; // use this for timing
        //Serial.println(elaps);
        if (ct2>=1 && ct1 >=1 ){ // if low status is detected (unobstructed)
          //Serial.println("g");
          digitalWrite(led,LOW); // LED off if unobstructed
        }
        else
        {
          //Serial.println("0");
          digitalWrite(led,HIGH); // LED on if obstructed
          delay(500); // short delay for feedback
          digitalWrite(dir1,LOW); // opening
          for(int ii = 1;ii<=25*mtrpos; ii++){ // opens door back up so theres no crushing
            digitalWrite(stp1,HIGH);
            delay(4);
            digitalWrite(stp1,LOW);
            delay(4);
          }
          
          Serial.print("IRobstructed XX ");
          Serial.println(rt);
          digitalWrite(led,LOW);
          ct2 = 0;
          ct1 = 0;
          mtrposa =0;
          digitalWrite(ir2,LOW);
          digitalWrite(ir1,LOW);
          digitalWrite(slp1,LOW);
          irtriga = rt; // to reactivate the close loop
          return; // After door poens, reverts to resting state, door open waiting for another close command
  
          
        }
        ct2 = 0;
        ct1 = 0;
        digitalWrite(ir2,LOW); // natural state of IR LED to prevent burnout
        digitalWrite(ir1,LOW);
         // half second delay to allow receiver to rest, and clearly show pass/fail status
        digitalWrite(led,LOW); // turnes off LED to prevent burnout
        
        for(int k=1; k<=25; k++){ // part where motor actually moves, 450 steps at a time
          digitalWrite(stp1,HIGH);
          delay(4);
          digitalWrite(stp1,LOW);
          delay(4);
          
        
        }
       mtrposa = mtrposa+1; // position updater, counts to 15 for reopening if IR is obstructed
        
        
      }
    tone(5,440,1000);  // tone indicating door is done moving. comment out if you don't want this buzzer
    mtrposa = 0;
    
    Serial.print("CLOSED XX ");  // serial feedback
    Serial.println(rt);
    adoor = 0; // sets door status to closed
    digitalWrite(dir1, LOW);  // turns drivers off
    digitalWrite(slp1, LOW);

    }

    if(posim == 0 && posif ==2){
      digitalWrite(dir1, HIGH);
      digitalWrite(slp1, HIGH);
      for(int j =1;j<=15; j++){ //motor control, 8 samples of IR, each determines obstruction motor moves after no obstruction is detected.
        digitalWrite(ir2,HIGH); // turns IR LED on
        digitalWrite(ir1,HIGH);
        int ct2 = 0;
        int ct1 = 0;
        // zero counter, doesn't need to be global as its contained to loop
        //int t1 = millis(); // use this for timing
        for(int i=1;i<128;i++){ // IR reading part
          delay(1);
          // Takes a short sample across the reciever
          // This is a method of reading the IR sensor quickly, as it only outputs a low status when IR blaster is first turned on. This is either because the wavelength drifts, or because the transistor gets saturated. Either way, 128 samples is better that 1.
          int r2 = digitalRead(rec2);  
           //Serial.print(r);
            if (r2 == 0){
              ct2 = ct2+1;
            }
          int r1 = digitalRead(rec1);  
           //Serial.print(r);
            if (r1 == 0){
              ct1 = ct1+1;
            }
          
        }
        //Serial.println("");
        digitalWrite(ir2,LOW);
        digitalWrite(ir1,LOW);
        //int t2 = millis(); // use this for timing
        //int elaps = t2-t1; // use this for timing
        //Serial.println(elaps);
        if (ct2>=1 && ct1 >=1 ){ // if both IR sensors return unobstructed.
          //Serial.println("g");
          digitalWrite(led,LOW); // LED off if unobstructed
        }
        else
        {
          //Serial.println("0");
          digitalWrite(led,HIGH); // LED on if obstructed
          delay(500);
          digitalWrite(dir1,LOW); // opening
          for(int ii = 1;ii<=25*mtrpos; ii++){ // opens door back up so theres no crushing
            digitalWrite(stp1,HIGH);
            delay(4);
            digitalWrite(stp1,LOW);
            delay(4);
          }
          Serial.print("IRObstructed XX ");
          Serial.println(rt);
          digitalWrite(led,LOW);
          ct2 = 0;
          ct1 = 0;
          mtrpos =0;
          digitalWrite(ir2,LOW);
          digitalWrite(ir1,LOW);
          digitalWrite(slp1,LOW);
          irtriga = rt;
          return; // After door poens, reverts to resting state, door open waiting for another close command
  
          
        }
        ct2 = 0;
        ct1 = 0;
        digitalWrite(ir2,LOW); // natural state of IR LED to prevent burnout
        digitalWrite(ir1,LOW);
         // half second delay to allow receiver to rest, and clearly show pass/fail status
        digitalWrite(led,LOW); // turnes off LED to prevent burnout
        
        for(int k=1; k<=25; k++){ // part where motor actually moves, 450 steps at a time
          digitalWrite(stp1,HIGH);
          delay(4);
          digitalWrite(stp1,LOW);
          delay(4);
          
        
        }
       mtrposa = mtrposa+1; // position updater, counts to 8 for reopening if IR is obstructed
        
        
      }
    tone(5,440,1000);  // tone indicating door is done moving
    mtrposa = 0;
    
    Serial.print("CLOSED XX ");  // serial feedback
    Serial.println(rt);
    adoor = 0;
    digitalWrite(dir1, LOW);  // turns drivers off
    digitalWrite(slp1, LOW);
    }
      
  } // end of door a timing loop


  
} // end of open door a loop


if(bdoor ==1){ // only runs this when the door is open, checks if voles are in opposite cages.
  long bopent = rt-tbop; // intitial timer for how long the door is open
  long irtrigtb = rt-irtrigb; // ir trigger timer, can be adjusted independent of the door open time
  if(bopent>30000&&irtrigtb>5000){ // adjust timer values here, aopent is for how long the door stays open after pressing lever. irtrigt is for how long it waits after IR is obstructed in door closing.
    if(posim ==-2 && posif == 0){ // if in opposite cages
      digitalWrite(dir2, HIGH);
      digitalWrite(slp2, HIGH);
      for(int j =1;j<=15; j++){ //motor control, 15 samples of IR, each determines obstruction. motor moves after no obstruction is detected.
        digitalWrite(ir3,HIGH); // turns IR LEDs on
        digitalWrite(ir4,HIGH);
        
        // zero counter, doesn't need to be global as its contained to loop
        int ct3 = 0;
        int ct4 = 0;
        
        //int t1 = millis(); // use this for timing
        for(int i=1;i<128;i++){ // IR reading part
          delay(1); // delay needed to adjust samplint rate (1000 Hz)
          // Takes a short sample across the reciever
          // This is a method of reading the IR sensor quickly, as it only outputs a low status when IR blaster is first turned on. This is either because the wavelength drifts, or because the transistor gets saturated. Either way, 128 samples is better that 1.
          int r3 = digitalRead(rec3);  // IR reading
           //Serial.print(r); // not needed but can be commented in if problems
            if (r3 == 0){
              ct3 = ct3+1; // updates 0 counter
            }
          int r4 = digitalRead(rec4);  // IR reading
           //Serial.print(r); // not needed
            if (r4 == 0){
              ct4 = ct4+1; // updates 0 counter
            }
          
        }
        //Serial.println("");
        digitalWrite(ir3,LOW); // turns ir leds off
        digitalWrite(ir4,LOW);
        //int t2 = millis(); // use this for timing
        //int elaps = t2-t1; // use this for timing
        //Serial.println(elaps);
        if (ct3>=1 && ct4 >=1 ){ // if low status is detected (unobstructed)
          //Serial.println("g");
          digitalWrite(led,LOW); // LED off if unobstructed
        }
        else
        {
          //Serial.println("0");
          digitalWrite(led,HIGH); // LED on if obstructed
          delay(500); // short delay for feedback
          digitalWrite(dir2,LOW); // opening
          for(int ii = 1;ii<=25*mtrposb; ii++){ // opens door back up so theres no crushing
            digitalWrite(stp2,HIGH);
            delay(4);
            digitalWrite(stp2,LOW);
            delay(4);
          }
          
          Serial.print("IRobstructed XX ");
          Serial.println(rt);
          digitalWrite(led,LOW);
          ct3 = 0;
          ct4 = 0;
          mtrposb =0;
          digitalWrite(ir3,LOW);
          digitalWrite(ir4,LOW);
          digitalWrite(slp2,LOW);
          irtrigb = rt; // to reactivate the close loop
          return; // After door poens, reverts to resting state, door open waiting for another close command
  
          
        }
        ct3 = 0;
        ct4 = 0;
        digitalWrite(ir3,LOW); // natural state of IR LED to prevent burnout
        digitalWrite(ir4,LOW);
         // half second delay to allow receiver to rest, and clearly show pass/fail status
        digitalWrite(led,LOW); // turnes off LED to prevent burnout
        
        for(int k=1; k<=25; k++){ // part where motor actually moves, 450 steps at a time
          digitalWrite(stp2,HIGH);
          delay(4);
          digitalWrite(stp2,LOW);
          delay(4);
          
        
        }
       mtrposb = mtrposb+1; // position updater, counts to 15 for reopening if IR is obstructed
        
        
      }
    tone(5,440,1000);  // tone indicating door is done moving. comment out if you don't want this buzzer
    mtrposb = 0;
    
    Serial.print("CLOSED XX ");  // serial feedback
    Serial.println(rt);
    bdoor = 0; // sets door status to closed
    digitalWrite(dir2, LOW);  // turns drivers off
    digitalWrite(slp2, LOW);

    }

    if(posim == 0 && posif ==-2){
      digitalWrite(dir2, HIGH);
      digitalWrite(slp2, HIGH);
      for(int j =1;j<=15; j++){ //motor control, 8 samples of IR, each determines obstruction motor moves after no obstruction is detected.
        digitalWrite(ir3,HIGH); // turns IR LED on
        digitalWrite(ir4,HIGH);
        int ct3 = 0;
        int ct4 = 0;
        // zero counter, doesn't need to be global as its contained to loop
        //int t1 = millis(); // use this for timing
        for(int i=1;i<128;i++){ // IR reading part
          delay(1);
          // Takes a short sample across the reciever
          // This is a method of reading the IR sensor quickly, as it only outputs a low status when IR blaster is first turned on. This is either because the wavelength drifts, or because the transistor gets saturated. Either way, 128 samples is better that 1.
          int r3 = digitalRead(rec3);  
           //Serial.print(r);
            if (r3 == 0){
              ct3 = ct3+1;
            }
          int r4 = digitalRead(rec4);  
           //Serial.print(r);
            if (r4 == 0){
              ct4 = ct4+1;
            }
          
        }
        //Serial.println("");
        digitalWrite(ir3,LOW);
        digitalWrite(ir4,LOW);
        //int t2 = millis(); // use this for timing
        //int elaps = t2-t1; // use this for timing
        //Serial.println(elaps);
        if (ct3>=1 && ct4 >=1 ){ // if both IR sensors return unobstructed.
          //Serial.println("g");
          digitalWrite(led,LOW); // LED off if unobstructed
        }
        else
        {
          //Serial.println("0");
          digitalWrite(led,HIGH); // LED on if obstructed
          delay(500);
          digitalWrite(dir2,LOW); // opening
          for(int ii = 1;ii<=25*mtrposb; ii++){ // opens door back up so theres no crushing
            digitalWrite(stp2,HIGH);
            delay(4);
            digitalWrite(stp2,LOW);
            delay(4);
          }
          Serial.print("IRObstructed XX ");
          Serial.println(rt);
          digitalWrite(led,LOW);
          ct3 = 0;
          ct4 = 0;
          mtrposb =0;
          digitalWrite(ir3,LOW);
          digitalWrite(ir4,LOW);
          digitalWrite(slp2,LOW);
          irtrigb = rt;
          return; // After door poens, reverts to resting state, door open waiting for another close command
  
          
        }
        ct3 = 0;
        ct4 = 0;
        digitalWrite(ir3,LOW); // natural state of IR LED to prevent burnout
        digitalWrite(ir4,LOW);
         // half second delay to allow receiver to rest, and clearly show pass/fail status
        digitalWrite(led,LOW); // turnes off LED to prevent burnout
        
        for(int k=1; k<=25; k++){ // part where motor actually moves, 450 steps at a time
          digitalWrite(stp2,HIGH);
          delay(4);
          digitalWrite(stp2,LOW);
          delay(4);
          
        
        }
       mtrposb = mtrposb+1; // position updater, counts to 8 for reopening if IR is obstructed
        
        
      }
    tone(5,440,1000);  // tone indicating door is done moving
    mtrposb = 0;
    
    Serial.print("CLOSED XX ");  // serial feedback
    Serial.println(rt);
    bdoor = 0;
    digitalWrite(dir2, LOW);  // turns drivers off
    digitalWrite(slp2, LOW);
    }
      
  } // end of door b timing loop


  
} // end of open door b loop


} // void loop end
