# # IoT-Based Water Cleaning and Monitoring System

An IoT project that monitors water quality parameters (water level, TDS, turbidity) and automates cleaning using a motor. It supports short-range data transmission using LoRa and local/manual control via Bluetooth voice commands.

---

## Features

- Real-time water quality monitoring (TDS, turbidity, level)
- Automatic tank cleaning using a scrubber motor
- ESP32-based data handling and wireless communication
- Bluetooth voice command support for manual control
- LCD (I2C) display cycling through sensor data and status
- Blynk app integration for IoT dashboard

---

## Hardware Components

- ESP32 Dev Board (x2)
- TDS Sensor
- Turbidity Sensor
- Ultrasonic Sensor (e.g., HC-SR04)
- 500 RPM Motor + Shaft + Scrubber
- Relay Module (1-Channel)
- I2C LCD Display (16x2 or 20x4)
- Bluetooth Module (HC-05)
- Power Supply (USB/Battery)

---

## Required Libraries

Install the following libraries from the Arduino Library Manager:

| Library Name                  | Use                                     |
|-------------------------------|------------------------------------------|
| `LiquidCrystal_I2C`           | For displaying data on I2C LCD          |
| `SoftwareSerial`              | For serial communication with HC-05     |
| `Blynk` / `BlynkSimpleEsp32`  | For Blynk app integration                |
| `Wire`                        | For I2C communication with LCD          |
| `NewPing` (optional)          | For more accurate ultrasonic reading    |

To install a library:

```arduino
Sketch -> Include Library -> Manage Libraries...
For any further clarifications leave a message to pssudharsansreeram@gmail.com
Thank you
