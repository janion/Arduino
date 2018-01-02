# LED Clock

Required hardware:
- Arduino (Nano used but any ATtiny328 will work)
- 2N2222  transistor ofr IR LED (with 470ohm resistor on base)
- DS3231 RTC (Real Time Clock)
- HT16K33 4 digit seven segment display
- 3x momentary push buttons (with 10Kohm resistor each)
- Toggle switch
- LED alarm incator
- Piezo buzzer

Uses libraries:
- [RCTLib](https://github.com/adafruit/RTClib)
- [TimeAlarms](https://github.com/PaulStoffregen/TimeAlarms) (& [TimeLib](https://github.com/PaulStoffregen/Time))
- [IRLib2](https://github.com/cyborg5/IRLib2) (Edited to use TIMER1 instead of TIMER2 as the tone function used TIMER2)
- [Adafruit_LEDBackpack](https://github.com/adafruit/Adafruit_LED_Backpack) (& [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library))

To do:
- Refactor into alarm clock class with a 7 segment diaply and buttons to configure
  - Change to use rotary encoder and LCD screen
