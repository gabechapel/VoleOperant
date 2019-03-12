//Ryan Cameron
//Created: 2/20/2019
// This simply takes a ttl pulse and reads it when the pellet should be dispensed, then sends a signal when the pellet is taken

// The following initiatlize libraries used in FED code
#include <avr/sleep.h>
#include <avr/power.h>
#include <Adafruit_MotorShield.h>

//#define PHOTO_INTERRUPTER_PIN 2 // This initializes the pin on the Arduino that the photointerrupter is connected to

//State variables
int PIState;
int lastState = 1;
int leverState; // 1 when the lever is pressed

//Pin variables
int leverPin = 3; // This initiatlizes the pin on the Arduino that the BNC output is connected to
int dispensePin = 7;
int takePin = 8;
int PHOTO_INTERRUPTER_PIN = 2;

const int CS_pin = 4;  // This initializes the SD card on pin 10

// Setting up the stepper motor
const int STEPS_TO_INCREMENT = 64;
const int MOTOR_STEPS_PER_REVOLUTION = 513;
Adafruit_MotorShield gMotorShield = Adafruit_MotorShield();
// Set the second argument to 1 to use M1 and M2 on the motor shield, or set as 2 to use M3 and M4:
Adafruit_StepperMotor *gPtrToStepper = gMotorShield.getStepper(MOTOR_STEPS_PER_REVOLUTION, 1);

int counter = 1;
int max_counter = 10;
int dispense = 0; //Initially don't dispense the pellet
int take = 0;

//Timer Variables
long t1 = 0;
long t2 = 0;
long dt = 0;

void setup() {
  // make all unused pins inputs with pullups enabled by default, lowest power drain
  // leave pins 0 & 1 (Arduino RX and TX) as they are
  for (byte i = 4; i <= 20; i++) {
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
  pinMode(leverPin, INPUT);
  pinMode(CS_pin, OUTPUT);
  pinMode(dispensePin, OUTPUT);
  pinMode(takePin, OUTPUT);
  digitalWrite(leverPin, LOW);
  digitalWrite(dispensePin, LOW);
  digitalWrite(takePin, LOW);

  //Starting the Motoshield libaries
  gMotorShield.begin(); // use default I2C address of 0x40

  // Set stepper rpm, 255 max for this 5V stepper
  gPtrToStepper->setSpeed(255);

  delay (50);  // delay helps give the card a bit more time
//  lastState = 0; //Assumes there is no pellet in the dispenser
}

//The following is the main loop of the FED code
void loop() {
  leverState = digitalRead(leverPin);
  if (leverState == 1) {//If we want to dispense a pellet
    power_twi_enable();
    dispense = 0; //Set the loop to start
    take = 0;
    // SECTION 3, The following checks if the pellet has been removed, and if it has it dispenses another pellet.
      PIState = digitalRead(PHOTO_INTERRUPTER_PIN); //Reads state of the IR beam, 1 = open, 0 = blocked
      Serial.print("Photointerrupter State: "); Serial.println(PIState);

////////// Wait For Pellet To Be Dispensed //////////
      while (dispense == 0){
        // Start rotating motor to dispense a pellet
        power_twi_enable();
        gPtrToStepper->step(STEPS_TO_INCREMENT / 2, FORWARD, DOUBLE);
        delay (200);//500
        gPtrToStepper->step(STEPS_TO_INCREMENT / 2, BACKWARD, DOUBLE);
        delay(50);//500
        gPtrToStepper->release();
        power_twi_disable();

        //Read photo interrupter
        PIState = digitalRead(PHOTO_INTERRUPTER_PIN); //Reads state of the IR beam, 1 = open, 0 = blocked
        Serial.println("Photointerrupter State: "); Serial.println(PIState);

        if (PIState == 0){ // if pellet was dispensed
          dispense = 1;
          Serial.println("DISPENSE HIGH");
          digitalWrite(dispensePin,HIGH); //Send signal that pellet was dispensed
          t1 = millis(); //get time when dispense pin is set to high
        }
      }
      
////////// Wait For Pellet To Be Taken //////////
      while (take == 0){
        Serial.println("TAKE 0");
        // turn off dispense pin after 1s
        t2 = millis();
        dt = abs(t2 - t1);
        if(dt >= 1000){
          digitalWrite(dispensePin,LOW);
          Serial.println("DISPENSE LOW");
        }
        
        //Read photo interrupter
        PIState = digitalRead(PHOTO_INTERRUPTER_PIN); //Reads state of the IR beam, 1 = open, 0 = blocked
        if (PIState == 1){ // if pellet was taken
          Serial.println("TAKEN");
          power_twi_enable();
          power_spi_enable();
          while ( counter <= max_counter) {
            PIState = digitalRead(PHOTO_INTERRUPTER_PIN);
            if (PIState == 0) {
              break;
            }
            else if (counter == max_counter) {
              take = 1;
              digitalWrite(takePin, HIGH); //Send signal that the pellet was taken
              Serial.println("TAKEN HIGH");
              delay(1000);
              digitalWrite(takePin,LOW);
              Serial.println("TAKEN LOW");
            }
            counter++;
          }
          counter = 0;
        }
      }
      enterSleep();
  }
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
