# IoT AQI Monitoring System for Indoor Farming

## Overview

This project is an IoT-based Air Quality Index (AQI) Monitoring System designed for indoor farming. It uses an ESP32 microcontroller with sensors to measure air quality, temperature, and humidity and sends data to ThingsBoard Cloud via MQTT. The system also displays readings on an OLED screen and uses RGB LEDs as status indicators.

## Features

- Measures Temperature, Humidity, TVOC, and eCO2
- Sends telemetry data to ThingsBoard Cloud over MQTT
- Local OLED display for real-time monitoring
- RGB LED indicators for different air quality levels

## Hardware

- ESP32 Development Board  
- DHT11 Sensor (Temperature & Humidity)  
- Adafruit SGP30 Sensor (TVOC & eCO2)  
- Adafruit SSD1306 OLED Display (128x32)  
- RGB LEDs (Red, Green, Blue)  
- Jumper wires and breadboard  

## Software Requirements

- Arduino IDE (v1.8 or higher)  
- Libraries:  
  - WiFi  
  - PubSubClient  
  - Wire  
  - Adafruit_SGP30  
  - Adafruit_SSD1306  
  - DHT sensor library  

Refer to `libraries.txt` for exact library versions.

## Wiring Diagram

See the `images/Schematic_diagram.png` file for the complete hardware connection layout.

## Setup Instructions

1. Connect the hardware as per the wiring diagram.  
2. Open the `IndoorAQI.ino` sketch in Arduino IDE.  
3. Update the WiFi SSID, password, MQTT server address, and ThingsBoard device token in the code.  
4. Install the required libraries.  
5. Upload the sketch to your ESP32 board.  
6. Power the device; it will connect to WiFi and ThingsBoard automatically.

## Usage Example

Once running, the device will:  
- Read sensor values every 2 seconds.  
- Display temperature, humidity, TVOC, and eCO2 on the OLED screen.  
- RGB LEDs change color based on air quality levels (green, blue, red).  
- Send JSON telemetry data to ThingsBoard MQTT topic `v1/devices/me/telemetry`.  

Example of sent JSON payload:  
