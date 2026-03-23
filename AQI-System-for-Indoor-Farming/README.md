# AQI Monitoring System for Indoor Farming

An IoT-based Air Quality Index (AQI) monitoring system designed for indoor farming environments. The system uses an ESP32 microcontroller with multiple sensors to measure temperature, humidity, TVOC (Total Volatile Organic Compounds), and eCO2 (equivalent CO2). Data is displayed locally on an OLED screen, indicated visually via RGB LEDs, and transmitted to ThingsBoard Cloud over MQTT for remote monitoring.

---

## Features

- Measures temperature and humidity using the DHT11 sensor
- Measures TVOC and eCO2 levels using the Adafruit SGP30 sensor
- Real-time data display on a 128x32 SSD1306 OLED screen
- RGB LED color indicators for different air quality levels (Good, Moderate, Poor)
- Publishes telemetry data to ThingsBoard Cloud over MQTT
- Automatic WiFi and MQTT reconnection on disconnect

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Development Board |
| Temperature & Humidity Sensor | DHT11 |
| Air Quality Sensor | Adafruit SGP30 (TVOC & eCO2) |
| Display | Adafruit SSD1306 OLED 128x32 (I2C) |
| Indicators | RGB LED (Red, Green, Blue) |
| Miscellaneous | Jumper wires, breadboard |

---

## Wiring Diagram

Refer to `images/Schematic.png` for the complete hardware connection layout.

| Component | ESP32 Pin |
|-----------|-----------|
| DHT11 Data | GPIO 5 |
| SGP30 SDA | GPIO 21 |
| SGP30 SCL | GPIO 22 |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| RGB Red | GPIO 13 |
| RGB Blue | GPIO 12 |
| RGB Green | GPIO 11 |

---

## Software Requirements

- Arduino IDE 1.8 or higher
- ESP32 board package installed in Arduino IDE

### Required Libraries

| Library | Purpose |
|---------|---------|
| WiFi | WiFi connectivity |
| PubSubClient | MQTT communication |
| Wire | I2C communication |
| Adafruit SGP30 | SGP30 sensor driver |
| Adafruit SSD1306 | OLED display driver |
| DHT sensor library | DHT11 sensor driver |

Refer to `libraries.txt` for exact library versions used during development.

---

## Setup Instructions

1. Connect the hardware as per the wiring table and schematic diagram.
2. Open `IndoorAQI.ino` in Arduino IDE.
3. Update the following constants in the code:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* pass     = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "demo.thingsboard.io";
const char* token    = "YOUR_THINGSBOARD_DEVICE_TOKEN";
```

4. Install all required libraries via **Sketch > Include Library > Manage Libraries**.
5. Select the correct board (**ESP32 Dev Module**) and COM port under **Tools**.
6. Upload the sketch to the ESP32.

---

## Usage

Once powered and connected:

- Sensor values are read every 2 seconds.
- The OLED screen cycles through Temperature, Humidity, TVOC, and eCO2 readings.
- RGB LED changes color based on air quality:

| LED Color | Air Quality Level | eCO2 Range (ppm) | TVOC Range (ppb) |
|-----------|------------------|------------------|------------------|
| Green | Good | 400 – 1000 | 0 – 250 |
| Blue | Moderate | 1001 – 1500 | 251 – 650 |
| Red | Poor | 1501 – 2100 | 651 – 5000 |

- JSON telemetry is published to ThingsBoard on topic `v1/devices/me/telemetry`:

```json
{
  "temperature": 28.5,
  "humidity": 65.0,
  "TVOC": 120,
  "ECO2": 850
}
```

---

## ThingsBoard Dashboard

Log in to [ThingsBoard](https://demo.thingsboard.io) and configure a dashboard to visualize the incoming telemetry data using charts, gauges, or value cards.

Refer to `images/Dashboard.PNG` for a sample dashboard layout.

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| OLED not displaying | Check I2C address (default `0x3C`); verify SDA/SCL connections |
| DHT sensor shows NaN | Ensure DHT11 is connected to the correct GPIO and is properly powered |
| SGP30 readings always zero | Allow 15–30 seconds for sensor warm-up after power-on |
| MQTT connection fails | Verify the ThingsBoard token, server address, and WiFi credentials |
| WiFi not connecting | Confirm SSID and password; ensure 2.4 GHz network is used |

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
