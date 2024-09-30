/*
  A test sketch for all the Slave controller functions as a standalone (NO Bluetooth link)
*/

//Libraries
#include <Adafruit_NeoPixel.h>  //Via Library Manager < https://github.com/adafruit/Adafruit_NeoPixel >
#include <ESP32Servo.h>         //Via Library Manager < https://madhephaestus.github.io/ESP32Servo/index.html > or < https://github.com/madhephaestus/ESP32Servo >
#include <DFPlayer.h>           // Custom < https://github.com/macca448/DFPlayer/tree/main >

//RPM Sensor ACTIVE state
#define ACTIVE_STATE LOW    //Define if your sensor is Active LOW or HIGH in it's triggered state

//PINS
#define MOTOR   27          //MOSFET Motor Sped Control pin
#define BOOST   13          //Neopixel pin for BOOST Rings
#define GLOW    14          //RED LED Pin for BOOST Igniter
#define LEARN   12          //Neopixel pin for Learning Mode pixel strip
#define BT_LED  2           //BT Link-Up as NAV Beacon flash
#define RPM     19          //RPM Sensor pin
#define NOZZLE  4           //Servo pin for nozzle (180 = Closed and 0 = Open)

//NEEDED FOR TESTING IF USING A POT FOR SPEED CONTROL
#define SPEED   35          //POT Wiper pin for Speed control
#define BUTTON  0           //We will use this to change speed control from POT to Serial input

//TIMING CONSTANTS - Some of these constants can be adjusted to suit personal preference
#define BEACON_TIMER  40    //The Beacon uses the 1mS non-blocking duty in the loop so this is 40mS
#define BEACON_STEP   5     //PWM fade step each "BEACON_TIMER" loop
#define BEACON_RAMP   50    //BEACON_TIMER * BEACON_RAMP = 2 seconds
#define SWEEP_PERIOD  10    //Servo easing step period in mS
#define SERVO_STEP    2     //Increase this to speed up the sero travel speed
#define AB_TIMEOUT    30    //Time-out in Seconds for the AB BOOST
#define MIL_THR_VAL   60    //Trigger point as % for AB Boost effect in final code it will be 90%
#define RPM_FACTOR    2     //Because I have a 2 count per one revolution
#define ONE_SEC       1000  //Multiple timeres using variable counters using the loops 1mS clock

//CONSTANTS
#define DUTY      1000UL    //This is our primary loop timing duty period of 1000uS or 1mS
#define DEBOUNCE  50        //50 * 1mS = 50mS button de-bounce
#define PWM_MAX   255       //PWM maximum
#define PWM_MIN   0         //PWM minimum

#define RED50     0x7F0000  //50% Red
#define WHITE50   0x7F7F7F  //50% White
#define ORANGE50  0x7F4C00  //50% Orange
#define RED75     0xBF0000  //75% Red
#define WHITE75   0x7F7F7F  //75% White
#define ORANGE75  0xFF8C00  //75% Orange
#define ORANGE100 0xFFA500  //100% Orange
#define RED100    0xFF0000  //100% Red
#define WHITE100  0xFFFFFF  //100% White

#define MP3_SERIAL_SPEED    9600      //DFPlayer Mini suport only 9600-baud
#define MP3_SERIAL_TIMEOUT  350       //average DFPlayer response timeout 200msec..300msec for YX5200/AAxxxx chip & 350msec..500msec for GD3200B/MH2024K chip

//NEOPIXEL
#define BOOST_COUNT 30                                              //Boost LED count
#define LEARN_COUNT 24                                              //Learning Strip LED count
Adafruit_NeoPixel boost(BOOST_COUNT, BOOST, NEO_GRB + NEO_KHZ800);  //Boost Neopixel Rings Object
Adafruit_NeoPixel learn(LEARN_COUNT, LEARN, NEO_GRB + NEO_KHZ800);  //Learning Neopixel Strip Object

//Servo Object 
Servo servo;

//DFPlayer Object
DFPlayer mp3;

//VARIABLES
volatile uint16_t rpmCount = 0;
static int  servoOpen = 0, servoClosed = 180;
uint32_t    lastMicros = 0, lastBoostPeriod, abColor, boostStepDelay, startMillis,  
            colours[]{RED50, ORANGE50, RED75, ORANGE75, RED100, ORANGE100, WHITE50};
bool        learnState = 1, runAB = 0, boostOn = 0, flicker = 0, beaconEN = 1, goLearn = 0, pwmUpdate = 0,
            doNav = 0, moveNozzle = 0, pressed = 0, lastPress = 1, usePOT = 0, up = 1, running = 0, lastRunning = 0;
int         redLearn = PWM_MIN, blueLearn = PWM_MAX, navPWM = PWM_MIN, servpPos = servoOpen, boostCount = 0,
            glowPWM = 0, beaconCount = 0, beaconState = 0, isrCount = 0;
uint16_t    lastRPM = 1000, learnCount = 0, speedPinVal;
uint8_t     learnPixel = 0, learnStep = 0, boostSeq = 0, randomCount, flickerCount = 0, beaconStep = 0, rgbVal,
            lastSpeedVal = 255, lastSpeedPC = 0, boostTimeOut, nozzleStepCount = 0, nozzleTo = servoOpen, beaconTimer = 0, 
            speedVal, speedPercent, dbTimer = 0;


// Sketch TAB's as include headers - Order cab be important so DO NOT MOVE!
#include "neo_LM_strip.h"
#include "boostnGlow.h"
#include "nav_bt.h"

//ISR RPM COUNTER
void IRAM_ATTR rpmISR(void){
  rpmCount++;
}

void isr_mp3_change(bool engineState){                                //Called when motor starts or Stops
  if(engineState){
      mp3.playTrack(1);                                               //Will play 001.mp3 file in SD card root (Engine Start)
      delay(300);                                                     
      rpmCount = 0;                                                   //Zero's RPM counter
      if(ACTIVE_STATE == LOW){                                        //Attach interrupt as motor has started to spin
        attachInterrupt(digitalPinToInterrupt(RPM), rpmISR, FALLING);  //If RPM Sensor is Active LOW (line 14)
      }else{
        attachInterrupt(digitalPinToInterrupt(RPM), rpmISR, RISING);  //If RPM Sensor is Active HIGH (line 14)
      }
    }else{
      detachInterrupt(RPM);                                           //Motor has Stopped so we detach the ISR
      mp3.playTrack(2);                                               //Will play 002.mp3 file in SD card root (engine shut down)
      delay(300);
      running = 0;
    }
    return;
}

void doList(bool fail){                                             //Serial print statements called after each input
  if(fail){
    Serial.println(F("ERROR with data input - Please try again\n"));
  }
  Serial.println(F("Use button to change between POT and Serial Input for speed control"));
  Serial.println(F("c or C to Change Servo Open/Close Direction"));
  Serial.println(F("n or N to toggle Navigation BT Beacon"));
  Serial.println(F("l or L to toggle Learning Strip Effect ON or OFF"));
  Serial.println(F("sxxx or Sxxx where xxx is 0 to 255 for Serial Speed Control"));
  delay(50);
  return;
}

void setup() {
  Serial.begin(115200);                 //Start Serial for debug in control
  pinMode(BUTTON, INPUT);               //Setup buttons and LED's
  pinMode(RPM, INPUT);
  pinMode(SPEED, INPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(GLOW, OUTPUT);
  pinMode(BT_LED, OUTPUT);
  analogWrite(GLOW, PWM_MIN);           // Start in known state
  analogWrite(BT_LED, PWM_MIN);         // Start in known state
  analogWrite(MOTOR, PWM_MIN);          // Start in known state

  //Setup Servo
  ESP32PWM::allocateTimer(3);       // Only need one timer as there is only one servo
	servo.setPeriodHertz(50);         // standard 50 hz servo
  servo.attach(NOZZLE, 540, 2450);  // attaches servo on pin 5, 540uS MIN, 2450uS MAX (for 0 to 180 degree on a standard servo)
  servo.write(servpPos);            // start position at 0 angle position

  //Start Neopixel objects
  boost.begin();                    // INITIALIZE NeoPixel strip object for AB Boost effect
  boost.show();                     // Turn OFF all pixels
  learn.begin();                    // INITIALIZE NeoPixel strip object for Learing Strip effect
  learn.show();                     // Turn OFF all pixels

  randomSeed(A0);                   // Using A0 input for a seed value to create random numbers for AB start sequence
  randomCount = random(2, 8);       // Creates a random number between 2 and 8

  Serial2.begin(MP3_SERIAL_SPEED, SERIAL_8N1, 16, 17);                //DFPlayer ESP32 RX2(gpio16) TX2(gpio17) 
  mp3.begin(Serial2, MP3_SERIAL_TIMEOUT, DFPLAYER_MP3_TF_16P, false); //"DFPLAYER_MP3_TF_16P" see NOTE, false=no feedback from module after the command
  delay(250);
  mp3.stop();        //if player was runing during ESP8266 reboot
  delay(250);
  mp3.reset();       //reset all setting to default 
  delay(250);
  mp3.setSource(2);  //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash 
  delay(250); 
  mp3.setEQ(0);      //0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  delay(250);
  mp3.setVolume(30); //0..30, module persists volume on power failure
  delay(250);
  Serial.println(F("Setup complete - Starting Main Loop"));
}

void loop() {                         // NON-Blocking loop (No delay()'s)

  if(usePOT){
    speedPinVal  = analogRead(SPEED);              // Add a POT for motor Speed control
    speedVal = map(speedPinVal, 0, 4095, 0, 255); // Create the PWM value from POT value
    speedPercent = map(speedVal, 0, 255, 0, 100); // Convert PWM to Percentage value
  }
  if(running != lastRunning){                     //When motor starts or Stops
    isr_mp3_change(running);
  }
  
  bool press = digitalRead(BUTTON);       //Watch for button press

  if(press != lastPress){                 //On button state change this is true
    pressed = 1;
    dbTimer = 0;
  }

  if(runAB){                              // Outside loop timer because it uses it's own
    boostAndGlow();                       // Glow fade-up, flicker and nozzle close
  }
  
  if(micros() - lastMicros >= DUTY){      // Loop Duty period is 1mS (1000uS)
    lastMicros = micros();
    isrCount++;                           // Counter to create a 1 second print cycle
    dbTimer++;                            // De-bounce timing counter
      
      if(doNav){
        doNavBT();                        // BT link up as an avaiation nagigation light fade up / blink pattern
      }
      if(moveNozzle){
        nozzleMove();                     // Triggered by BOOST ON/OFF 
      }
      if(goLearn){
        learnWipe();                      // Simple Blue to Red strip wipe effect IE: front to back wipe
      }

      if(dbTimer >= DEBOUNCE){            //Button de-bounce (non-blocking)
        if(pressed){
          if(press == 0){                 //Only true if button is pressed
            usePOT = !usePOT;             //Use POT for speed control ON/OFF toggle
            Serial.println(usePOT ? "Using POT for Speed Control" : "Using Serial Command for Speed Control");
          }
          pressed = 0;
        }
      }

      if(speedPercent != lastSpeedPC){              // Only look inside if there has been a change
        if(speedPercent >= MIL_THR_VAL && !runAB){  // When true the AB Boost sequence will run
          boostTimeOut = AB_TIMEOUT;                // Creates a BOOST countdown and time-out period 
          boostOn = 1;
          runAB = 1;
          boostSeq = 0;
          boostStepDelay = 10UL;
          nozzleTo = servoClosed;
          moveNozzle = 1;          
        }else if(speedPercent < MIL_THR_VAL){      // Shut's off and resets AB Boost when true
          boostOn = 0;
          runAB = 0;
          analogWrite(GLOW, PWM_MIN);
          boost.clear();
          boost.show();
            if(nozzleTo != servoOpen){
              nozzleTo = servoOpen;
              moveNozzle = 1;
            }          
        }
      }
      if(isrCount >= ONE_SEC){                      //non-blocking one second delay
        uint16_t rpmVal = 0;
        isrCount = 0;
          if(running){
            noInterrupts();
            rpmVal = (rpmCount / RPM_FACTOR) * 60;  // Calculate RPM
            rpmCount = 0;
            interrupts();
          }
          if(pwmUpdate){                            //PWM only prints if it changed during the last delay period
            Serial.printf("Current PWM Val: %u \tMotor RPM: %u\n", speedVal, rpmVal);
            pwmUpdate = false;
          }
          if(boostOn){                              //Boost time-out statement
            boostTimeOut -= 1;
            Serial.printf("BOOST Time-Out in: %u seconds\n", boostTimeOut);
              if(boostTimeOut == 0){
                boostOn = 0;
                nozzleTo = servoOpen;
                moveNozzle = 1;
                boost.clear();
                boost.show();
                analogWrite(GLOW, PWM_MIN);
              }
          }
        lastRPM = rpmVal;                             //So we know when the RPM changes
      } 
      if(speedVal != lastSpeedVal){                   //Only true is there is a change in the speed value      
        running = startStop(speedVal, lastSpeedVal);  //Test if motor is running or not and set flag
        analogWrite(MOTOR, speedVal);                 //Update motor PWM value
        pwmUpdate = true;                             //To print current PWM value
      }
      lastRunning = running;                          //To watch fo a state change
      lastSpeedPC = speedPercent;
      lastSpeedVal = speedVal;
  }

  // The following Serial Statements are for testing only and will be replaced by BTSerial or ESP-NOW functions
  if(Serial.available() > 0){ 
    String inString = Serial.readStringUntil('\n');
      if(inString.startsWith("c") || inString.startsWith("C")){
        up = !up;
        moveNozzle = 1;
        Serial.println(F("Servo Sweep Direction Swapped"));
          if(up){
            servoOpen = 0;
            servoClosed = 180;
          }else{
            servoOpen = 180;
            servoClosed = 0;
          }
          int currentPos = servo.read();
            if((currentPos > 90 && up) || (currentPos < 90 && !up)){
              nozzleTo = servoOpen;
            }else if((currentPos < 90 && up) || (currentPos > 90 && !up)){
              nozzleTo = servoClosed;
            }        
      }else if(inString.startsWith("l") || inString.startsWith("L")){
        goLearn = !goLearn;
        Serial.println(goLearn ? "Learn On":"Learn Off");
        if(goLearn){
          learnCount = 199;
          learnStep = 9;
          blueLearn = 255;
          redLearn = 0;
        }else{
          learn.clear(); 
          learn.show();
        }
      }else if(inString.startsWith("n") || inString.startsWith("N")){
        doNav = !doNav;
        Serial.println(doNav ? "Nav On":"Nav Off");
        if(!doNav){
          analogWrite(BT_LED, PWM_MIN);
        }
      }else if(inString.startsWith("s") || inString.startsWith("S") && usePOT == false){
        String sub1 = inString.substring(1);
        uint8_t subVal = sub1.toInt();
          if(subVal >= PWM_MIN && subVal <= PWM_MAX){
            speedVal = subVal;
            speedPercent = map(speedVal, 0, 255, 0, 100); // Convert PWM to Percentage value
            Serial.printf("Motor PWM: %u \t RPM: %u %%\n", speedVal, speedPercent);
          }
      }else{
        doList(1);
      }
      doList(0);
  }
  lastPress = press;
}
