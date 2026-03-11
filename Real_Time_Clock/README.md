# Real-Time Clock using DS3231 with ESP32

## Overview
This project demonstrates how to interface the DS3231 Real-Time Clock module with an ESP32 board over I2C.

## Hardware Requirements
- ESP32 Development Board
- DS3231 RTC Module
- Jumper wires

### Wiring
| DS3231 Pin | ESP32 Pin |
|------------|------------|
| SDA        | GPIO 21    |
| SCL        | GPIO 22    |
| VCC        | 3.3V       |
| GND        | GND        |

## Installation
1. Download the repository:
    ```bash
    https://github.com/MakeWithArpit/Real_Time_Clock.git
    ```
2. Open the project in [Arduino IDE / PlatformIO / ESP-IDF].

3. Install required libraries:
    - For Arduino: `RTClib` or any DS3231 library

4. Upload the code to your ESP32.

## Usage
Open the serial monitor at 115200 baud rate to see the current time from DS3231.
