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

byte intensity = 25;
byte sign;
int SNAILS = 6;
int ledsToLight[] = {0, 10, 20, 30, 40, 50};
int speeds[] = {1, -2, 2, -1, 1, -2};
int colours[][3] = {{intensity, 0, 0},
                    {intensity, 0, 0},
                    {0, intensity, 0},
                    {0, intensity, 0},
                    {0, 0, intensity},
                    {0, 0, intensity}
                    };

void setup() {
    LED.setOutput(outputPin); // Digital Pin 7

  /* You may uncomment one of the following three lines to switch 
  to a different data transmission sequence for your addressable LEDs.
  (These functions can be used at any point in your program as needed.)   */

  LED.setColorOrderRGB();  // Uncomment for RGB color order
  //LED.setColorOrderBRG();  // Uncomment for BRG color order
  //LED.setColorOrderGRB();  // Uncomment for GRB color order (Default; will be used if none other is defined.)
}

void loop() {
  moveSnails();
  displaySnails();

  delay(40); // Wait (ms)
}

void moveSnails() {
  for (int i = 0; i < SNAILS; i++) {
    ledsToLight[i] = (ledsToLight[i] + speeds[i] + LEDCount) % LEDCount;
  }
}

void displaySnails() {
  for (int i = 0; i < LEDCount; i++) {
    value.r = 0;
    value.g = 0;
    value.b = 0;
    LED.set_crgb_at(i, value);
  }
  
  for (int i = 0; i < SNAILS; i++) {
    value = LED.get_crgb_at(ledsToLight[i]);
    value.r = (value.r + colours[i][0]) % (intensity + 1);
    value.g = (value.g + colours[i][1]) % (intensity + 1);
    value.b = (value.b + colours[i][2]) % (intensity + 1);
    LED.set_crgb_at(ledsToLight[i], value);
    
//    value.r = colours[i][0];
//    value.g = colours[i][1];
//    value.b = colours[i][2];
//    LED.set_crgb_at(ledsToLight[i], value);
  }
  LED.sync(); // Sends the data to the LEDs
}

