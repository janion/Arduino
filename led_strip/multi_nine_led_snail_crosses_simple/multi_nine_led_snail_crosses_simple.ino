/*
 * light_ws2812 example
 * Fade LEDs in R, G, B order; demonstrate functions for changing color order.
 *
 * Created: September 6, 2014
 * Author: Windell Oskay (www.evilmadscientist.com)
 */

#include <WS2812.h>

#define outputPin 7  // Digital output pin (default: 7)
#define LEDCount 120   // Number of LEDs to drive (default: 9)


WS2812 LED(LEDCount); 
cRGB value;

byte intensity = 25;
int SNAILS = 12;
int ledsToLight[] = {0, 5, 25, 35, 40, 50,
                     60, 65, 85, 95, 100, 110};
int speeds[] = {1, -2, 2, -1, 1, -2,
                1, -2, 2, -1, 1, -2};
int colours[][3] = {{intensity, 0, 0},
                    {intensity, 0, 0},
                    {0, intensity, 0},
                    {0, intensity, 0},
                    {0, 0, intensity},
                    {0, 0, intensity},
                    {intensity, 0, 0},
                    {intensity, 0, 0},
                    {0, intensity, 0},
                    {0, intensity, 0},
                    {0, 0, intensity},
                    {0, 0, intensity}
                    };
float snailShape[] = {0.2, 0.4, 0.6, 0.8, 1, 0.8, 0.6, 0.4, 0.2};

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
  displaySnails2();

  delay(10); // Wait (ms)
}

void moveSnails() {
  for (int i = 0; i < SNAILS; i++) {
    ledsToLight[i] = (ledsToLight[i] + speeds[i] + LEDCount) % LEDCount;
  }
}

void displaySnails2() {
  for (int i = 0; i < LEDCount; i++) {
    value.r = 0;
    value.g = 0;
    value.b = 0;
    LED.set_crgb_at(i, value);
  }
  
  for (int i = 0; i < SNAILS; i++) {
    for (int led = 0; led < 9; led++) {
      int rounded = getRoundedIndex(led + ledsToLight[i]);
      value = LED.get_crgb_at(rounded);
      value.r = value.r + (int) (colours[i][0] * snailShape[led]);
      value.g = value.g + (int) (colours[i][1] * snailShape[led]);
      value.b = value.b + (int) (colours[i][2] * snailShape[led]);

      if (value.r > intensity) {
        value.r = intensity;
      }
      if (value.g > intensity) {
        value.g = intensity;
      }
      if (value.b > intensity) {
        value.b = intensity;
      }
      LED.set_crgb_at(rounded, value);
    }
  }
  LED.sync(); // Sends the data to the LEDs
}

int getRoundedIndex(int value) {
  return (value + LEDCount) % LEDCount;
}

