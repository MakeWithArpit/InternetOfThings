# 📡 ESP32 Wi-Fi Provisioning — Captive Portal

A lightweight, production-ready **Wi-Fi provisioning solution** for ESP32-based IoT products. Lets end users connect your device to their home or office network through a beautiful web interface — no app required.

---

## ✨ How It Works

```
Device powers on
       │
       ▼
 Saved Wi-Fi found?
    │         │
   YES        NO
    │         │
    ▼         ▼
 Connect   Start AP "MyDevice-Setup"
 & run     User connects via phone
 app        Browser opens setup page
            User picks Wi-Fi + password
                    │
                    ▼
            Credentials saved to NVS
            Device restarts & connects
```

> The ESP32 first checks its flash memory (NVS) for saved credentials. If none exist, it creates its own Wi-Fi hotspot and serves a setup page. Once the user submits their credentials, the device saves them and connects — permanently.

---

## 🖼️ Preview

| Setup Page | Connected State |
|---|---|
| Dark, minimal UI with network dropdown | Success banner — device restarts automatically |

The page is served entirely from ESP32 flash memory — no internet required, no external CDN, no app.

---

## 🚀 Features

- **Zero dependencies** — Uses only ESP32 built-in libraries
- **Captive Portal** — Browser opens automatically when phone connects to the AP
- **Live Wi-Fi Scan** — Dropdown populated with real nearby networks
- **Manual SSID Entry** — For hidden networks
- **NVS Persistence** — Credentials survive restarts and power cuts
- **Reset Button** — Hold BOOT (GPIO0) for 3 seconds to clear credentials
- **LED Status Indicator** — Blink patterns for connecting / connected / AP mode
- **Wi-Fi Watchdog** — Auto-reconnects if connection drops during runtime
- **Lightweight UI** — ~5 KB page, pure HTML/CSS/JS, no frameworks

---

## 📦 Requirements

### Hardware
- Any **ESP32** board (ESP32-WROOM, ESP32-S3, ESP32-C3, etc.)
- Optional: LED on GPIO 2 (built-in on most dev boards)
- Optional: BOOT button on GPIO 0 (built-in on most dev boards)

### Software
- [Arduino IDE](https://www.arduino.cc/en/software) 1.8+ or 2.x
- **ESP32 Arduino Core** by Espressif ([Installation Guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html))

### Libraries (all built-in with ESP32 core — no manual install needed)
| Library | Purpose |
|---|---|
| `WiFi.h` | Wi-Fi station & AP management |
| `WebServer.h` | HTTP server for the setup page |
| `DNSServer.h` | Redirect all domains → device IP (captive portal) |
| `Preferences.h` | NVS flash storage for credentials |

---

## 🛠️ Installation

**1. Clone the repository**
```bash
git clone https://github.com/yourusername/esp32-wifi-provisioning.git
cd esp32-wifi-provisioning
```

**2. Open in Arduino IDE**

Open `ESP32_WiFi_Provisioning.ino` in Arduino IDE.

**3. Configure for your product**

Edit the `#define` block at the top of the file:
```cpp
#define AP_SSID          "MyDevice-Setup"  // Name users will see when scanning
#define AP_PASSWORD      "12345678"         // AP password (min 8 chars, "" for open)
#define CONNECT_TIMEOUT  15                 // Seconds before giving up on connect
#define RESET_PIN        0                  // GPIO for reset button (BOOT button)
#define RESET_HOLD_MS    3000               // Hold duration to trigger reset
#define STATUS_LED       2                  // Built-in LED GPIO, -1 to disable
```

**4. Upload to your ESP32**

Select your board and port in Arduino IDE, then click Upload.

---

## 📱 First-Time Setup (Client Flow)

This is what your end user does — only once:

1. **Power on** the device
2. On their phone, go to **Settings → Wi-Fi**
3. Connect to **`MyDevice-Setup`** (password: `12345678`)
4. A **setup page opens automatically** in the browser
5. **Select their home Wi-Fi** from the dropdown
6. Enter the **password** and tap **Connect**
7. Device saves credentials, restarts, and connects ✅

> **Note:** On some Android phones, the browser may not open automatically. In that case, manually navigate to `192.168.4.1` in any browser.

---

## 🔁 Resetting Wi-Fi Credentials

If the user changes their router or needs to reconfigure:

- **Hold the BOOT button (GPIO0) for 3 seconds**
- The LED blinks rapidly, credentials are cleared
- Device restarts and enters provisioning mode again

You can also trigger this programmatically:
```cpp
clearCredentials();
ESP.restart();
```

---

## 🧩 Integrating Your Application Code

After provisioning, add your product logic in two places:

**In `setup()` — runs once after connecting:**
```cpp
if (connectToSavedWiFi()) {
    // ✅ Wi-Fi connected — start your app
    mqttConnect();
    initSensors();
    startOTA();
}
```

**In `loop()` — runs continuously:**
```cpp
void loop() {
    checkResetButton();  // Always keep this

    if (WiFi.getMode() == WIFI_AP) {
        dnsServer.processNextRequest();
        server.handleClient();
        return;  // Don't run app code while in provisioning mode
    }

    // ─── Your application code below ───
    readSensor();
    publishData();
    delay(1000);
}
```

---

## 📡 API Endpoints

The web server exposes three routes during provisioning mode:

| Method | Endpoint | Description |
|---|---|---|
| `GET` | `/` | Serves the HTML setup page |
| `GET` | `/scan` | Returns nearby networks as JSON |
| `POST` | `/save` | Accepts `ssid` + `password`, saves to NVS, restarts |

### `/scan` Response Example
```json
{
  "networks": [
    { "ssid": "HomeNetwork_5G", "rssi": -52, "encrypted": true },
    { "ssid": "AndroidAP",      "rssi": -71, "encrypted": false }
  ]
}
```

### `/save` Request
```
Content-Type: application/x-www-form-urlencoded

ssid=HomeNetwork_5G&password=mysecretpassword
```

---

## 🗂️ Project Structure

```
esp32-wifi-provisioning/
├── ESP32_WiFi_Provisioning.ino   # Main sketch
└── README.md                     # This file
```

The entire HTML/CSS/JS page is embedded as a PROGMEM string inside the `.ino` file — no SPIFFS or SD card needed.

---

## 🔧 Troubleshooting

| Problem | Solution |
|---|---|
| Captive portal doesn't open | Navigate manually to `http://192.168.4.1` |
| Networks not showing in dropdown | Tap "Refresh list" on the setup page |
| Device not connecting after setup | Hold BOOT 3s to reset, then re-provision |
| AP not visible in phone Wi-Fi list | Check `AP_SSID` / `AP_PASSWORD` in code; restart device |
| LED not blinking | Set `STATUS_LED` to your board's LED GPIO or `-1` to disable |
| "Connection timed out" in Serial | Wrong password entered; re-provision with correct one |

---

## 🛡️ Security Notes

- Credentials are stored in ESP32's **NVS (Non-Volatile Storage)**, which is internal flash — not accessible over-the-air.
- The AP password prevents unauthorized devices from reaching the setup page.
- For production deployments, consider using **WPA2-Enterprise** or **BLE provisioning** with certificate pinning for higher security requirements.
- The `/save` endpoint is only active while in AP mode — it is not reachable once the device joins the main Wi-Fi network.

---

## 🗺️ Roadmap

- [ ] OTA (Over-the-Air) firmware update integration
- [ ] BLE provisioning support (alternative to captive portal)
- [ ] Multi-network fallback (store and try multiple SSIDs)
- [ ] HTTPS / self-signed certificate support
- [ ] SPIFFS version with separate HTML file

---

## 📄 License

MIT License — free to use in personal and commercial products.

---

## 🙌 Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you'd like to change.

1. Fork the repository
2. Create your feature branch: `git checkout -b feature/my-feature`
3. Commit your changes: `git commit -m 'Add my feature'`
4. Push to the branch: `git push origin feature/my-feature`
5. Open a Pull Request

---

<p align="center">Made with ☕ for the ESP32 community</p>
