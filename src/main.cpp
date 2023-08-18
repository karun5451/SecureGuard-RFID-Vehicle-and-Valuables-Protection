#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "board.h"
#include "config.h"
#include "Alerts.h"
#include "ServerFunctions.h"


ESP8266WebServer server(80);

bool key_registered = false;
bool requested_to_register = false;
float temperatureC = 0.0;



void handleLockedStateEvents(float totalAcceleration, float totalRotation) {
  // Handle events when the device is locked

  // Check for suspicious motion
  if (totalAcceleration > accelThreshold) {
    triggerAlarm();
    addToEventLog("Suspicious Motion Detected");
  }

  // Check for tampering or vibration event
  if (totalRotation > gyroRotationThreshold) {
    triggerAlarm();
    addToEventLog("Device Lifted or Moved Detected");
  }
}

void handleUnlockedStateEvents(const sensors_event_t &a) {
  // Handle events when the device is unlocked

  // Check for collision or sudden brake event
  if (a.acceleration.x < -15.0 || a.acceleration.x > 15.0 ||
      a.acceleration.y < -15.0 || a.acceleration.y > 15.0 ||
      a.acceleration.z < -15.0 || a.acceleration.z > 15.0) {
    triggerAlarm();
    addToEventLog("Collision or Sudden Brake Detected");
  }
}


float calculateTotalAcceleration(const sensors_event_t &a) {
  // Calculate total acceleration from MPU data
  return sqrt(a.acceleration.x * a.acceleration.x +
              a.acceleration.y * a.acceleration.y +
              a.acceleration.z * a.acceleration.z);
}

float calculateTotalRotation(const sensors_event_t &g) {
  // Calculate total rotation from MPU data
  return sqrt(g.gyro.x * g.gyro.x +
              g.gyro.y * g.gyro.y +
              g.gyro.z * g.gyro.z);
}



void setup() {

  setupBUZZER();

  Serial.begin(115200);
  Serial.println(F("Started:: "));
  startTime = millis();  // Record the start time

  setupEEPROM();
  setupMPU6050();
  setupRFID();

  // Set up access point (AP) mode
  WiFi.softAP(storedApSSID, storedApPassword);

  // Set custom hostname for the AP
  WiFi.hostname(storedCustomDeviceName);
  if (MDNS.begin(storedCustomDeviceName)) {
    Serial.println("mDNS responder started");
  }

   // Print Wi-Fi MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
  
  // Get IP address of the AP
  IPAddress apIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(apIP);

  
  // Start the web server
  server.on("/", HTTP_GET, handleHome);
  server.on("/eventlog", HTTP_GET, handleEventLog);
  server.on("/config", HTTP_GET, handleConfig);
  server.on("/saveconfig", HTTP_POST, handleSaveConfig);
  server.on("/togglelock", HTTP_GET, handleToggleLock);
  server.begin();

  key_registered = loadRegisteredCardsFromEEPROM();

  if (!key_registered) {
    Serial.println("No registered key found, please register your key");
  }else{
    Serial.println("Registered Cards:");
    for (int i = 0; i < numRegisteredCards; ++i) {
        Serial.print("Card ");
        Serial.print(i + 1);
        Serial.print(": ID=");
        Serial.print(registeredCards[i].cardID);
        Serial.print(", Name=");
        Serial.println(registeredCards[i].userName);
    }
  }

}


void loop() {
  server.handleClient();
  
  // Check RFID
    if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
            String content = "";
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
                content.concat(String(mfrc522.uid.uidByte[i], HEX));
            }
            content.toUpperCase();

            // Debug log: Print detected card UID
            Serial.print("Found key: ");
            Serial.println(content);

            if (!key_registered || requested_to_register) {
                server.send(200, "text/plain", "New card UID: " + content + "<br>Please verify and confirm.");

                if (requested_to_register && addRegisteredCard(content.c_str(), "New User")) {
                    // Debug log: Card successfully registered
                    Serial.println("Card successfully registered.");
                    playRegistrationSound(); // Play a sound to indicate successful registration
                } else {
                    // Debug log: Cannot register more cards
                    Serial.println("Cannot register more cards, array is full.");
                    playErrorSound(); // Play a sound to indicate registration error
                }

                key_registered = true;
                requested_to_register = false;
            } else {
                unsigned long currentMillis = millis();

                if (!isCardRegistered(content.c_str())) { 
                    if (!isCardPresent) {
                        isCardPresent = true;
                        cardPresentStartTime = currentMillis;
                        // Beep when an authorized card is detected
                        addToEventLog("Authorized Card Detected");
                        triggerAuthorizedBeep();
                    }

                    if (currentMillis - cardPresentStartTime >= authorizedCardPresenceInterval) {
                        // Long time card present, perform lock/unlock based on sequence
                        if (currentMillis - lastRfidDetectionTime <= rfidDetectionTimeout) {
                            // Double detection detected
                            if (isLockingSequence) {
                                toggleLockState();
                                delay(1000);
                                rfidDetectionCount = 0; // Reset detection count
                                isLockingSequence = false; // Reset locking sequence
                            }
                        } else {
                            // Single detection detected
                            rfidDetectionCount++;
                            lastRfidDetectionTime = currentMillis;
                            if (rfidDetectionCount == 1) {
                                isLockingSequence = true; // Begin locking sequence
                                delay(500); // Wait for the next detection in the locking sequence
                            }
                        }
                    }
                } else {
                    isCardPresent = false;
                    unsigned long cardAbsentTime = currentMillis - cardPresentStartTime;
                    if (cardAbsentTime >= unauthorizedCardPresenceInterval) {
                        // Unauthorized card detected after a certain period, trigger siren
                        triggerUnauthorizedSiren();
                        addToEventLog("Unauthorized Card Detected");
                    }
                }
            }
        } else {
            isCardPresent = false;
        }
    }

    if (mpu.getMotionInterruptStatus()) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        // Check for temperature increase
        float temperatureC = temp.temperature;
        if (temperatureC > temperatureThreshold) {
        triggerAlarm();
        addToEventLog("Temperature Increase Detected");
        }
    
        // Calculate total acceleration and rotation
        float totalAcceleration = calculateTotalAcceleration(a);
        float totalRotation = calculateTotalRotation(g);

        // Check motion and other events if locked
        if (locked) {
            handleLockedStateEvents(totalAcceleration, totalRotation);
        } else {
            handleUnlockedStateEvents(a);
        }
    }
    
}
