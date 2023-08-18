# SecureGuard RFID Protection

SecureGuard RFID Protection is a versatile and customizable security system designed to protect vehicles and valuables using RFID technology, Arduino, MPU6050, and MFRC522. This repository provides the complete source code and instructions to build a robust security system that can be tailored for various modes of operation, such as safeguarding vehicles or protecting valuable items.

## Features

- RFID-based locking and unlocking mechanism
- Advanced sensor-based event detection (acceleration, vibration, gyro rotation, temperature)
- Configurable mode of operation (vehicle or valuable items)
- Real-time event logging with timestamp
- Web-based control panel for easy configuration and monitoring
- Support for BLE-based locking/unlocking in the Pro version

## Components

- Nodemcu
- MPU6050 Accelerometer and Gyro Sensor
- MFRC522 RFID Reader
- WiFi Module (ESP8266)
- Buzzer (for alarms)
- Light Sensor (optional, for ambient light detection)

## Modules and Functionality

The SecureGuard RFID Protection project consists of the following modules and functionality:

- `Alerts`: Provides functions to trigger various types of alerts, such as unauthorized card detection and locking/unlocking sounds.

- `ServerFunctions`: Implements the web server functionality for the control panel, event logging, and configuration settings.

- `Board`: Contains functions for initializing and managing the hardware components of the board.

- `Config`: Handles configuration settings, including mode of operation and threshold values for event detection.

- `Main`: The main application file that orchestrates the overall system functionality.

- Interfacing with MPU6050 for accelerometer and gyro sensor data.
- Battery status read via analog pin A0.
- Interfacing of the MFRC522 RFID reader via SPI.
- Support for MIFARE 1k cards and tags.

## Getting Started

Follow the detailed [Setup Guide](/setup-guide.md) to set up and configure the SecureGuard RFID Protection system. The guide provides step-by-step instructions, circuit diagrams, and tips for customizing the system according to your needs.

## Usage

Once the system is set up, you can easily control and monitor your SecureGuard device using the web-based control panel. Access the control panel by connecting to the device's WiFi network and visiting the provided IP address.

## TODO List

- [ ] Implement battery status monitoring.
- [ ] Enhance event logging with more details.
- [ ] Integrate BLE-based locking/unlocking in Pro version.
- [ ] Add support for LDR ambient light detection.
- [ ] Further optimize event detection algorithms.
- [ ] Enhance user interface for the control panel.
- [ ] Expand documentation and examples.

## Building on PlatformIO

To build and upload the SecureGuard RFID Protection project using PlatformIO:

1. Clone this repository to your local machine.
2. Open the project in PlatformIO IDE.
3. Compile and upload the project to your hardware.

Refer to the [PlatformIO documentation](https://docs.platformio.org/en/latest/) for more details on using PlatformIO.

## Contributors

- [Karun Nunavath](https://github.com/karun5451)

## License

This project is licensed under the MIT License - see the [LICENSE](/LICENSE) file for details.

---

**SecureGuard RFID Protection** - Ultimate security for vehicles and valuables.
