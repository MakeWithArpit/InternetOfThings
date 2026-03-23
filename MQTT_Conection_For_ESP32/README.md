# MQTT Connection for ESP32

A minimal working example that demonstrates how to establish a reliable MQTT connection from an ESP32 to the ThingsBoard Cloud platform. The sketch connects the ESP32 to a WiFi network, authenticates with ThingsBoard using a device token, and continuously publishes a simulated temperature value as JSON telemetry every two seconds.

This project serves as a foundation or connectivity test for any ESP32-based IoT application that uses MQTT.

---

## Features

- Connects ESP32 to a 2.4 GHz WiFi network with retry logic
- Establishes authenticated MQTT connection to ThingsBoard
- Automatically reconnects to MQTT broker if the connection drops
- Publishes JSON telemetry payload to ThingsBoard every 2 seconds
- Serial monitor output for connection status and debugging

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Development Board |
| Connectivity | 2.4 GHz WiFi network |

No additional sensors or peripherals are required. This project uses a simulated (random) temperature value to demonstrate the data publishing flow.

---

## Software Requirements

- Arduino IDE 1.8 or higher
- ESP32 board package installed in Arduino IDE

### Required Libraries

| Library | Purpose |
|---------|---------|
| WiFi | WiFi connectivity (built-in with ESP32 package) |
| PubSubClient | MQTT client for broker communication |
| ArduinoJson | JSON payload serialization |

---

## Setup Instructions

1. Open `MQTT_Cheacking.ino` in Arduino IDE.
2. Update the following constants at the top of the file:

```cpp
const char* ssid       = "YOUR_WIFI_SSID";
const char* password   = "YOUR_WIFI_PASSWORD";
const char* mqttServer = "demo.thingsboard.io";
const int   mqttPort   = 1883;
const char* token      = "YOUR_THINGSBOARD_DEVICE_TOKEN";
```

3. Install the `PubSubClient` and `ArduinoJson` libraries via **Sketch > Include Library > Manage Libraries**.
4. Select board **ESP32 Dev Module** and the correct COM port under **Tools**.
5. Upload the sketch to the ESP32.

---

## How to Get a ThingsBoard Device Token

1. Log in to [ThingsBoard Demo](https://demo.thingsboard.io).
2. Go to **Devices** and create a new device.
3. Open the device and navigate to **Manage Credentials**.
4. Copy the **Access Token** and paste it as the value of `token` in the sketch.

---

## Usage

After uploading, open the Serial Monitor at **115200 baud**. The expected output is:

```
Connecting to WiFi..........
Connected to WiFi.
Connecting to ThingsBoard...Connected to ThingsBoard
Connecting to ThingsBoard...Connected to ThingsBoard
```

The device will publish a JSON telemetry payload every 2 seconds:

```json
{
  "Temperature": 73
}
```

This data will appear in real time on the ThingsBoard dashboard under the device's **Latest Telemetry** tab.

---

## MQTT Details

| Parameter | Value |
|-----------|-------|
| Broker | `demo.thingsboard.io` |
| Port | `1883` |
| Publish Topic | `v1/devices/me/telemetry` |
| Authentication | Device Access Token (as username) |

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| WiFi connection fails after 16 attempts | Check SSID and password; ensure 2.4 GHz network is in range |
| MQTT connection fails with `rc=-2` | Verify ThingsBoard server address and port; check network connectivity |
| MQTT connection fails with `rc=-4` | Device token is incorrect; regenerate from ThingsBoard |
| No telemetry visible in ThingsBoard | Confirm the publish topic is exactly `v1/devices/me/telemetry` |

---

## Extending This Project

To use this as a base for a real sensor project:

1. Replace the `random(0.0, 150.0)` line with actual sensor readings.
2. Add additional key-value pairs to the `ArduinoJson` document for multiple telemetry fields.
3. Adjust the `delay(2000)` value to control the data publishing interval.

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
