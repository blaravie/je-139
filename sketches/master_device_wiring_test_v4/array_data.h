//Text for the LCD
char  ch0[21] PROGMEM  = "JET CONTROLLOR TEST ",
      ch1[21] PROGMEM  = "DEVICE & WIRING TEST",
      ch2[21] PROGMEM  = "    PRESS BUTTON    ",
      ch3[21] PROGMEM  = "    TO CONTINUE     ",
      ch4[21] PROGMEM  = "      TURN DIAL     ",
      ch5[21] PROGMEM  = "  PRESS BTN TO SET  ",
      ch6[21] PROGMEM  = "THROTTLE TO STOP POS",
      ch7[21] PROGMEM  = "THROTTLE TO FULL POS",
      ch8[21] PROGMEM  = " LED CHASE SPEED 1  ",
      ch9[21] PROGMEM  = " LED CHASE SPEED 2  ",
      ch10[21] PROGMEM = " LED CHASE SPEED 3  ",
      ch11[21] PROGMEM = " LED CHASE SPEED 4  ",
      ch12[21] PROGMEM = " TURN KNOB TO UPDATE",
     blank[21] PROGMEM = "                    ",
  magFail1[21] PROGMEM = "  MAG ENCODER FAIL  ",
  magFail2[21] PROGMEM = " DISORDERED NUMBERS ";

const char *const configLabels[] PROGMEM {ch0, ch1, ch2, ch3, ch4, ch5, ch6, ch7};
const char *const mainLabels[] PROGMEM {ch8, ch9, ch10, ch11, ch12};
enum configNames{Main, Wiring, Press, Continue, Turn, Set, StopPos, FullPos};
enum mainNames{Menu1, Menu2, Menu3, Menu4, Change};

//Arays to create a chase effect with the 5 LEDS
uint8_t   leds[]{BT_LED, ON_LED, READY_LED, AB_LED, LM_LED},
          larsonEFX[]{0x0, 0x0, 0x0, 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x1E, 0x1C, 0x18, 0x10, 0x0, 
                      0x0, 0x0, 0x0, 0x10, 0x18, 0x1C, 0x1E, 0x1F, 0xF, 0x7, 0x3, 0x1};

