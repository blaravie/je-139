//Learing Neopixel strip Blue to Red one way chase effect
void learnWipe(void){
  learnCount++;
    if(learnCount >= 200){
      learnStep++;
        if(learnStep >= 15){
          learnStep = 0;
            if(learnState){
              learn.setPixelColor(learnPixel, learn.Color(redLearn, 0, blueLearn));
              learn.show();
              learnPixel++;
              blueLearn -= (float)PWM_MAX/LEARN_COUNT*1.5 + 0.5;
              redLearn += (float)PWM_MAX/LEARN_COUNT*1.5 + 0.5;
              if(blueLearn < 0) blueLearn = 0;
              if(redLearn > 255) redLearn = 255;
                if(learnPixel == LEARN_COUNT){
                  blueLearn = PWM_MIN;
                  redLearn = PWM_MIN;
                  learnCount = 0;
                  learnState = 0;
                  learnPixel = 0;
                  return;
                }
            }else{
            learn.setPixelColor(learnPixel, learn.Color(0, 0, 0));
            learn.show();
            learnPixel++;
              if(learnPixel == LEARN_COUNT){
                blueLearn = PWM_MAX;
                redLearn = PWM_MIN;
                learnCount = 0;
                learnState = 1;
                learnPixel = 0;
              }
            }
        }
    }
  return;
}

