# Flask Web Server for ESP8266 Data Reception

A lightweight Flask web server designed to receive HTTP POST data from an ESP8266 microcontroller and display it on a browser-based dashboard. The server stores the most recently received data in memory and renders it on the dashboard page on every request.

This project is deployable on any cloud platform that supports Python and Gunicorn, such as Render, Railway, or Heroku.

---

## Features

- Accepts incoming JSON data from an ESP8266 via HTTP POST on the `/data` endpoint
- Stores the latest received data in memory and serves it through a web dashboard
- Browser-accessible dashboard rendered using a Jinja2 HTML template
- Production-ready with Gunicorn as the WSGI server
- Minimal dependencies — runs entirely on Python and Flask

---

## Project Structure

```
web-server-testing-for-esp8266/
|
|-- main.py                  # Flask application — routes and logic
|-- requirements.txt         # Python dependencies
|-- Procfile                 # Process command for cloud deployment
`-- templates/
    `-- dashboard.html       # HTML dashboard rendered by Flask
```

---

## API Endpoints

| Method | Endpoint | Description |
|--------|----------|-------------|
| POST | `/data` | Receive JSON payload from ESP8266 |
| GET | `/` | Render the dashboard with the latest received data |

### POST /data

Accepts a JSON body and updates the in-memory store with the received key-value pairs.

**Example request from ESP8266:**

```json
{
  "temperature": 28.5,
  "humidity": 65.2,
  "soil_moisture": 42
}
```

**Response:**

```json
{
  "success": true
}
```

---

## Software Requirements

- Python 3.8 or higher
- pip

### Dependencies

| Package | Purpose |
|---------|---------|
| Flask | Web framework |
| Gunicorn | WSGI server for production |
| APScheduler | Optional background task scheduling |
| Requests | HTTP client utilities |
| Bootstrap-Flask | Bootstrap integration for templates |

---

## Local Setup Instructions

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/InternetOfThings.git
cd InternetOfThings/web-server-testing-for-esp8266
```

### 2. Create and Activate a Virtual Environment

```bash
python -m venv venv

# Windows
venv\Scripts\activate

# macOS / Linux
source venv/bin/activate
```

### 3. Install Dependencies

```bash
pip install -r requirements.txt
```

### 4. Run the Server Locally

```bash
python main.py
```

The server starts at `http://0.0.0.0:10000`. Open `http://localhost:10000` in a browser to view the dashboard.

---

## Cloud Deployment

The `Procfile` is configured for platforms like Render or Heroku:

```
web: gunicorn main:app
```

### Deploying on Render (example)

1. Push the project to a GitHub repository.
2. Create a new **Web Service** on [Render](https://render.com).
3. Set the build command to `pip install -r requirements.txt`.
4. Set the start command to `gunicorn main:app`.
5. After deployment, note the public URL.

---

## ESP8266 Integration

On the ESP8266 side, send a POST request to `http://<your-server-url>/data` with a JSON body. Example using Arduino's `ESP8266HTTPClient`:

```cpp
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* serverUrl = "http://<your-server-url>/data";

StaticJsonDocument<200> doc;
doc["temperature"] = 28.5;
doc["humidity"]    = 65.2;

String payload;
serializeJson(doc, payload);

HTTPClient http;
http.begin(serverUrl);
http.addHeader("Content-Type", "application/json");
http.POST(payload);
http.end();
```

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| Dashboard shows no data | No POST request has been received yet; send data from the ESP8266 first |
| POST returns 404 | Confirm the ESP8266 is posting to `/data` and not the root URL |
| Server not reachable from ESP8266 | Ensure the server URL is publicly accessible; check firewall and port settings |
| `gunicorn` not found on cloud | Confirm `gunicorn` is listed in `requirements.txt` |
| Template not rendering | Ensure `dashboard.html` is inside a folder named `templates/` |

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
