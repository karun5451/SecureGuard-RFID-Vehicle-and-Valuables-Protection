#include <Arduino.h>
#include "board.h"
#include "config.h"


void triggerAlarm() {
  tone(alarmPin, 3000, 500); // Higher-pitched alarm
  delay(500);
  tone(alarmPin, 2000, 500); // Lower-pitched alarm
  delay(500);
  noTone(alarmPin); // Turn off the buzzer
}

void playLockSound() {
  tone(alarmPin, 1200, 200); // Lock sound
  delay(200);
  noTone(alarmPin);
}

void playUnlockSound() {
  tone(alarmPin, 1800, 200); // Unlock sound
  delay(200);
  noTone(alarmPin);
}

void triggerAuthorizedBeep() {
  tone(alarmPin, 2000, 100);
  delay(100);
  noTone(alarmPin);
}

void triggerUnauthorizedSiren() {
  tone(alarmPin, 800, 100); // Siren-like sound
  delay(300);
  noTone(alarmPin);
}

void playRegistrationSound() {
  tone(alarmPin, 2000, 300); // Play a registration sound
  delay(300);
  noTone(alarmPin);
}

void playErrorSound() {
  tone(alarmPin, 500, 200); // Play an error sound
  delay(300);
  noTone(alarmPin);
}


