## Controller Change Log

<br>

### v1.0.4
  1. Removed the DFPlayer
  2. Removed Analog Out from AS5600 Magnetic Encoder

<br>

### v1.0.3
  1.  DFPlayer changed to +5V Vcc
  2.  DFPlayer now connected via ESP32 UART2 (TX2 / RX2) 
  3.  AS5600 connections changed to just DIR, SDA and SCL
  4.  R1 to R5 now 150R (tentative, further testing needed)


<br>

### v1.0.2
  1. Switched `LED2` and `LED3` positions
  2. Added SW1 `ENGINE START SW`
  3. Added J5 to AS5600 for incremental DIRECTION - Default HIGH = CCW, LOW = CW
  4. Changed AS5600 to 3v3 Vcc
  5. Added Bi-Directional Level Shift as a precaution if 5V LCD used.
   `Suggest production I2C_LCD be 3v3 version`
