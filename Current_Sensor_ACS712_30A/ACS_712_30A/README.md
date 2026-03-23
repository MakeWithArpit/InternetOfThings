# Current Sensor — ACS712 (30A) with ESP32

This project demonstrates how to measure AC current using two ACS712 30A current sensors connected to an ESP32. The sensors are automatically calibrated on startup using the library's built-in midpoint detection, and measured current values are printed to the Serial Monitor every second.

---

## Features

- Reads AC current from two independent ACS712 30A sensors simultaneously
- Automatic sensor calibration on startup using `autoMidPoint()`
- Uses the ESP32's 12-bit ADC with a 3.3V reference for accurate readings
- Serial monitor output with current values in Amperes

---

## Hardware Requirements

| Component | Specification |
|-----------|--------------|
| Microcontroller | ESP32 Development Board |
| Current Sensor | ACS712 30A Module (x2) |
| Miscellaneous | Jumper wires |

> The ACS712 30A variant supports a measurement range of -30A to +30A. It operates on 5V but its output signal is safe to read with ESP32's 3.3V ADC when used carefully.

---

## Wiring

| ACS712 Pin | ESP32 Pin |
|------------|-----------|
| Sensor 1 OUT | GPIO 34 |
| Sensor 2 OUT | GPIO 35 |
| VCC | 5V (VIN pin) |
| GND | GND |

> GPIO 34 and GPIO 35 are input-only pins on the ESP32 and are well suited for analog sensor readings.

---

## Software Requirements

- Arduino IDE 1.8 or higher
- ESP32 board package installed in Arduino IDE

### Required Library

| Library | Purpose |
|---------|---------|
| ACS712 (by Rob Tillaart) | ACS712 sensor driver with AC current measurement |

Install via **Sketch > Include Library > Manage Libraries** and search for **ACS712**.

---

## Setup Instructions

1. Connect both ACS712 sensor output pins to GPIO 34 and GPIO 35.
2. Power the ACS712 modules from the ESP32's 5V (VIN) pin.
3. Open `ACS_712_30A.ino` in Arduino IDE.
4. Select board **ESP32 Dev Module** and the correct COM port under **Tools**.
5. Upload the sketch to the ESP32.

---

## Code Overview

```cpp
ACS712 sensor1(34, 3.3, 4095);  // GPIO 34, 3.3V reference, 12-bit ADC
ACS712 sensor2(35, 3.3, 4095);  // GPIO 35, 3.3V reference, 12-bit ADC
```

- `34` / `35` — ADC pin numbers
- `3.3` — ESP32 ADC reference voltage in volts
- `4095` — Maximum ADC value for 12-bit resolution

On startup, `autoMidPoint()` is called for each sensor. This calibrates the zero-current offset by averaging multiple ADC readings while no current flows. **Ensure no current passes through the sensor during the 2-second calibration delay.**

In the main loop, `mA_AC(50)` takes 50 samples and returns the RMS AC current in milliamps. This is divided by 1000 to convert to Amperes.

---

## Usage

Open the Serial Monitor at **115200 baud**. After the calibration message, readings will appear every second:

```
Calibrating sensors...
Calibration Done
Sensor 1 Current: 1.23 A
Sensor 2 Current: 0.87 A
Sensor 1 Current: 1.25 A
Sensor 2 Current: 0.89 A
```

---

## Troubleshooting

| Issue | Resolution |
|-------|-----------|
| Current reads 0.00 A always | Confirm sensor output is connected to the correct ADC pin; verify VCC is 5V |
| Readings are unstable or noisy | Add a small decoupling capacitor (100nF) near the sensor VCC pin; increase sample count in `mA_AC()` |
| Calibration reads non-zero with no load | Re-run calibration with no current flowing; do not power up while load is connected |
| Library not found | Install **ACS712** by Rob Tillaart from the Library Manager |

---

## Extending This Project

- Add WiFi and MQTT to publish current readings to ThingsBoard or any MQTT broker.
- Use the power formula `P = V x I` if line voltage is known, to calculate power consumption.
- Add alert logic to trigger a relay or notification when current exceeds a threshold.

---

## License

This project is licensed under the **MIT License**.

---

## Author

**Arpit Gangwar**  
B.Tech Computer Science and Engineering  
Invertis University, Bareilly
