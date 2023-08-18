#ifndef BOARD_H
#define BOARD_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN D8  
#define RST_PIN D3 
#define alarmPin D0
#define lightSensor A0

extern Adafruit_MPU6050 mpu;
extern MFRC522 mfrc522;

void setupBUZZER();
void setupEEPROM();
void setupRFID();
void setupMPU6050();

#endif // CONFIG_H
