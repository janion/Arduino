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

int i = 0;
byte intensity = 255;
int ledToLight = 0;

void setup() {
    LED.setOutput(outputPin); // Digital Pin 7

  /* You may uncomment one of the following three lines to switch 
  to a different data transmission sequence for your addressable LEDs.
  (These functions can be used at any point in your program as needed.)   */

  LED.setColorOrderRGB();  // Uncomment for RGB color order
  //LED.setColorOrderBRG();  // Uncomment for BRG color order
  //LED.setColorOrderGRB();  // Uncomment for GRB color order (Default; will be used if none other is defined.)

  for (int x = 0; x < LEDCount; x++) {
    value.r = 0;
    value.g = 0;
    value.b = 0;
    LED.set_crgb_at(x, value);
  }
  LED.sync();
}

void loop() {
  if (i % 3 == 0) {
    value.r = intensity;
    value.g = 0;
    value.b = 0;
  } else if (i % 3 == 1) {
    value.r = 0;
    value.g = intensity;
    value.b = 0;
  } else if (i % 3 == 2) {
    value.r = 0;
    value.g = 0;
    value.b = intensity;
  }
  LED.set_crgb_at(ledToLight, value);
  
  LED.sync(); // Sends the data to the LEDs
  i++;
  delay(1000);
}

