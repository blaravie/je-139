void knobCallback( long value ){
	Serial.printf( "Value: %ld\n", value );
  encNum = value;
}

void encoderTextCallback(void){
    if(encNum != lastEncNum){
      if(runOnce){
          if(encNum == 0 || encNum == 2){
            lcd.setCursor(0,1);
            lcd.print(configLabels[StopPos]);
          }else if(encNum == 1 || encNum == 3){
            lcd.setCursor(0,1);
            lcd.print(configLabels[FullPos]);
          }
      }else{
        if(encNum == 3){
          menu = Menu4;
        }else if(encNum == 2){
          menu = Menu3;
        }else if(encNum == 1){
          menu = Menu2;
        }else if(encNum == 0){
          menu = Menu1;
        }
        lcd.setCursor(0,1);
        lcd.print(mainLabels[menu]);
      }
      lastEncNum = encNum;
    }
    return;
}

// Because Non-Blocking tasks are all in the Core 1 loop we can run
// dirty blocking tasks on Core 0 which now is just the LCD char strings
// being read from PROGMEM. Might be better to copy to RAM first. Testing needed here
// but it works fine as it is

void loop0(void *pvParameters){    // Core 0 loop - User tasks
  while (1){

    if(lcdTask != lastLcdTask){
        switch (lcdTask){
          case 4:
            chaseSpeed = 20;
            break;
          case 3:
            chaseSpeed = 10;
            break;
          case 2:
            chaseSpeed = 5;
            break;
          case 1:
            chaseSpeed = 1;
            break;
        }
        lastLcdTask = lcdTask;
    }

   encoderTextCallback();

    if(singlePress){  
      if(runOnce){      
        if(encNum == 0 || encNum == 2){
          lowValue = angle;
        }else if(encNum == 1 || encNum == 3){
          highValue = angle;
        }

        if(lowValue > 0 && highValue > 0){
          bool magFail = 0;
            if(lowValue > highValue){
              up = false;
                if(lowValue < 172){
                  //MAG Encoder Angle Fail
                  magFail = 1;
                }
            }else{
              up = true;
                if(lowValue > 188){
                  //MAG Encoder Angle Fail
                  magFail = 1;
                }
            }
            lcd.clear();
              if(magFail){
                lcd.setCursor(0,1);
                lcd.print(magFail1);
                lcd.setCursor(0,2);
                lcd.print(magFail2);
                  while(magFail){
                    digitalWrite(AB_LED, !digitalRead(AB_LED));
                    delay(250);
                  }                  
              }else{
                lcd.setCursor(0,0);
                lcd.print(ch0);
                lcd.setCursor(0,1);
                lcd.print(mainLabels[Change]);
                lcd.setCursor(1,2);
                lcd.print("THR ANGLE:");
                lcd.setCursor(1,3);
                lcd.print("THR VALUE:");
                runOnce = false;
              }
        }    
      }else{
        if(menu == Menu4){
          lcdTask = 4;
        }else if(menu == Menu3){
          lcdTask = 3;
        }else if(menu == Menu2){
          lcdTask = 2;
        }else if(menu == Menu1){
          lcdTask = 1;
        }
      }
      singlePress = false;
    }

    if(updateVals){
      lcd.setCursor(13,2);
      lcd.print(angle);
      lcd.print("  ");
      lcd.setCursor(13,3);
      lcd.print(throttle);
      lcd.print("  ");
      updateVals = 0;
    }
    
    if(refreshDisplay){
      lcd.setCursor(0,0);
      lcd.print(blank);
      lcd.setCursor(0,0);
      lcd.print(configLabels[Main]);
      lcd.setCursor(0,1);
      lcd.print(blank);
      lcd.setCursor(0,1);
      lcd.print(mainLabels[menu]);
      lcd.setCursor(0,2);
      lcd.print(blank);
      lcd.setCursor(0,2);
      lcd.print(" THR ANGLE:  ");
      lcd.setCursor(13,2);
      lcd.print(angle);
      lcd.setCursor(0,3);
      lcd.print(blank);
      lcd.setCursor(0,3);
      lcd.print(" THR VALUE:  ");
      lcd.setCursor(13,3);
      lcd.print(throttle);
      lcd.print("  ");
      refreshDisplay = 0;
    }
    delay(1);
  }
}
