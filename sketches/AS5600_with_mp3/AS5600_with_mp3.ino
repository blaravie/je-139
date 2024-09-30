//
//    FILE: AS5600_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: https://github.com/RobTillaart/AS5600
//
//  Examples may use AS5600 or AS5600L devices.
//  Check if your sensor matches the one used in the example.
//  Optionally adjust the code.


#include <AS5600.h>
#include <DFPlayer.h>

#if defined (ESP32)
  #define MP_SERIAL Serial2            //ESP32 TX2 (GPIO17) / RX2 (GPIO16)
#elif defined (ESP8266)
  #define MP_SERIAL Serial1            //ESP8266 TX1 (GPIO2) ONLY  IE No Feedback from Player
#endif

#define BUTTON 26
#define MP3_SERIAL_SPEED    9600
#define MP3_SERIAL_TIMEOUT  350


AS5600 as5600;   //  use default Wire
DFPlayer mp3;

bool pressed = 0, press, lastPress = 1;
uint8_t file = 1, lastFile = 0;
uint32_t lastPeriod = 0;
int count = 0;

void startWire(void){
  Wire.begin(SDA, SCL);
  as5600.begin(32);  //  set direction pin.
  as5600.setDirection(AS5600_COUNTERCLOCK_WISE);  //  default, just be explicit.
  int b = as5600.isConnected();
  Serial.print("Connect: ");
  Serial.println(b);
  delay(1000);
  return;
}

void setup(){
  Serial.begin(115200);
  
  MP_SERIAL.begin(MP3_SERIAL_SPEED, SERIAL_8N1);             //ESP32 RX2(gpio16) TX2(gpio17)   ESP8266 TX1 ONLY (gpio2)
  mp3.begin(MP_SERIAL, MP3_SERIAL_TIMEOUT, DFPLAYER_MP3_TF_16P, false); //"DFPLAYER_MP3_TF_16P" see NOTE, false=no feedback from module after the command
  
  delay(1000);
  mp3.stop();        //if player was runing during ESP8266 reboot
  mp3.reset();       //reset all setting to default  
  mp3.setSource(2);  //1=USB-Disk, 2=TF-Card, 3=Aux, 4=Sleep, 5=NOR Flash  
  mp3.setEQ(0);      //0=Off, 1=Pop, 2=Rock, 3=Jazz, 4=Classic, 5=Bass
  mp3.setVolume(30); //0..30, module persists volume on power failure
  
  pinMode(BUTTON, INPUT_PULLUP);
  Serial.println(__FILE__);
  Serial.print("AS5600_LIB_VERSION: ");
  Serial.println(AS5600_LIB_VERSION);
  startWire();
  delay(1000);
  mp3.playTrack(2);
  delay(1000);
}


void loop(){
  press = digitalRead(BUTTON);
  
  if(press != lastPress){
    pressed = 1;
    delay(50);
    if(press == LOW){
      mp3.stop();
      delay(500);
        if(file == 1){
          mp3.playTrack(1);
          delay(500);
        }else{
          mp3.playTrack(2);
          delay(500);
        }
        file++;
        file %= 2;
        Serial.printf("Play file: %u\n\n", file);
    }
    pressed = 0;
  }
  
  if(millis() - lastPeriod >= 50){
    lastPeriod = millis(); count++;
      if(count >= 30 && !pressed){
        int angle = (float(as5600.rawAngle() * AS5600_RAW_TO_DEGREES)+0.5);
        Serial.printf("Rounded Angle int: %i°\n\n", angle);
        count = 0;
      }
  }
  lastPress = press;
  lastFile = file;
}


//  -- END OF FILE --
