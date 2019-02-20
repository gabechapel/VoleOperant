// The following initiatlize libraries used in FED code
#include <avr/sleep.h>
#include <avr/power.h>
#include <SdFat.h>               
#include <Adafruit_MotorShield.h>

// Intitialzing global variables
File dataFile;
#define FILENAME "FED_DATA.csv"  // Change this to alter CSV file name
#define PHOTO_INTERRUPTER_PIN 2 // This initializes the pin on the Arduino that the photointerrupter is connected to

SdFat SD; // defining an object SD

int PIState = 1;
int lastState = 1;
int leverPin = 3; // This initiatlizes the pin on the Arduino that the BNC output is connected to
int leverState = 0; // 1 when the lever is pressed
int startTone = 6; //Pin that controls the start tone
int pelletTone = 5; //pin that controls the tone when a pellet is dispensed
int leverLight = 4; //pin that controls the light to signal the vole that the lever can be pressed

// Defining constants for calculating timing
long t1 = 0;
long t2 = 0;
long dt = 0;//time that has passed
// Define experimental timing constants
int time1 = 2000;
int time2 = 2000;
int time3 = 1000;
int time4 = 2000;
int timeInter = 120000;

const int CS_pin = 10;  // This initializes the SD card on pin 10

// Setting up the stepper motor 
const int STEPS_TO_INCREMENT = 64;
const int MOTOR_STEPS_PER_REVOLUTION = 513;
Adafruit_MotorShield gMotorShield = Adafruit_MotorShield();
// Set the second argument to 1 to use M1 and M2 on the motor shield, or set as 2 to use M3 and M4:
Adafruit_StepperMotor *gPtrToStepper = gMotorShield.getStepper(MOTOR_STEPS_PER_REVOLUTION,1); 

int counter = 1;
int max_counter = 10;
int dispense = 1; //Initially don't dispense the pellet

void setup(){
  // make all unused pins inputs with pullups enabled by default, lowest power drain
  // leave pins 0 & 1 (Arduino RX and TX) as they are
  for (byte i=4; i <= 20; i++) {    
    pinMode(i, INPUT_PULLUP);     
  }
  
  // this saves power by disabling ADC as we won't be using it
  ADCSRA = 0;  
  power_adc_disable(); // ADC converter
  power_timer1_disable();// Timer 1
  power_timer2_disable();// Timer 2

  // This starts serial monitoring - With FED connected, open the Serial Monitor in the 
  // Arduino IDE to watch FED output in real-time for debugging
  Serial.begin(9600);
  Serial.println(F("Starting up..."));

  // Set Arduino pins modes to input or output
  pinMode(PHOTO_INTERRUPTER_PIN, INPUT);
  pinMode(leverPin, INPUT_PULLUP);
  pinMode(CS_pin, OUTPUT);
  pinMode(startTone, OUTPUT);
  pinMode(pelletTone, OUTPUT);
  pinMode(leverLight, OUTPUT);

  //Starting the Motoshield libaries
  gMotorShield.begin(); // use default I2C address of 0x40
  
  // Set stepper rpm, 255 max for this 5V stepper
  gPtrToStepper->setSpeed(255);
  
  delay (50);  // delay helps give the card a bit more time
  lastState = 0; //Assumes there is no pellet in the dispenser
}

//The following is the main loop of the FED code
void loop() {
power_twi_enable();
dispense = 0; //Set the loop to start

// SECTION 1, First set the tone for 2 seconds
digitalWrite(startTone, HIGH);
delay(time1); //Wait 2 seconds
digitalWrite(startTone, LOW); // Turn off the tone

//SECTION 2, Wait two seconds or until there is a lever press to dispense the pellet
t1 = millis(); //Get the time
digitalWrite(leverLight, HIGH);
while ((leverState == 0) && (dt < time2)) {//While both the lever is unpressed and less than 2 seconds have passed
  leverState = digitalRead(leverPin);
  t2 = millis();
  dt = abs(t2 - t1);
}
if (leverState == 1) { //If the vole pressed the lever
  digitalWrite(leverLight, LOW); //Turn off the lever LED here
}

// SECTION 3, The following checks if the pellet has been removed, and if it has it dispenses another pellet.
while (dispense == 0){
   PIState = digitalRead(PHOTO_INTERRUPTER_PIN); //Reads state of the IR beam, 1 = open, 0 = blocked
   Serial.print("Photointerrupter State: "); Serial.println(PIState);
   if (PIState == 1 && PIState != lastState) { //If its unblocked and wasn't unblocked before, PELLET WAS TAKEN  
     power_twi_enable();
     power_spi_enable();
    
     while ( counter <= max_counter) {
       PIState = digitalRead(PHOTO_INTERRUPTER_PIN);
       if (PIState == 0) {
         lastState = PIState; // lastState becomes LOW
         break;
       }
       else if (counter == max_counter) {
         lastState = PIState; // lastState becomes HIGH
       }
       counter++;
     }
     counter = 0;
   }
    
   else if (PIState == 1) { //If its unblocked, NOTHING HAS DISPENSED
     power_twi_enable();
     gPtrToStepper->step(STEPS_TO_INCREMENT/2, FORWARD, DOUBLE);
     delay (100);//500
     gPtrToStepper->step(STEPS_TO_INCREMENT/2,BACKWARD,DOUBLE);
     delay(50);//500
     gPtrToStepper->release();
     power_twi_disable();
     lastState = PIState;
   }
    
   else if (PIState == 0 && (PIState != lastState)) { // IF its blocked and something wasn't blocking it before, PELLET WAS DISPENSED
     lastState = PIState; // lastState becomes LOW
     digitalWrite(pelletTone, HIGH);
     delay(time3); //Keep the tone going for 1 second
     digitalWrite(pelletTone, LOW); // Turn off the pellet tone
     dispense = 1;
   }
    
   else {
     lastState = PIState;
     enterSleep();
     dispense = 1;
   }
   delay(100);
 }
 //SECTION 4
 if (leverState == 0) { //If the vole never pressed the lever
  delay(time4); //Wait 2 seconds
  digitalWrite(leverLight, LOW); //Turn off the lever light
 }

 //SECTION INTERTEST
 delay(timeInter);
}
//END OF TEST

// function for entering sleep mode to save power
void enterSleep() {
  power_usart0_disable();// Serial (USART)
  sleep_enable();
  attachInterrupt(0, pinInterrupt, RISING);
  lastState = 0;
  delay(100);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  cli();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  sleep_disable();
}

// function for allowing the FED to wakeup when pellet is removed
void pinInterrupt(void) {
  detachInterrupt(0);
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  /* Re-enable the serial. */
  power_usart0_enable();
}
