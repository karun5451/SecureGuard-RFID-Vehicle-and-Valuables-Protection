#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

#define REGISTERED_CARDS_EEPROM_ADDRESS 200 // Define the EEPROM address for registered cards

// Define the maximum number of registered cards
const int MAX_REGISTERED_CARDS = 10; // Adjust as needed

#define UID_LENGTH 8
#define MAX_USER_NAME_LENGTH 50
#define MAX_REGISTERED_CARDS 4 // {you , one who came for you, one who came by you, one who will be for you}
// Struct to store registered card data
struct RegisteredCard {
  char cardID[UID_LENGTH];
  char userName[MAX_USER_NAME_LENGTH];
  
  // Function to read registered card data from EEPROM
  void readFromEEPROM(int index) {
    int addr = REGISTERED_CARDS_EEPROM_ADDRESS + index * sizeof(RegisteredCard);
    EEPROM.get(addr, *this);
  }
  
  // Function to write registered card data to EEPROM
  void writeToEEPROM(int index) {
    int addr = REGISTERED_CARDS_EEPROM_ADDRESS + index * sizeof(RegisteredCard);
    EEPROM.put(addr, *this);
    EEPROM.commit();
  }
};

extern int numRegisteredCards;

extern RegisteredCard registeredCards[MAX_REGISTERED_CARDS];

extern bool requested_to_register;


// Configuration variables and constants
extern unsigned long startTime;

extern int rfidDetectionCount;
extern unsigned long lastRfidDetectionTime;
extern const unsigned long rfidDetectionTimeout;

extern const unsigned long authorizedCardPresenceInterval;
extern const unsigned long unauthorizedCardPresenceInterval;

extern bool isCardPresent;
extern unsigned long cardPresentStartTime;

extern bool isLockingSequence;

// MPU6050 configuration settings
extern int motionDetectionThreshold;
extern int motionDetectionDuration;
extern bool interruptPinLatch;
extern bool interruptPinPolarity;
extern bool motionInterrupt;

// motion EVENT trigger configurations
extern int accelThreshold;
extern int vibrationThreshold;
extern int gyroRotationThreshold;
extern float temperatureThreshold;

extern float temperatureC;

extern int locked;

extern unsigned long lastAlarmTime;
extern const unsigned long alarmInterval;

extern const int EEPROM_SIZE;

extern const char* defaultApSSID;
extern const char* defaultApPassword;
extern const char* defaultCustomDeviceName;
extern char storedApSSID[32];
extern char storedApPassword[64];
extern char storedCustomDeviceName[32];

void setupEEPROM();
bool loadRegisteredCardsFromEEPROM();
bool isCardRegistered(const char* cardID);
bool addRegisteredCard(const char* cardID, const char* userName);

#endif // CONFIG_H
