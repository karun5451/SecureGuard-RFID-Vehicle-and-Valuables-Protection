#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <SPI.h>
#include <MFRC522.h>

#include "board.h"
#include "config.h"


Adafruit_MPU6050 mpu;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setupBUZZER(){
    pinMode(alarmPin, OUTPUT);
    digitalWrite(alarmPin, LOW);
}

void setupRFID(){
  SPI.begin();
  mfrc522.PCD_Init();
}

void setupMPU6050() {
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  // Setup MPU6050 based on configurations
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(motionDetectionThreshold);
  mpu.setMotionDetectionDuration(motionDetectionDuration);
  mpu.setInterruptPinLatch(interruptPinLatch);
  mpu.setInterruptPinPolarity(interruptPinPolarity);
  mpu.setMotionInterrupt(motionInterrupt);
}
