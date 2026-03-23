# ESP32 IoT Soil Moisture Monitor

An ESP32-based IoT system for monitoring soil moisture, temperature, and humidity in real time. Sensor readings are displayed on a 16x2 I2C LCD, indicated visually via RGB LEDs, and published to ThingsBoard Cloud over MQTT for remote monitoring and logging.

---

## Features

- Reads soil moisture percentage from an analog soil moisture sensor
- Reads temperature and humidity from a DHT11 sensor
- Displays soil condition, temperature, and humidity on a 16x2 I2C LCD
- RGB LED indicates soil moisture status — Red (Dry), Blue (Medium), Green (Wet)
- Publishes all sensor data to ThingsBoard via MQTT over WiFi
- Serial monitor output for debugging and local observation

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Development Board |
| Soil Moisture Sensor | Analog type (resistive/capacitive) |
| Temperature & Humidity Sensor | DHT11 |
| Display | 16x2 I2C LCD (address `0x27`) |
| Indicators | RGB LED with current-limiting resistors |
| Miscellaneous | Jumper wires, breadboard |

---

## Wiring

| Component | ESP32 Pin |
|-----------|-----------|
| Soil Moisture Signal (SIG) | GPIO 34 |
| DHT11 Data | GPIO 4 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| RGB Red | GPIO 25 |
| RGB Green | GPIO 26 |
| RGB Blue | GPIO 27 |

Refer to `Images/Schematic.png` for the complete circuit diagram.

---

## Software Requirements

- Arduino IDE 1.8 or higher
- ESP32 board package installed in Arduino IDE

### Required Libraries

| Library | Purpose |
|---------|---------|
| DHT sensor library (Adafruit) | DHT11 temperature and humidity readings |
| LiquidCrystal_I2C | I2C LCD communication |
| ArduinoJson | JSON serialization for MQTT payload |
| PubSubClient | MQTT client for ThingsBoard |
| Wire | I2C communication |

---

## Setup Instructions

1. Wire all components as per the wiring table.
2. Open `code.ino` in Arduino IDE.
3. Update the following values in the code:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* token    = "YOUR_THINGSBOARD_DEVICE_TOKEN";
```

4. Optionally adjust calibration thresholds for your soil sensor:

```cpp
#define SOIL_DRY_THRESHOLD    3000
#define SOIL_MEDIUM_THRESHOLD 1500
```

5. Install all required libraries via **Sketch > Include Library > Manage Libraries**.
6. Select board **ESP32 Dev Module** and the correct COM port under **Tools**.
7. Upload the code to the ESP32.

---

## Usage

After powering the board:

- The ESP32 connects to WiFi and then to the ThingsBoard MQTT server.
- Sensor readings are taken every 2 seconds.
- The LCD alternates between displaying soil moisture with condition, and temperature with humidity.
- RGB LED reflects moisture condition:

| LED Color | Soil Condition | Raw ADC Value |
|-----------|---------------|---------------|
| Red | Dry | > 3000 |
| Blue | Medium | 1500 – 3000 |
| Green | Wet | < 1500 |

- Data is published to ThingsBoard on topic `v1/devices/me/telemetry`.

---

## MQTT Payload Example

```json
{
  "soil_moisture": 2100,
  "soil_percent": 49,
  "temperature": 27,
  "humidity": 62
}
```

---

## ThingsBoard Dashboard

Log in to [ThingsBoard](https://demo.thingsboard.io) and create a dashboard to visualize soil moisture trends, temperature, and humidity over time.

Refer to `Images/Dashboard.PNG` for a sample dashboard layout.

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| LCD not displaying | Verify I2C address (default `0x27`); check SDA/SCL connections |
| DHT11 reads NaN | Confirm sensor is connected to GPIO 4 and is properly powered |
| Soil sensor reads constant 0 or 4095 | Sensor may not be inserted correctly; check analog pin connection |
| MQTT connection fails | Verify ThingsBoard token, server, and WiFi credentials |
| WiFi not connecting | Ensure 2.4 GHz network; double-check SSID and password |

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
