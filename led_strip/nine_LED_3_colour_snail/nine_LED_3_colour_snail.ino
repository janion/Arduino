/*
 * light_ws2812 example
 * Fade LEDs in R, G, B order; demonstrate functions for changing color order.
 *
 * Created: September 6, 2014
 * Author: Windell Oskay (www.evilmadscientist.com)
 */

#include <WS2812.h>

#define outputPin 7  // Digital output pin (default: 7)
#define LEDCount 60   // Number of LEDs to drive (default: 9)


WS2812 LED(LEDCount); 
cRGB value;

byte intensity;
byte sign;
int ledToLight = 0;

void setup() {
    LED.setOutput(outputPin); // Digital Pin 7

  /* You may uncomment one of the following three lines to switch 
  to a different data transmission sequence for your addressable LEDs.
  (These functions can be used at any point in your program as needed.)   */

  LED.setColorOrderRGB();  // Uncomment for RGB color order
  //LED.setColorOrderBRG();  // Uncomment for BRG color order
  //LED.setColorOrderGRB();  // Uncomment for GRB color order (Default; will be used if none other is defined.)

  intensity = 25;
}

void loop() {
  byte i = 0;

  while (i < LEDCount){
    if (abs((i + (3 * LEDCount)) - (ledToLight % (3 * LEDCount))) < 5) { // Green entry
      value.r = (int) (intensity * exp(-abs((i + (3 * LEDCount)) - (ledToLight % (3 * LEDCount)))  / 1.5));
      value.g = 0; 
      value.b = 0; 
      
    } else if (abs(i - (ledToLight % (3 * LEDCount))) < 5) { // Green standard
      value.r = (int) (intensity * exp(-abs(i - (ledToLight % (3 * LEDCount)))  / 1.5));
      value.g = 0; 
      value.b = 0; 
      
    } else if (abs(i - ((ledToLight % (3 * LEDCount)) - LEDCount)) < 5) { // Red standard
      value.r = 0;
      value.g = (int) (intensity * exp(-abs(i - ((ledToLight % (3 * LEDCount)) - LEDCount))  / 1.5)); 
      value.b = 0; 
      
    } else if (abs(i - ((ledToLight % (3 * LEDCount)) - (2 * LEDCount))) < 5) { // Blue standard
      value.r = 0;
      value.g = 0; 
      value.b = (int) (intensity * exp(-abs(i - ((ledToLight % (3 * LEDCount)) - (2 * LEDCount)))  / 1.5)); 
      
    } else if (abs((i - LEDCount) - (ledToLight % LEDCount)) < 5) { // Blue exit
      value.r = 0;
      value.g = 0; 
      value.b = (int) (intensity * exp(-abs((i - LEDCount) - (ledToLight % LEDCount))  / 1.5)); 
      
    } else {
      value.r = 0;
      value.g = 0;
      value.b = 0;
    }
    
    LED.set_crgb_at(i, value); // Set value at LED found at index 0
    i++;
  }
  ledToLight = (ledToLight + 1) % (3 * LEDCount);

  LED.sync(); // Sends the data to the LEDs
  delay(60); // Wait (ms)
}
