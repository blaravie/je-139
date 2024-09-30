//Libraries
#include <Arduino.h>
//#include <Preferences.h>
#include <Wire.h>                           //Part of board drivers
#include <AS5600.h>                         //Library Manager or https://github.com/RobTillaart/AS5600
#include <hd44780.h>                        //Library Manager or https://github.com/duinoWitchery/hd44780
#include <hd44780ioClass/hd44780_I2Cexp.h>  //LCD with i2c expander i/o class header
#include <ESP32RotaryEncoder.h>             //Library Manager or https://github.com/MaffooClock/ESP32RotaryEncoder

/*  Sets rotation direction for low throttle to high throttle incremental numbering 
    for clockwise it's "#define MAG_DIR  AS5600_CLOCK_WISE"
    for counter clockwise it's #define MAG_DIR  AS5600_COUNTERCLOCK_WISE            */
//#define MAG_DIR  AS5600_CLOCK_WISE        
#define MAG_DIR  AS5600_COUNTERCLOCK_WISE

//I/O Pins
#define SW          15                //Start Switch - INPUT_PULLUP
#define BT_LED      2                 //BT Link indicator - OUTPUT
#define ON_LED      13                //YELLOW ON LED - OUTPUT
#define READY_LED   12                //GREEN READY LED - OUTPUT
#define AB_LED      14                //RED AB LED - OUTPUT
#define LM_LED      27                //ORANGE LEARNING MODE LED - OUTPUT
#define EN_BTN      26                //ENCODER BUTTON - INPUT_PULLUP
#define EN_CLK      34                //ENCODER CLOCK - INPUT
#define EN_DA       39                //ENCODER DATA - INPUT
#define MAG_DIR_PIN 32                //MAGNETIC ENCODER SET DIRECTION - OUTPUT

//Constants
#define PERIOD      50
#define REFRESH_LCD 400               //50mS x 400 = 20 Seconds

const byte ledCount = 5, fxCount = 26;

//Variables
int       angle, playStopCount, lcdRefresh = 0;
uint8_t   larsonIndex, throttle, lcdCount = 0, lcdTask = 0, lastLcdTask = 1, menu, chaseSpeed = 1, chaseCount;
long      lastEncNum = 1, encNum = 0; 
bool      swState, swLastState = 1, ledState = 0, blink = 0, larson = 0, update = 0, runOnce = 1, playStop = 0,
          up = 1, pressed = 0, lastBtn = 1, singlePress = 0, refreshDisplay = 0, updateVals = 0;
uint32_t  lastPeriod = 0;

static int lowValue = 0, highValue = 0;

//Objects (Attached devices with libraries)
hd44780_I2Cexp lcd;                           //LCD with I2C backpack 
AS5600 as5600;                                //use default Wire
RotaryEncoder rotaryEncoder(EN_CLK, EN_DA);   //Rotary Encoder (Dial/Knob)

#include "array_data.h"
#include "core_zero_loop.h"
#include "core_one_loop.h"

void setup(){
  //Serial.begin(115200);                                      //Serial Monitor Input and Feedback
  
  pinMode(SW, INPUT_PULLUP);
  pinMode(EN_BTN, INPUT_PULLUP);

  rotaryEncoder.setEncoderType(EncoderType::SW_FLOAT);
  rotaryEncoder.setBoundaries(0, 3, true);
  rotaryEncoder.onTurned( &knobCallback );
  rotaryEncoder.begin();

  for(byte i = 0; i < ledCount; i++){
    pinMode(leds[i], OUTPUT);
    delay(250);
    digitalWrite(leds[i], HIGH);
    delay(250);
    digitalWrite(leds[i], LOW);
  }
    
  Wire.begin(SDA, SCL);

  int status = lcd.begin(20, 4);
	if(status) // non zero status means it was unsuccesful
	{
		// hd44780 has a fatalError() routine that blinks an led if possible
		// begin() failed so blink error code using the onboard LED if possible
		hd44780::fatalError(status); // does not return
	}
  
  as5600.begin(MAG_DIR_PIN);                      //  set direction pin.
  as5600.setDirection(MAG_DIR);                   //  default, just be explicit. see lines 10 to 12 
  bool magConnected = as5600.isConnected();
  //Serial.println(magConnected ? "Mag Enc Connected" : "Mag Enc Failed");
  delay(500);

  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  if(!magConnected){
    lcd.print("Mag Encoder Fail");
    lcd.setCursor(0,1);
    lcd.print("Setup Terminated");
    lcd.setCursor(0,2);
    lcd.print("Check Wiring and");
    lcd.setCursor(0,3);
    lcd.print("Driver Configuration");
    while(!magConnected){
      digitalWrite(leds[3], !digitalRead(leds[3]));
      delay(250);
    }
  }else{
    lcd.print(configLabels[Main]);
    lcd.setCursor(0,1);
    lcd.print(configLabels[Wiring]);
    lcd.setCursor(0,2);
    lcd.print(configLabels[Press]);
    lcd.setCursor(0,3);
    lcd.print(configLabels[Continue]);
  }

  while (digitalRead(EN_BTN) == 1){delay(10);}

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(ch0);
  lcd.setCursor(0,2);
  lcd.print(configLabels[Set]);
  lcd.setCursor(0,3);
  lcd.print(configLabels[Turn]); 

  xTaskCreatePinnedToCore(loop1, "loop1", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(loop0, "loop0", 4096, NULL, 1, NULL, 0);
}

void loop(){}
