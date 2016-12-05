#include "LinkedList.h"

/**
 * Need to work out how to deal with sized arrays.
 * I propose to make an empty array of -1s and take
 * equation results and if they lie within the bounds then assign
 * the rounded result to the respective array element
 */

const int LATCH_PIN = 11;
const int CLOCK_PIN = 10;
const int DATA_PIN = 13;
const int EXTRA_PIN = 12;

const int LAYER0 = 2;
const int LAYER1 = 3;
const int LAYER2 = 4;
const int LAYER3 = 5;
const int LAYER4 = 6;
int LAYERS[] =  {LAYER0, LAYER1, LAYER2, LAYER3, LAYER4 };

const int SIZE = 5;
int coordinates[SIZE][SIZE][SIZE];
byte data[SIZE][int((SIZE * SIZE) / 8)];

const int DELAY_MILLIS = 1;
double startTime;

double all[SIZE] {0.0, 1.0, 2.0, 3.0, 4.0};

int vals[SIZE] = {-1, -1, -1, -1, -1};

void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(EXTRA_PIN, OUTPUT);

  pinMode(LAYER0, OUTPUT);
  pinMode(LAYER1, OUTPUT);
  pinMode(LAYER2, OUTPUT);
  pinMode(LAYER3, OUTPUT);
  pinMode(LAYER4, OUTPUT);
  
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);

  startTime = millis();
}

void loop() {
  // Reset coordinates array
  resetData();

  // Calculate data
  double t = (millis() - startTime) / 1000.0;
  for (int x = 0; x < SIZE; x++) {
    for (int y = 0; y < SIZE; y++) {
      populateData(x, y, t);
    }
  }

  // Display data
  displayData();
}

void resetData() {
  for (int z = 0; z < SIZE; z++) {
    for (int x = 0; x < SIZE; x++) {
      for (int y = 0; y < SIZE; y++) {
        coordinates[x][y][z] = 0;
      }
    }
  }
}

void populateData(int x, int y, double t) {
  calculateValues(x, y, t);
  for (int i = 0; i < SIZE; i++) {
    int rounded = vals[i];
    if (rounded >= 0 && rounded < SIZE) {
      coordinates[x][y][rounded] = 1;
    }
  }
}

void calculateValues(double x, double y, double t) {
  int max = 40;
  if (int(t) % max < 10) {
//    vals[0] = int((2 * sin((4.0 * t) + 0.5 * (x + y))) + 2.499);
    vals[0] = int((2 * sin((6.0 * t) + 0.5 * ((x*sin(0.5*t)) + (y*cos(0.5*t))))) + 2.499);
  } else if (int(t) % max < 20) {
    vals[0] = int((2 * sin(sqrt(((0.75*(x - 2)) * (0.75*(x - 2))) + ((0.75*(y - 2)) * (0.75*(y - 2)))) - (4 * t))) + 2.499);
  } else if (int(t) % max < 30) {
    vals[0] = int(2.499 + 2 * sin(16 * t));
  } else if (int(t) % max < 40) {
    vals[0] = int(-0.45*(1+0.5*sin(10*t))*(x*(x-4) + y*(y-4)));
  }
}

void displayData() {
  for (int z = 0; z < SIZE; z++) {
    int layerPin = LAYERS[z];
    for (int x = 0; x < SIZE; x++) {
      for (int y = 0; y < SIZE; y++) {
        if (coordinates[x][y][z] == 1) {
          digitalWrite(DATA_PIN, HIGH);
        } else {
          digitalWrite(DATA_PIN, LOW);
        }
        digitalWrite(CLOCK_PIN, HIGH);
        digitalWrite(CLOCK_PIN, LOW);
      }
    }

    // Make sure this is only triggered if... (x, y) = (0, 0) ?
    if (coordinates[0][0][z] == 1) {
      digitalWrite(EXTRA_PIN, HIGH);
    }
    digitalWrite(layerPin, HIGH);
    digitalWrite(LATCH_PIN, HIGH);
    delay(DELAY_MILLIS);
    digitalWrite(layerPin, LOW);
    digitalWrite(LATCH_PIN, LOW);
    digitalWrite(EXTRA_PIN, LOW);
  }
}


