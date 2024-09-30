
/* Callback to set motor running state */
bool startStop(uint8_t currentPWM, uint8_t lastPWM){
  bool val;
    if(currentPWM >= 10 && lastPWM < 10){
      val = 1;
    }else if(currentPWM < 10 && lastPWM >= 10){
      val = 0;
    }
    return val;
}

/* BOOST and Glow Igniter Sequence */
void boostAndGlow(void){
  if(millis() - lastBoostPeriod >= boostStepDelay){
    switch (boostSeq){

      case 2:
        for(int i = 0; i < BOOST_COUNT; i++){
          boost.setPixelColor(i, abColor);
          boost.show();
          delay(1);
        }
        runAB = 0;
      break;

      case 1:
        flicker = !flicker;
        rgbVal = random(0, 6);
        abColor = colours[rgbVal];
          if(flicker){
            boost.fill(abColor);
            boostStepDelay = random(3, 9) * 5;
          }else{
            boost.clear();
            boostStepDelay = random(6, 18) * 10;
          }
          boost.show();
            if(millis() - startMillis >= 1500UL){
              boostSeq = 2;
              abColor = RED75;
              boostStepDelay = 10UL;
            }
      break;

      case 0:
        glowPWM += 10;
          if(glowPWM >= PWM_MAX){
            glowPWM = PWM_MAX;
            boostSeq = 1;
            boostStepDelay = 0;
            startMillis = millis();
          }
          analogWrite(GLOW, glowPWM);
      break;

    }
    lastBoostPeriod = millis();
  }
  return;  
}

/*  Nozzle Servo sequence for Open\Close 
    Sweep direction can be reversed via serial command 'c' || 'C' (Change)  */
void nozzleMove(void){
  nozzleStepCount++;
    if(nozzleStepCount >= SWEEP_PERIOD){
      nozzleStepCount = 0;
        if(nozzleTo == servoClosed){
          if(up){
            servpPos += SERVO_STEP;
              if(servpPos >= servoClosed){
                moveNozzle = 0;
              }
          }else{
            servpPos -= SERVO_STEP;
              if(servpPos <= servoClosed){
                moveNozzle = 0;
              }
          }
        }else{
          if(up){
            servpPos -= SERVO_STEP;
              if(servpPos <= servoOpen){
                moveNozzle = 0;
              }
          }else{
            servpPos += SERVO_STEP;            
              if(servpPos >= servoOpen){
                moveNozzle = 0;
              }
          }
        }
    }
    servo.write(servpPos);
    return;
}
