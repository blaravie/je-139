// By default Core 1 runs the bootloader so we use this core for the 
// loops Non-Blocking tasks 

void loop1(void *pvParameters){    // Core 1 loop - User tasks
  while (1){

    bool btnState = digitalRead(EN_BTN);
    if(btnState != lastBtn){
      pressed = 1;
      lastPeriod = millis();
    }
    
    swState = digitalRead(SW);
    if(swState != swLastState){
      update = true;
      lastPeriod = millis();
    }
    
    if(millis() - lastPeriod >= PERIOD){
      lcdCount++; lcdRefresh++; chaseCount++;

      angle = (float(as5600.rawAngle() * AS5600_RAW_TO_DEGREES)+0.5);
      
      if(up){
        throttle = map(angle, lowValue, highValue, 0, 100);
      }else{
        throttle = map(angle, highValue, lowValue, 0, 100);
      }
      
      if(pressed){
        if(btnState == LOW){
          singlePress = 1;
        }
        pressed = 0;
      }
      if(lcdCount >= 10 && !runOnce){
        updateVals = 1;
        lcdCount = 0;
      }
      if(lcdRefresh >= REFRESH_LCD && !runOnce){
        lcdRefresh = 0;
        refreshDisplay = 1;
      }
      if(larson){
        if(chaseCount >= chaseSpeed){
          chaseCount = 0;
          larsonIndex++;
          larsonIndex %= fxCount;
            for(byte i = 0; i < ledCount; i++){
              digitalWrite(leds[i], bitRead(larsonEFX[larsonIndex], i));
            }
        }
      }
      if(update){
          if(swState == 1){
            larson = 0;
            for(byte i = 0; i < ledCount; i++){
              digitalWrite(leds[i], LOW);
            }         
          }else{
            larson = 1;
            larsonIndex = 0;
            chaseCount = 0;
          }        
          update = 0;
      }
      lastPeriod = millis();
    }
    swLastState = swState;
    lastBtn = btnState;
  }
}
