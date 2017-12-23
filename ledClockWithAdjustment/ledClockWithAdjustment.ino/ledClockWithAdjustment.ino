#include <TimeLib.h>
#include <TimeAlarms.h>
// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
// IR transmitter
#include <IRLibAll.h>
// 7 segment display
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
// EEPROM
#include <EEPROM.h>

enum State {
  SHOW_TIME,
  SET_TIME,
  SET_ALARM
};

boolean stateChanged = false;

const int MODE_PIN = 2;
const int MINUTE_ADJUSTMENT_PIN = 5;
const int HOUR_ADJUSTMENT_PIN = 4;

// 3 minutes
const int secondsBetweenIncrements = 60 * 3;

// Alarm time
unsigned int alarmHour = 0;
unsigned int alarmMinute = 0;
unsigned int alarmSecond = 0;

// Current time
unsigned int hours;
unsigned int minutes;
unsigned int seconds;

RTC_DS3231 rtc;
IRsend irTransmitter;
Adafruit_7segment matrix = Adafruit_7segment();

AlarmId alarmId;
State state = SHOW_TIME;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(HOUR_ADJUSTMENT_PIN, INPUT);
  pinMode(MINUTE_ADJUSTMENT_PIN, INPUT);

  matrix.begin(0x70);
  matrix.setBrightness(0);

  if (! rtc.begin()) {
    while (1);
  }

  if (rtc.lostPower()) {
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    return;
  }
  
  // Set time on time library
  DateTime now = rtc.now();
  setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());

  alarmHour = EEPROM.read(0);
  alarmMinute = EEPROM.read(1);

  // Alarm
  alarmId = Alarm.alarmRepeat(alarmHour, alarmMinute, alarmSecond, turnOnRed);

  // Mode change Button
  attachInterrupt(digitalPinToInterrupt(MODE_PIN), changeMode, RISING);
}

void loop() {
  if (stateChanged) {
    stateChanged = false;
    updateState();
  }
  
  if (state == SHOW_TIME) {
    showTimePoll();
  } else if (state == SET_TIME) {
    setTimePoll();
  } else if (state == SET_ALARM) {
    setAlarmPoll();
  }
  Alarm.delay(1);
}

void showTimePoll() {
  static boolean colon;
  DateTime now = rtc.now();

  if (now.second() != seconds) {
    hours = now.hour();
    minutes = now.minute();
    seconds = now.second();
    
    showTime(hours, minutes, colon);
    colon = !colon;
  }
}

void setTimePoll() {
  static long lastFlashMillis = millis();
  static boolean isOn = true;
  boolean hourButtonDown = digitalRead(HOUR_ADJUSTMENT_PIN);
  boolean minuteButtonDown = digitalRead(MINUTE_ADJUSTMENT_PIN);
  while(digitalRead(HOUR_ADJUSTMENT_PIN) == HIGH || digitalRead(MINUTE_ADJUSTMENT_PIN) == HIGH);

  if (minuteButtonDown) {
    minutes = (++minutes) % 60;
    showTime(hours, minutes);
    lastFlashMillis = millis();
  }

  if (hourButtonDown) {
    hours = (++hours) % 24;
    showTime(hours, minutes);
    lastFlashMillis = millis();
  }
  
  DateTime now = rtc.now();
  // Set time on RTC
//  rtc.adjust(DateTime(now.year(), now.month(), now.day(), hours, minutes, now.second()));

  if (abs(millis() - lastFlashMillis) > 500) {
    if (isOn) {
      matrix.clear();
      matrix.writeDisplay();
      lastFlashMillis = millis();
      isOn = false;
    } else {
      showTime(hours, minutes, true);
      lastFlashMillis = millis();
      isOn = true;
    }
  }
}

void setAlarmPoll() {
  static long lastFlashMillis = millis();
  static boolean isOn = true;
  boolean hourButtonDown = digitalRead(HOUR_ADJUSTMENT_PIN);
  boolean minuteButtonDown = digitalRead(MINUTE_ADJUSTMENT_PIN);
  while(digitalRead(HOUR_ADJUSTMENT_PIN) == HIGH || digitalRead(MINUTE_ADJUSTMENT_PIN) == HIGH);

  if (minuteButtonDown) {
    alarmMinute = (++alarmMinute) % 60;
    showTime(alarmHour, alarmMinute);
    lastFlashMillis = millis();
    // Set alarm
    Alarm.free(alarmId);
    alarmId = Alarm.alarmRepeat(alarmHour, alarmMinute, alarmSecond, turnOnRed);
  }

  if (hourButtonDown) {
    alarmHour = (++alarmHour) % 24;
    showTime(alarmHour, alarmMinute);
    lastFlashMillis = millis();
    // Set alarm
    Alarm.free(alarmId);
    alarmId = Alarm.alarmRepeat(alarmHour, alarmMinute, alarmSecond, turnOnRed);
  }

  if (abs(millis() - lastFlashMillis) > 500) {
    if (isOn) {
      matrix.clear();
      showColon(true);
      lastFlashMillis = millis();
      isOn = false;
    } else {
      showTime(alarmHour, alarmMinute, true);
      lastFlashMillis = millis();
      isOn = true;
    }
  }
}

void updateState() {
  if (state == SHOW_TIME) {
    state = SET_TIME;
  } else if (state == SET_TIME) {
    state = SET_ALARM;
  } else if (state = SET_ALARM) {
    EEPROM.update(0, alarmHour);
    EEPROM.update(1, alarmMinute);
    state = SHOW_TIME;
  }
}

void showTime(int hoursToShow, int minutesToShow, boolean colonToShow) {
    showTime(hoursToShow, minutesToShow);
    showColon(colonToShow);
}

void showColon(boolean colonToShow) {
    matrix.drawColon(colonToShow);
    matrix.writeDisplay();
}

void showTime(int hoursToShow, int minutesToShow) {
    matrix.writeDigitNum(0, hoursToShow / 10);
    matrix.writeDigitNum(1, hoursToShow % 10);
    matrix.writeDigitNum(3, minutesToShow / 10);
    matrix.writeDigitNum(4, minutesToShow % 10);
    matrix.writeDisplay();
}

void changeMode() {
  static long modeChangeMillis = 0;
  if (millis() - modeChangeMillis > 100) {
    stateChanged = true;
    modeChangeMillis = millis();
  }
}

void turnOnRed() {
  Alarm.timerOnce(secondsBetweenIncrements, setOrange); 
  // Turn on
  irTransmitter.send(NEC, 0xff02fd, 0);
  
  // Set red
  irTransmitter.send(NEC, 0xff1ae5, 0);
}

void setOrange() {
  Alarm.timerOnce(secondsBetweenIncrements, setYellow);
  irTransmitter.send(NEC, 0xff2ad5, 0);
}

void setYellow() {
  Alarm.timerOnce(secondsBetweenIncrements, setBrighter);
  irTransmitter.send(NEC, 0xff0af5, 0);
}

void setBrighter() {
  static int brightnessCount = 0;
  if (brightnessCount++ < 7) {
    Alarm.timerOnce(secondsBetweenIncrements, setBrighter);
    irTransmitter.send(NEC, 0xff3ac5, 0);
  } else {
    brightnessCount = 0;
  }
}
