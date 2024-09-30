//BT Connection as a Navigation Beacon pulsing flash

void doNavBT(void){
  beaconTimer++;
  if(beaconTimer >= BEACON_TIMER){      //Primary 40mS counter
    beaconTimer = 0;
    beaconCount++;      
      if(beaconEN){
          beaconState += BEACON_STEP;         //Fading beacon
              if(beaconCount > BEACON_RAMP){
                  beaconState = PWM_MAX;
                  beaconCount = 0;
                  beaconEN = false;
                  goto bt_end;
              }
      }else{                                  //fast double blink when fade complete
          if(beaconCount > 40){
            beaconCount = 0;
            beaconEN = true;
          }else if(beaconCount > 10){
            beaconState = PWM_MIN;
          }else if(beaconCount > 8){
            beaconState = PWM_MAX;
          }else if(beaconCount > 6){
            beaconState = PWM_MIN;
          }else if(beaconCount > 4){
            beaconState = PWM_MAX;
          }else if(beaconCount > 2){
            beaconState = PWM_MIN;
          }else if(beaconCount > 0){
            beaconState = PWM_MAX;
          }
      }
  }
  bt_end:
  analogWrite(BT_LED, beaconState);
  return;
}
