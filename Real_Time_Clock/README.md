# Real-Time Clock with DS3231 and ESP32

This project demonstrates how to interface the DS3231 Real-Time Clock (RTC) module with an ESP32 microcontroller over the I2C bus. The current date and time are read from the RTC every second and printed to the Arduino Serial Monitor.

The DS3231 is a high-precision, temperature-compensated RTC that maintains accurate timekeeping even during power outages, thanks to its onboard battery backup capability.

---

## Features

- Reads current date and time from DS3231 over I2C
- Outputs formatted date, time, and Unix timestamp to the serial monitor every second
- Supports manual time setting via a single commented line in code
- Minimal hardware setup — only two wires (SDA, SCL) plus power

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Development Board |
| RTC Module | DS3231 with CR2032 coin cell battery |
| Miscellaneous | Jumper wires |

---

## Wiring

| DS3231 Pin | ESP32 Pin |
|------------|-----------|
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| VCC | 3.3V |
| GND | GND |

> The DS3231 module operates at 3.3V and is compatible with the ESP32's I2C pins directly.

---

## Software Requirements

- Arduino IDE 1.8 or higher
- ESP32 board package installed in Arduino IDE

### Required Library

| Library | Purpose |
|---------|---------|
| RTClib (by Adafruit) | DS3231 driver and DateTime utilities |

Install via **Sketch > Include Library > Manage Libraries** and search for **RTClib**.

---

## Setup Instructions

1. Wire the DS3231 module to the ESP32 as per the wiring table above.
2. Open `DS3231.ino` in Arduino IDE.
3. **First-time use only** — uncomment the following line to set the RTC to the current time, then re-comment it after uploading once:

```cpp
// rtc.adjust(DateTime(2025, 5, 1, 22, 17, 10));
```

Replace the values with the current date and time in `(YYYY, MM, DD, HH, MM, SS)` format.

4. Select board **ESP32 Dev Module** and the correct COM port under **Tools**.
5. Upload the sketch to the ESP32.

---

## Usage

Open the Serial Monitor at **115200 baud**. The output will update every second:

```
1746132000
2025/5/1 22:20:00
1746132001
2025/5/1 22:20:01
```

The first line is the Unix timestamp; the second line is the human-readable date and time.

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| Serial prints "RTC not found" | Check I2C wiring; ensure SDA is on GPIO 21 and SCL is on GPIO 22 |
| Time is incorrect or shows 2000/1/1 | The RTC was never set; uncomment `rtc.adjust(...)`, upload, then re-comment |
| Time resets after power cycle | The backup coin cell (CR2032) is missing or discharged — replace it |
| Library not found | Install **RTClib** by Adafruit from the Library Manager |

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
