# ESP32 IoT Soil Moisture Monitor

ESP32-based IoT project for monitoring soil moisture, temperature, and humidity with real-time data display on an LCD and RGB LED indication. The project sends sensor data to ThingsBoard cloud using MQTT protocol for remote monitoring.

## Features

- Measures soil moisture with analog sensor.
- Reads temperature and humidity with DHT11 sensor.
- Displays data and soil condition on 16x2 LCD (I2C).
- RGB LED shows soil moisture status: Red (Dry), Blue (Medium), Green (Wet).
- Connects to WiFi and publishes data to ThingsBoard via MQTT.
- Real-time cloud monitoring and logging.

## Hardware Requirements

- ESP32 development board
- Analog Soil Moisture Sensor
- DHT11 temperature and humidity sensor
- 16x2 I2C LCD display
- RGB LED with current limiting resistors
- Jumper wires and breadboard

## Wiring

| Component         | ESP32 Pin       |
|-------------------|-----------------|
| Soil Moisture (SIG) | GPIO 34       |
| DHT11 Data          | GPIO 4        |
| LCD SDA             | GPIO 21       |
| LCD SCL             | GPIO 22       |
| RGB Red             | GPIO 25       |
| RGB Green           | GPIO 26       |
| RGB Blue            | GPIO 27       |

## Setup Instructions

1. Clone the repository.
2. Install Arduino libraries:
   - `DHT sensor library` by Adafruit
   - `LiquidCrystal_I2C`
   - `ArduinoJson`
   - `PubSubClient`
3. Update your WiFi SSID and password in the code.
4. Update ThingsBoard MQTT token in the code.
5. Upload the code to ESP32 using Arduino IDE.

## Usage

- After powering the ESP32, it connects to the WiFi and ThingsBoard server.
- Sensor data is read every 2 seconds.
- Data is published to ThingsBoard MQTT topic for remote monitoring.
- LCD displays soil moisture, condition, temperature, and humidity.
- RGB LED color represents soil moisture status.

## MQTT Topic

- Publish topic: `v1/devices/me/telemetry`

## Customization

- Adjust `SOIL_DRY_THRESHOLD` and `SOIL_MEDIUM_THRESHOLD` constants to calibrate soil moisture condition levels.
- Change I2C address for LCD if needed.

## Troubleshooting

- Verify WiFi credentials and network availability.
- Check MQTT token and server settings.
- Confirm sensor wiring and functionality.
- Serial monitor provides debug info.

## Author

ARPIT GANGWAR  


