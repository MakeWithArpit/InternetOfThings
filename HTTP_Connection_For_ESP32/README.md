# HTTP Connection — ESP32 to Google Sheets

This project demonstrates how to establish bidirectional HTTP communication between an ESP32 microcontroller and a Google Spreadsheet using Google Apps Script as a web app backend. No third-party IoT platforms are required — data flows directly between the ESP32 and Google's infrastructure over WiFi.

---

## Repository Structure

```
HTTP_Connection_For_ESP32/
|
|-- HTTP_GET/
|   |-- ESP32_CODE/ESP32_CODE.ino    # Fetches data from Google Sheets
|   `-- Script.txt                   # Google Apps Script for GET handling
|
|-- HTTP_POST/
|   |-- ESP32_CODE/ESP32_CODE.ino    # Sends data to Google Sheets
|   `-- Script.txt                   # Google Apps Script for POST handling
|
`-- COMBINED/
    |-- ESP32_CODE/ESP32_CODE.ino    # Handles both GET and POST in one sketch
    `-- Script.txt                   # Google Apps Script handling both methods
```

---

## Features

- **GET Function** — Reads values from a Google Spreadsheet and stores them in ESP32 variables. Useful for reading configuration values or cloud-set control parameters.
- **POST Function** — Sends sensor readings or device status from the ESP32 to a Google Spreadsheet. Useful for real-time data logging.
- **Combined GET + POST** — Implements both upload and fetch operations in a single sketch, enabling full two-way communication with Google Sheets.
- Uses the ESP32's built-in `HTTPClient` library — no external platforms needed.
- Fully customizable and open-source.

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Development Board |
| Connectivity | 2.4 GHz WiFi network |

---

## Software Requirements

- Arduino IDE 1.8 or higher
- ESP32 board package installed in Arduino IDE

### Required Libraries

| Library | Purpose |
|---------|---------|
| WiFi | WiFi connectivity (built-in) |
| HTTPClient | HTTP GET and POST requests (built-in) |
| ArduinoJson | Parsing and serializing JSON payloads |

---

## Setup Instructions

### Step 1 — Google Apps Script Setup

1. Open [Google Sheets](https://sheets.google.com) and create a new spreadsheet.
2. Go to **Extensions > Apps Script**.
3. Paste the contents of the appropriate `Script.txt` file (GET, POST, or Combined).
4. Click **Deploy > New Deployment**.
5. Select type: **Web App**.
6. Set **Execute as:** Me, and **Who has access:** Anyone.
7. Click **Deploy** and copy the generated web app URL.

### Step 2 — ESP32 Code Configuration

1. Open the appropriate `.ino` file in Arduino IDE.
2. Update the following fields:

```cpp
const char* ssid       = "YOUR_WIFI_SSID";
const char* password   = "YOUR_WIFI_PASSWORD";
const char* serverName = "YOUR_GOOGLE_APPS_SCRIPT_WEB_APP_URL";
```

3. Select board **ESP32 Dev Module** and the correct COM port under **Tools**.
4. Upload the sketch to the ESP32.

---

## Usage

### GET Sketch

The ESP32 sends an HTTP GET request to the Apps Script URL. The script responds with a JSON object that the ESP32 parses and prints to the serial monitor.

**Example response from Google Sheets:**

```json
{
  "temperature": "28.5",
  "humidity": "65",
  "status": "Normal"
}
```

**Serial output:**

```
Received JSON: {"temperature":"28.5","humidity":"65","status":"Normal"}
Temp: 28.5
Humidity: 65
Status: Normal
```

---

### POST Sketch

The ESP32 serializes a JSON payload and sends it via HTTP POST to the Apps Script URL. The script appends the received values as a new row in the spreadsheet.

**Example payload sent to Google Sheets:**

```json
{
  "data1": 123.45,
  "data2": 67.89,
  "data3": "Normal"
}
```

**Serial output:**

```
POST Success: 200
```

---

### Combined Sketch

Performs a GET request first to fetch data from the sheet, then performs a POST to log new values — both within the same loop iteration. Requests repeat every 5 seconds.

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| GET returns -1 or timeout | Ensure the Apps Script is deployed as a web app with public access; enable redirect following via `setFollowRedirects` |
| POST returns error code | Verify the Content-Type header is set to `application/json`; confirm the web app URL is correct |
| WiFi not connecting | Check SSID, password, and ensure the ESP32 is within range of a 2.4 GHz network |
| JSON parsing fails | Confirm the Apps Script response format matches what the ESP32 expects; check for extra whitespace or HTML error pages |
| Apps Script URL not working | Re-deploy the script after any code changes; the URL changes with each deployment in some configurations |

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
