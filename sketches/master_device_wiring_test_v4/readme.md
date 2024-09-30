## Items to note:  

<br>
 
1. There is a sketch **`AS5600_with_mp3.ino`** which I suggest you run to get used to how to configure the mag encoder.
2. You may need an external power supply as USB power may not be enough.
3. You don't need a serial connection for this sketch as long as you know that the magnetic encoder is working.
4. Make sure to set AS5600 direction correctly in the scope. (Lines 10 to 14 of master_device_wiring_test_v4.ino tab)
5. Changed to a better LCD library **`<hd44780.h> `**. You must config the type of I2C chip configuration your unit employs. In my case it's the `backpack` external `PCF8574` module. [Read more here](https://github.com/duinoWitchery/hd44780#hw-support)
##

<br>

## Sketch Functions & Actions

<br>

1. You are first presented with a welcome and press button to continue
2. You then will be instructed to capture a throttle value. Read the screen to know where to move the throttle to and then press button. Turn the knob for the next value capture.
3. Once two values are captured **`Off and Full Throttle`** numbers are checked
4. If the throttle angle range fails sequential number test based on a **`170°`** swing the loop is halted and an error is displayed. RED LED will blink. 
5. On boot success you can Turn On the start switch and the 5 LED's will do a up/down chase.
6. When the LED chase is running you can turn the knob to select a different LED chase speed (50mS, 250mS, 500mS, 1Sec). To action selected speed press the button.
7. The other information displayed is the throttle position as an angle and as a percentage.
##
