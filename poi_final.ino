#include <WS2812.h>
#include <SD.h>

#define LED_PIN 2
#define SC_PIN 4

#define IMAGE_HEIGHT 16

WS2812 LED(IMAGE_HEIGHT); 
cRGB value;
long start;
long before = 0;
File file;

void setup() {
//  Serial.begin(9600);
  LED.setOutput(LED_PIN);
  LED.setColorOrderGRB();

  pinMode(SC_PIN, OUTPUT);
  
  if (!SD.begin(SC_PIN)) {
//    Serial.println("start card failed");
    return;
  }

  start = millis();
//  Serial.println(start);
}

void loop() {
  int fileNum = 1;
  while(true) {
    file = SD.open(String(fileNum) + ".bin");
    if (!file) {
      // no more files
      break;
    }

    String tymString = "";
    for (int i = 0; i < 5; i++) {
      tymString += (char) file.read();
    }
    long tym = tymString.toFloat() * 1000;
//    Serial.println(String(fileNum) + ".dat : " + tymString + " : " + String(tym));

    while(millis() < start + before + tym) {
      file.seek(5);
      while (file.available()) {
        for (int y = 0; y < IMAGE_HEIGHT; y++) {
          value.r = (byte) file.read();
          value.g = (byte) file.read();
          value.b = (byte) file.read();
          LED.set_crgb_at(y, value);
        }
        LED.sync();
      }
    }
    file.close();
    before += tym;
    fileNum++;
  }
}

