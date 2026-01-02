# VTOL Flying Wing - ESP32 Motor Test System

![Version](https://img.shields.io/badge/version-2.5.2-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Platform](https://img.shields.io/badge/platform-ESP32-orange.svg)

**Professional motor testing system with web interface, temperature monitoring, and dual protocol support (PWM/DSHOT)**

![VTOL Motor Test](https://img.shields.io/badge/VTOL-Flying%20Wing-brightgreen.svg)

---

## 🚀 Features

- **Web User Interface** - Control from any WiFi device (phone/tablet/computer)
- **Dual Protocol Support** - PWM and DSHOT300 with seamless switching
- **Temperature Monitoring** - Real-time motor temperature with auto-stop at 95°C
- **Real-time Data** - 1-second sensor updates via WebSocket
- **Mobile Responsive** - Optimized for phones, tablets, and desktop
- **Safety Systems** - Multi-layer safety including ARM/DISARM and emergency stop
- **Automated Testing** - SWEEP function tests 6 throttle points automatically
- **Data Export** - CSV export for analysis in Excel/Google Sheets

---

## 📋 Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [Web Interface](#web-interface)
- [Serial Commands](#serial-commands)
- [Safety Features](#safety-features)
- [Troubleshooting](#troubleshooting)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)

---

## 🔧 Hardware Requirements

### Required Components

| Component | Specification | Quantity | Purpose |
|-----------|--------------|----------|---------|
| **ESP32** | Development Board | 1 | Main controller with WiFi |
| **ESC** | 60A+ with DSHOT support | 1 | Motor controller |
| **Motor** | EMAX ECO II 2807 1500KV | 1 | Test subject |
| **Temperature Sensor** | MAX31855 + K-type probe | 1 | Motor temperature monitoring |
| **Power Monitor** | PM02v3 (optional) | 1 | Voltage/current sensing |
| **Battery** | 6S LiPo 6000-8000mAh | 1 | Power supply |

### Tested Hardware

- **Motor:** EMAX ECO II 2807 1500KV
- **ESC:** SpeedyBee 60A with BlueJay firmware
- **Thermocouple:** K-type, 0-400°C range
- **ESP32:** Generic ESP32 DevKit v1

---

## 💾 Software Requirements

### Arduino Libraries

Install via **Arduino IDE → Tools → Manage Libraries:**

| Library | Author | Version | Required |
|---------|--------|---------|----------|
| ESP Async WebServer | mathieucarbou | ≥3.0.0 | ✅ Yes |
| AsyncTCP | mathieucarbou | ≥3.0.0 | ✅ Yes |
| ArduinoJson | Benoit Blanchon | 6.x | ✅ Yes |
| Adafruit MAX31855 | Adafruit | Latest | ✅ Yes |
| ESP32Servo | Kevin Harrington | Latest | ✅ Yes |
| DShotRMT | derdoktor667 | ≥0.9.5 | ✅ Yes |

⚠️ **Important:** Use **mathieucarbou** forks of ESPAsyncWebServer and AsyncTCP, not the original me-no-dev versions (compatibility issues with ESP32 core 3.0+)

### Development Environment

- **Arduino IDE:** 2.x (recommended) or 1.8.19+
- **ESP32 Board Package:** 2.0.x or 3.0.x
- **Upload Speed:** 921600 baud
- **Flash Size:** 4MB

---

## 📥 Installation

### 1. Clone Repository

```bash
git clone https://github.com/YOUR_USERNAME/vtol-motor-test.git
cd vtol-motor-test
```

### 2. Install Libraries

Open Arduino IDE and install all required libraries (see table above).

**Installation Order (Important!):**
1. AsyncTCP
2. ESP Async WebServer
3. ArduinoJson (v6.x)
4. Adafruit MAX31855
5. ESP32Servo
6. DShotRMT

### 3. Hardware Wiring

```
ESP32 Pin Connections:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Motor Control:
  GPIO 25 → ESC Signal Wire

Temperature (MAX31855):
  GPIO 18 → SCK (Clock)
  GPIO 5  → CS (Chip Select)
  GPIO 19 → SO (Data Out)
  3.3V    → VCC
  GND     → GND

Power Monitor (PM02):
  GPIO 34 → Voltage Sensor
  GPIO 32 → ESC Current Sensor
  3.3V    → VCC
  GND     → GND

⚠️ CRITICAL: Common ground for all components!
```

### 4. Upload Sketch

1. Open `ESP32_Motor_Test.ino`
2. Select board: **ESP32 Dev Module**
3. Select correct COM port
4. Click **Upload** (Ctrl+U)

---

## 🚀 Quick Start

### First Time Setup

1. **Upload sketch** to ESP32
2. **Open Serial Monitor** (115200 baud)
3. **Verify system startup:**
   ```
   +========================================================+
   |  ESP32 MOTOR TEST - v2.5.2                            |
   +========================================================+
   
   [INIT] MAX31855 Thermocouple... OK
   [WIFI] AP IP address: 192.168.4.1
   [WEB] Web server started
   
   SYSTEM READY
   ```

### Connect to Web Interface

1. **Connect to WiFi:**
   - Network: `MotorTest_Rig`
   - Password: `test1234`

2. **Open browser:**
   - Navigate to: `http://192.168.4.1`

3. **Control motor:**
   - Click **ARM** button
   - Use slider or quick buttons for throttle
   - Monitor temperature in real-time

### Basic Test Procedure

```
1. ARM motor
2. Set throttle (T20 = 20%)
3. Monitor temperature
4. Record thrust reading
5. DISARM when complete
```

---

## 🌐 Web Interface

### Features

- **PWM/DSHOT Mode Selector** - Switch between protocols
- **Throttle Control** - Slider + quick buttons (0%, 5%, 10%, 25%, 50%, 75%, 95%)
- **Temperature Display** - Large, color-coded with warnings
- **Real-time Sensors** - Voltage, current, power, capacity
- **Status Indicators** - Connection, mode, sensor health
- **Mobile Responsive** - Works on any device

### Color-Coded Temperature

| Color | Temperature | Status |
|-------|------------|--------|
| 🟢 Green | <60°C | Normal |
| 🟡 Orange | 60-75°C | Warming |
| 🟠 Orange | 75-85°C | Hot |
| 🔴 Red (Flashing) | ≥85°C | Critical |

**Auto-stop at 95°C for safety!**

---

## 💻 Serial Commands

### Motor Control

```
ARM           - Arm motor (enable throttle)
DISARM        - Disarm motor (disable throttle)
STOP          - Emergency stop
T<0-100>      - Set throttle (e.g., T50 = 50%)
```

### Mode Switching

```
PWM           - Switch to PWM mode
DSHOT         - Switch to DSHOT mode
```

### Data Display

```
POWER / P     - Show all sensor data
TEMP          - Show temperature details
STATUS        - Show complete system status
```

### Testing

```
SWEEP         - Automated thrust test (6 points)
EXPORT        - Export test data as CSV
```

### Help

```
HELP / H / ?  - Show command reference
```

---

## 🛡️ Safety Features

### Multi-Layer Protection

1. **ARM/DISARM System** - Prevents accidental starts
2. **Throttle Validation** - Range checking (0-100%)
3. **Temperature Monitoring** - Continuous monitoring with warnings
4. **Auto-Stop** - Automatic shutdown at 95°C
5. **Emergency Stop** - Always available (web + serial)
6. **Mode Change Protection** - Cannot switch while armed

### Temperature Thresholds

```
60°C  - Warm (yellow warning)
75°C  - Hot (orange warning)
85°C  - Very hot (red warning)
95°C  - CRITICAL (auto-stop)
```

---

## 🔍 Troubleshooting

### Cannot Connect to WiFi

**Problem:** Network not visible or won't connect

**Solutions:**
1. Verify ESP32 powered on
2. Check Serial Monitor shows "AP IP address: 192.168.4.1"
3. Password is exactly: `test1234`
4. Reboot ESP32 if needed

### Motor Doesn't Respond

**PWM Mode:**
1. Check ESC powered (battery connected)
2. Verify ARM status
3. Listen for ESC startup beeps
4. Check wiring: GPIO 25 → ESC signal

**DSHOT Mode:**
1. Verify DSHOT task started (check serial)
2. ESC must support DSHOT300
3. BlueJay or compatible firmware required
4. Try PWM mode to verify hardware

### Temperature Shows "--°C"

**Solutions:**
1. Check MAX31855 wiring (5 connections)
2. Verify VCC = 3.3V (NOT 5V!)
3. Ensure thermocouple connected to module
4. Check for loose connections

### Compilation Errors

**Error: mbedtls_md5_starts_ret not declared**

**Solution:** Install mathieucarbou versions of:
- ESP Async WebServer
- AsyncTCP

(Not the me-no-dev versions)

---

## 📚 Documentation

Complete documentation available in `/docs`:

- **Web Interface Guide** - Complete WUI implementation details
- **Hardware Setup** - Detailed wiring diagrams
- **Sensor Calibration** - Calibration procedures
- **Troubleshooting Guide** - Comprehensive problem solving
- **MAX31855 Diagnosis** - Temperature sensor troubleshooting

---

## 🏗️ Project Structure

```
vtol-motor-test/
├── ESP32_Motor_Test.ino    # Main sketch
├── README.md                              # This file
├── LICENSE                                # MIT License
├── docs/
│   ├── WEB_INTERFACE.md                  # Web UI documentation
│   ├── HARDWARE_SETUP.md                 # Wiring and setup
│   ├── TROUBLESHOOTING.md                # Problem solving
│   └── MAX31855_DIAGNOSIS.md             # Temperature sensor guide
├── examples/
│   ├── basic_test/                       # Simple motor test
│   └── sweep_test/                       # Automated sweep example
└── images/
    ├── web_interface.png                 # Screenshots
    ├── hardware_setup.jpg                # Wiring photos
    └── wiring_diagram.png                # Schematic
```

---

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Development Guidelines

- Follow existing code style
- Add comments for complex sections
- Test on actual hardware before submitting
- Update documentation as needed
- Include example usage

---

## 📊 Performance Metrics

| Metric | Value | Notes |
|--------|-------|-------|
| **WebSocket Latency** | 20-50ms | WiFi + processing |
| **Temperature Update** | 250ms | MAX31855 conversion |
| **Sensor Broadcast** | 1 second | Configurable |
| **DSHOT Update Rate** | 1kHz | Continuous stream |
| **Memory Usage** | 83% Flash, 14% RAM | ESP32 with 4MB |
| **WiFi Range** | ~30 feet | Typical indoor |
| **Concurrent Clients** | 4-5 | ESP32 hardware limit |

---

## 🔄 Version History

| Version | Date | Changes |
|---------|------|---------|
| **2.5.2** | Dec 21, 2025 | Mode selector UI, 5%/10% throttle buttons |
| **2.5.1** | Dec 21, 2025 | Critical sensor fixes, voltage calculation corrected |
| **2.5.0** | Dec 21, 2025 | Web interface + temperature monitoring |
| **2.4.0** | Dec 14, 2025 | Automated SWEEP, CSV export |
| **2.3.0** | Dec 14, 2025 | ESC current sensor, dual sensors |

See [CHANGELOG.md](CHANGELOG.md) for detailed history.

---

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

**Libraries:**
- ESP32Servo by Kevin Harrington
- DShotRMT by derdoktor667
- Adafruit MAX31855 by Adafruit Industries
- ESP Async WebServer by mathieucarbou
- AsyncTCP by mathieucarbou
- ArduinoJson by Benoit Blanchon

**Hardware:**
- ESP32 by Espressif Systems
- EMAX ECO II motors
- SpeedyBee ESC

---

## 📞 Support

**Issues:** [GitHub Issues](https://github.com/YOUR_USERNAME/vtol-motor-test/issues)

**Documentation:** See `/docs` folder

**Hardware:** Check troubleshooting guide first

---

## ⭐ Star History

If you find this project useful, please consider giving it a star!

---

**Made with ❤️ for the VTOL Flying Wing Project**

*Desktop Aerospace Manufacturing • Open Source Motor Testing*
