#include "Arduino.h"

const int led1 = 12;
const int led2 = 12;
const int led3 = 12;

const int detector1 = A5;
const int detector2 = A4;
const int detector3 = A3;

const int coil1 = 8;
const int coil2 = 8;
const int coil3 = 8;

const int trigger = 5;

const int threshold = 300;

void setup() {
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);
	pinMode(detector1, INPUT);
	pinMode(detector2, INPUT);
	pinMode(detector3, INPUT);
	pinMode(coil1, OUTPUT);
	pinMode(coil2, OUTPUT);
	pinMode(coil3, OUTPUT);
	pinMode(trigger, INPUT);
}

void loop() {
	while (digitalRead(trigger) == LOW);

	// Turn on light gate LEDs
	digitalWrite(led1, HIGH);
	digitalWrite(led2, HIGH);
	digitalWrite(led2, HIGH);

	/////////////////////////////////////////////////////////////////

	// Fire coil
	digitalWrite(coil1, HIGH);
	// Wait for projectile to leave first light gate
	while (analogRead(detector1) > threshold);
	// Turn off coil
	digitalWrite(coil1, LOW);

	/////////////////////////////////////////////////////////////////

	// Wait for projectile to enter second light gate
	while (analogRead(detector2) < threshold);
	// Turn on coil
	digitalWrite(coil2, HIGH);

	// Wait for projectile to leave second light gate
	while (analogRead(detector2) > threshold);
	// Turn off coil
	digitalWrite(coil2, LOW);

	/////////////////////////////////////////////////////////////////

	// Wait for projectile to enter third light gate
	while (analogRead(detector3) < threshold);
	// Turn on coil
	digitalWrite(coil3, HIGH);

	// Wait for projectile to leave third light gate
	while (analogRead(detector3) > threshold);
	// Turn off coil
	digitalWrite(coil3, LOW);

	/////////////////////////////////////////////////////////////////

	// Turn off light gate LEDs
	digitalWrite(led1, LOW);
	digitalWrite(led2, LOW);
	digitalWrite(led3, LOW);

	// Make sure trigger has been released properly
	while (digitalRead(trigger) == HIGH);
	delay(1000);
}
