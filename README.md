# Internet of Things — Project Collection

This repository is a collection of IoT projects built using ESP32 and ESP8266 microcontrollers. Each project covers a distinct concept in embedded systems, wireless communication, and cloud-based data monitoring. All projects are written in C++ using the Arduino framework, with supporting server-side code in Python where applicable.

---

## Repository Structure

```
InternetOfThings-main/
|
|-- AQI-System-for-Indoor-Farming/       # Air quality monitoring with MQTT and ThingsBoard
|-- ESP32-IoT-SoilMonitor/               # Soil moisture monitoring with cloud logging
|-- HTTP_Connection_For_ESP32/           # ESP32 to Google Sheets via HTTP GET and POST
|-- MQTT_Conection_For_ESP32/            # Basic MQTT connectivity test with ThingsBoard
|-- Real_Time_Clock/                     # DS3231 RTC module interfacing with ESP32
|-- Current_Sensor_ACS712_30A/           # AC current measurement using ACS712 sensor
`-- web-server-testing-for-esp8266/      # Flask web server for receiving ESP8266 HTTP data
```

---

## Projects Overview

### 1. AQI System for Indoor Farming

An IoT-based Air Quality Index monitoring system designed for indoor farming environments. The ESP32 reads temperature, humidity, TVOC, and eCO2 values from onboard sensors, displays them on an OLED screen, and transmits telemetry data to ThingsBoard cloud over MQTT. RGB LEDs provide a local visual indication of air quality levels.

**Hardware:** ESP32, DHT11, Adafruit SGP30, SSD1306 OLED, RGB LED  
**Communication:** MQTT over WiFi to ThingsBoard  
**Refer to:** `AQI-System-for-Indoor-Farming/README.md`

---

### 2. ESP32 IoT Soil Moisture Monitor

A real-time soil condition monitoring system that reads soil moisture, temperature, and humidity, and displays the data on a 16x2 I2C LCD. RGB LEDs indicate moisture status (Dry, Medium, Wet), and all readings are published to ThingsBoard via MQTT for remote monitoring.

**Hardware:** ESP32, Analog Soil Moisture Sensor, DHT11, 16x2 I2C LCD, RGB LED  
**Communication:** MQTT over WiFi to ThingsBoard  
**Refer to:** `ESP32-IoT-SoilMonitor/README.md`

---

### 3. HTTP Connection for ESP32 (Google Sheets Integration)

Demonstrates bidirectional HTTP communication between an ESP32 and Google Sheets using Google Apps Script as a web app backend. Organized into three sub-folders — GET only, POST only, and a combined GET+POST sketch — this project enables cloud-based data logging and retrieval without any third-party platforms.

**Hardware:** ESP32  
**Communication:** HTTP over WiFi to Google Apps Script  
**Refer to:** `HTTP_Connection_For_ESP32/README.md`

---

### 4. MQTT Connection for ESP32

A minimal working example that demonstrates how to establish a reliable MQTT connection from an ESP32 to the ThingsBoard cloud platform. The sketch connects to WiFi, authenticates with ThingsBoard, and publishes a simulated temperature value as JSON telemetry every two seconds.

**Hardware:** ESP32  
**Communication:** MQTT over WiFi to ThingsBoard  
**Refer to:** `MQTT_Conection_For_ESP32/README.md`

---

### 5. Real-Time Clock with DS3231

Demonstrates how to interface the DS3231 RTC module with an ESP32 over the I2C bus. The current date and time are read from the RTC and printed to the serial monitor every second. Includes wiring details and library setup instructions.

**Hardware:** ESP32, DS3231 RTC Module  
**Communication:** I2C (no network required)  
**Refer to:** `Real_Time_Clock/README.md`

---

### 6. Current Sensor — ACS712 (30A)

Demonstrates how to measure AC current using two ACS712 30A sensors connected to an ESP32. The sensors are automatically calibrated on startup and polled every second. Measured current values are printed to the serial monitor.

**Hardware:** ESP32, ACS712 30A Current Sensor (x2)  
**Communication:** Serial monitor only  
**Refer to:** `Current_Sensor_ACS712_30A/README.md`

---

### 7. Web Server for ESP8266 (Flask)

A lightweight Flask web server designed to receive HTTP POST data from an ESP8266 microcontroller and display it on a simple browser-based dashboard. The server is deployable on any cloud platform that supports Python (Render, Railway, Heroku, etc.).

**Stack:** Python, Flask, Gunicorn  
**Communication:** HTTP POST from ESP8266  
**Refer to:** `web-server-testing-for-esp8266/README.md`

---

## Common Requirements

| Requirement | Details |
|-------------|---------|
| Microcontroller | ESP32 or ESP8266 |
| IDE | Arduino IDE 1.8 or higher |
| Language | C++ (Arduino framework), Python 3.x |
| Cloud Platform | ThingsBoard (MQTT projects), Google Sheets (HTTP project) |

---

## Getting Started

1. Clone the repository:

```bash
git clone https://github.com/yourusername/InternetOfThings.git
cd InternetOfThings
```

2. Navigate to the project folder you want to run.
3. Open the `.ino` file in Arduino IDE.
4. Install the required libraries listed in each project's README.
5. Update WiFi credentials, MQTT tokens, or server URLs as needed.
6. Upload the code to your board.

---

## License

All projects in this repository are open-source and available under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
