#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

// Define configuration variables and constants
unsigned long startTime = 0;
int rfidDetectionCount = 0;
unsigned long lastRfidDetectionTime = 0;
const unsigned long rfidDetectionTimeout = 1000;

const unsigned long authorizedCardPresenceInterval = 60000;
const unsigned long unauthorizedCardPresenceInterval = 120000;

bool isCardPresent = false;
unsigned long cardPresentStartTime = 0;

bool isLockingSequence = false;

// Define MPU6050 configuration settings
int motionDetectionThreshold = 1;
int motionDetectionDuration = 20;
bool interruptPinLatch = true;
bool interruptPinPolarity = true;
bool motionInterrupt = true;

int accelThreshold = 100;
int vibrationThreshold = 5;
int gyroRotationThreshold = 5;
float temperatureThreshold = 45.0;

int locked = 0;

unsigned long lastAlarmTime = 0;
const unsigned long alarmInterval = 5000;

const int EEPROM_SIZE = 512;

const char* defaultApSSID = "vata_wifi";
const char* defaultApPassword = "vata@123";
const char* defaultCustomDeviceName = "vata";
char storedApSSID[32];
char storedApPassword[64];
char storedCustomDeviceName[32];

// Initialize the array of RegisteredCard
RegisteredCard registeredCards[MAX_REGISTERED_CARDS];
int numRegisteredCards = 0;

void setupEEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  
  // Load AP configuration from EEPROM with default values
  EEPROM.get(10, storedApSSID);
  if (strlen(storedApSSID) == 0) {
    strcpy(storedApSSID, defaultApSSID);
    EEPROM.put(10, storedApSSID);
  }

  EEPROM.get(50, storedApPassword);
  if (strlen(storedApPassword) == 0) {
    strcpy(storedApPassword, defaultApPassword);
    EEPROM.put(50, storedApPassword);
  }

  EEPROM.get(100, storedCustomDeviceName);
  if (strlen(storedCustomDeviceName) == 0) {
    strcpy(storedCustomDeviceName, defaultCustomDeviceName);
    EEPROM.put(100, storedCustomDeviceName);
  }

  accelThreshold = EEPROM.read(0);
  if (accelThreshold == EEPROM.read(0xFF)) {
    accelThreshold = 100; // Default value
    EEPROM.write(0, accelThreshold);
  }

  vibrationThreshold = EEPROM.read(1);
  if (vibrationThreshold == EEPROM.read(0xFF)) {
    vibrationThreshold = 5; // Default value
    EEPROM.write(1, vibrationThreshold);
  }

  gyroRotationThreshold = EEPROM.read(2);
  if (gyroRotationThreshold == EEPROM.read(0xFF)) {
    gyroRotationThreshold = 5; // Default value
    EEPROM.write(2, gyroRotationThreshold);
  }

  temperatureThreshold = EEPROM.read(3);
  if (temperatureThreshold == EEPROM.read(0xFF)) {
    temperatureThreshold = 45.0; // Default value
    EEPROM.put(3, temperatureThreshold);
  }

  locked = EEPROM.read(4);
  if (locked == EEPROM.read(0xFF)) {
    locked = 0; // Default value
    EEPROM.write(4, locked);
  }

  // Commit the changes to EEPROM
  EEPROM.commit();
}

bool loadRegisteredCardsFromEEPROM() {
    // Load registered card data from EEPROM and populate the registeredCards array
    for (int i = 0; i < MAX_REGISTERED_CARDS; i++) {
        registeredCards[i].readFromEEPROM(i);
    }

    // Check if loading was successful by verifying data in the array
    for (int i = 0; i < MAX_REGISTERED_CARDS; i++) {
        if (strlen(registeredCards[i].cardID) == 0) {
            return false; // Return false if loading is unsuccessful
        }
    }

    return true; // Return true if loading is successful
}



void registerCard(const char* cardID, const char* userName) {
  for (int i = 0; i < MAX_REGISTERED_CARDS; ++i) {
    // Find an empty slot in the array
    if (strlen(registeredCards[i].cardID) == 0) {
      strncpy(registeredCards[i].cardID, cardID, UID_LENGTH - 1);
      strncpy(registeredCards[i].userName, userName, MAX_USER_NAME_LENGTH - 1);
      EEPROM.put(200 + (i * sizeof(RegisteredCard)), registeredCards[i]);
      EEPROM.commit();
      break;
    }
  }
}


bool addRegisteredCard(const char* cardID, const char* userName) {
    if (numRegisteredCards < MAX_REGISTERED_CARDS) {
        RegisteredCard newCard;
        strncpy(newCard.cardID, cardID, UID_LENGTH - 1);
        strncpy(newCard.userName, userName, MAX_USER_NAME_LENGTH - 1);
        newCard.userName[MAX_USER_NAME_LENGTH - 1] = '\0'; // Ensure null-terminated string

        registeredCards[numRegisteredCards] = newCard;
        numRegisteredCards++;

        // Update EEPROM with the new registered card data
        EEPROM.put(REGISTERED_CARDS_EEPROM_ADDRESS + (numRegisteredCards - 1) * sizeof(RegisteredCard), newCard);
        EEPROM.commit();

        return true; // Successfully added the registered card
    } else {
        return false; // Cannot add more registered cards, array is full
    }
}

bool isCardRegistered(const char* cardID) {
  for (int i = 0; i < numRegisteredCards; i++) {
    if (strcmp(registeredCards[i].cardID, cardID) == 0) {
      return true; // Card is registered
    }
  }
  return false; // Card is not registered
}
