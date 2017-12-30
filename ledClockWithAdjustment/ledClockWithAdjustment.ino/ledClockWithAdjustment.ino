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
  SET_TIME_HOUR,
  SET_TIME_MINUTE,
  SET_ALARM_HOUR,
  SET_ALARM_MINUTE
};

boolean stateChanged = false;

const int MODE_PIN = 2;
const int SNOOZE_PIN = 3;
const int TIME_ADJUSTMENT_PIN = 4;
const int BUZZER = 5;

// 3 minutes
const int secondsBetweenIncrements = 60 * 3;

// Beep constants
const double beepDuration = 100; // ms
const double beepGap = 100; // ms
const double beepSeparation = 1; // s
const int alarmRepeats = 10; //90; // ~s
const int beepFrequency = 2000; // Hz

// Alarm time
unsigned int alarmHour = 0;
unsigned int alarmMinute = 0;
unsigned int alarmSecond = 0;

boolean isSnoozPressed = false;

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
  pinMode(TIME_ADJUSTMENT_PIN, INPUT);
  pinMode(SNOOZE_PIN, INPUT);

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
  
  setAlarmTime();

  // Mode change button
  attachInterrupt(digitalPinToInterrupt(MODE_PIN), changeMode, RISING);
  // Snooze Button
  attachInterrupt(digitalPinToInterrupt(SNOOZE_PIN), snooze, RISING);
}

void loop() {
  if (stateChanged) {
    stateChanged = false;
    updateState();
  }
  
  if (state == SHOW_TIME) {
    showTimePoll();
  } else if (state == SET_TIME_HOUR || state == SET_TIME_MINUTE) {
    setTimePoll();
  } else if (state == SET_ALARM_HOUR || state == SET_ALARM_MINUTE) {
    setAlarmPoll();
  }
  Alarm.delay(1);
}

void showTimePoll() {
  static boolean colon;

  if (second() != seconds) {
    hours = hour();
    minutes = minute();
    seconds = second();
    
    showTime(hours, minutes, colon);
    colon = !colon;
  }
}

void setTimePoll() {
  static long lastFlashMillis = millis();
  static boolean isOn = true;
  boolean timeButtonDown = digitalRead(TIME_ADJUSTMENT_PIN);
  while(digitalRead(TIME_ADJUSTMENT_PIN) == HIGH);

  if (timeButtonDown) {
    if (state == SET_TIME_HOUR) {
      hours = (++hours) % 24;
    } else {
      minutes = (++minutes) % 60;
    }
    showTime(alarmHour, alarmMinute, true);
    lastFlashMillis = millis();
  }

  // Set time on RTC
  rtc.adjust(DateTime(year(), month(), day(), hours, minutes, second()));
  setTime(hours, minutes, second(), day(), month(), year());

  if (abs(millis() - lastFlashMillis) > 500) {
    if (isOn) {
      if (state == SET_TIME_HOUR) {
        showTime(-1, minutes, false);
      } else {
        showTime(hours, -1, false);
      }
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
  boolean timeButtonDown = digitalRead(TIME_ADJUSTMENT_PIN);
  while(digitalRead(TIME_ADJUSTMENT_PIN) == HIGH);

  if (timeButtonDown) {
    if (state == SET_ALARM_HOUR) {
      alarmHour = (++alarmHour) % 24;
      setAlarmTime();
    } else {
      alarmMinute = (++alarmMinute) % 60;
      setAlarmTime();
    }
    showTime(alarmHour, alarmMinute, true);
    lastFlashMillis = millis();
  }

  if (abs(millis() - lastFlashMillis) > 500) {
    if (isOn) {
      if (state == SET_ALARM_HOUR) {
        showTime(-1, alarmMinute, true);
      } else {
        showTime(alarmHour, -1, true);
      }
      lastFlashMillis = millis();
      isOn = false;
    } else {
      showTime(alarmHour, alarmMinute, true);
      lastFlashMillis = millis();
      isOn = true;
    }
  }
}

void setAlarmTime() {
  int hoursForAlarm = alarmHour;
  int minutesForAlarm = alarmMinute - 30;

  if (minutesForAlarm < 0) {
    minutesForAlarm += 60;
    hoursForAlarm--;
  }

  if (hoursForAlarm < 0) {
    hoursForAlarm += 24;
  }
  
  Alarm.free(alarmId);
  alarmId = Alarm.alarmRepeat(hoursForAlarm, minutesForAlarm, alarmSecond, turnOnRed);
}

void updateState() {
  if (state == SHOW_TIME) {
    state = SET_TIME_HOUR;
  } else if (state == SET_TIME_HOUR) {
    state = SET_TIME_MINUTE;
  } else if (state == SET_TIME_MINUTE) {
    state = SET_ALARM_HOUR;
  } else if (state == SET_ALARM_HOUR) {
    state = SET_ALARM_MINUTE;
  } else if (state == SET_ALARM_MINUTE) {
    EEPROM.update(0, alarmHour);
    EEPROM.update(1, alarmMinute);
    state = SHOW_TIME;
  }
}

void showTime(int hoursToShow, int minutesToShow, boolean colonToShow) {
  matrix.clear();

  if (hoursToShow > -1) {
    matrix.writeDigitNum(0, hoursToShow / 10);
    matrix.writeDigitNum(1, hoursToShow % 10);
  }
  if (minutesToShow > -1) {
    matrix.writeDigitNum(3, minutesToShow / 10);
    matrix.writeDigitNum(4, minutesToShow % 10);
  }
  matrix.drawColon(colonToShow);
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
    Alarm.timerOnce(secondsBetweenIncrements, beep);
  }
}

void beep() {
  static int count = 0;
  if (count == 0) {
    isSnoozPressed = false;
  }
  if (count++ < alarmRepeats && !isSnoozPressed) {
    Alarm.timerOnce(beepSeparation, beep); 
    // Turn on
    tone(BUZZER, beepFrequency, beepDuration);
    Alarm.delay(beepDuration);
    Alarm.delay(beepGap);
    tone(BUZZER, beepFrequency, beepDuration);
  } else {
    count = 0;
    isSnoozPressed = false;
  }
}

void snooze() {
  isSnoozPressed = true;
}
