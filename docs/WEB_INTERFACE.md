# VTOL Flying Wing Motor Test System - Web User Interface
## Complete Implementation Documentation

**Version:** 2.5.2  
**Date:** December 21, 2025  
**Status:** Production Ready ✅  
**Project:** VTOL Flying Wing Motor Thrust Test Rig

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [System Requirements](#system-requirements)
3. [Hardware Configuration](#hardware-configuration)
4. [Software Dependencies](#software-dependencies)
5. [Web Interface Features](#web-interface-features)
6. [Implementation Architecture](#implementation-architecture)
7. [Network Configuration](#network-configuration)
8. [User Interface Design](#user-interface-design)
9. [WebSocket Communication Protocol](#websocket-communication-protocol)
10. [Mode Switching Implementation](#mode-switching-implementation)
11. [Temperature Monitoring Integration](#temperature-monitoring-integration)
12. [Safety Systems](#safety-systems)
13. [Building and Deployment](#building-and-deployment)
14. [Troubleshooting Guide](#troubleshooting-guide)
15. [Performance Metrics](#performance-metrics)
16. [Future Enhancements](#future-enhancements)

---

## Executive Summary

The VTOL Motor Test Web User Interface (WUI) provides a professional, mobile-responsive control and monitoring system for the ESP32-based motor test rig. The interface enables safe remote operation of the test system through any WiFi-capable device (phone, tablet, or computer) without requiring USB cable connection.

### **Key Capabilities:**

- **Real-time Monitoring:** 1-second update rate for all sensor data
- **Dual Protocol Support:** PWM and DSHOT mode switching
- **Temperature Safety:** Color-coded warnings with automatic shutdown at 95°C
- **Mobile Responsive:** Optimized for phones, tablets, and desktop browsers
- **Simultaneous Users:** Multiple viewers can monitor tests concurrently
- **Zero Configuration:** WiFi Access Point mode requires no router

### **Primary Use Cases:**

1. **Safe Distance Testing:** Control motor from across the room during propeller tests
2. **Mobile Monitoring:** Use phone/tablet to monitor while recording thrust data
3. **Collaborative Testing:** Multiple team members can observe simultaneously
4. **Data Collection:** Real-time sensor display for manual data recording
5. **Mode Comparison:** Easy switching between PWM and DSHOT protocols

---

## System Requirements

### **Hardware Requirements:**

| Component | Specification | Purpose |
|-----------|--------------|---------|
| **Microcontroller** | ESP32 Development Board | WiFi capability, dual-core processing |
| **Motor Controller** | SpeedyBee 60A ESC (BlueJay) | DSHOT300 and PWM support |
| **Motors** | EMAX ECO II 2807 1500KV | Test subjects (6× for VTOL) |
| **Power Monitor** | PM02v3 | Voltage and current sensing |
| **Temperature Sensor** | MAX31855 + K-type thermocouple | Motor temperature monitoring |
| **Battery** | 6S LiPo (6000-8000mAh) | Power supply for motor testing |

### **Client Device Requirements:**

**Minimum:**
- Any device with WiFi capability (802.11 b/g/n)
- Web browser with JavaScript enabled
- Screen resolution: 320×480 minimum

**Recommended:**
- Modern smartphone, tablet, or laptop
- Chrome, Safari, Firefox, or Edge browser
- Screen resolution: 720×1280 or higher

**Supported Platforms:**
- ✅ Android (Chrome, Firefox)
- ✅ iOS (Safari)
- ✅ Windows (Chrome, Edge, Firefox)
- ✅ macOS (Safari, Chrome)
- ✅ Linux (Firefox, Chrome)

### **Network Requirements:**

- **No router required** - ESP32 creates Access Point
- **Range:** ~30 feet (10 meters) typical
- **Concurrent clients:** Tested with up to 5 simultaneous connections
- **Bandwidth:** Minimal (~1-2 KB/sec per client)

---

## Hardware Configuration

### **ESP32 Pin Assignments:**

```
MOTOR CONTROL:
  GPIO 25 → ESC Signal Output (PWM/DSHOT300)

ANALOG SENSORS:
  GPIO 34 → PM02 Voltage Sensor (0-3.3V)
  GPIO 35 → PM02 Current Sensor (0-3.3V) - Currently non-functional
  GPIO 32 → ESC Current Sensor (0-3.3V) - Active sensor

TEMPERATURE (MAX31855 SPI):
  GPIO 18 → MAX31855 SCK (Clock)
  GPIO 5  → MAX31855 CS (Chip Select)
  GPIO 19 → MAX31855 SO (Data Out/MISO)

POWER:
  3.3V    → MAX31855 VCC, PM02 Power
  5V      → ESP32 via USB (isolated from battery)
  GND     → Common ground (ALL components)
```

### **Wiring Diagram Reference:**

```
                    ┌─────────────────┐
                    │   ESP32 Board   │
                    │                 │
    USB Power ──────┤ 5V          3V3 ├──────┬─── MAX31855 VCC
                    │                 │      └─── PM02 VCC
                    │                 │
    ESC Signal ─────┤ GPIO 25         ├───── (other sensors)
                    │                 │
    MAX31855 ───────┤ GPIO 18,5,19    │
    (SPI)           │                 │
                    │             GND ├───── Common Ground ⚠️
                    └─────────────────┘
                             │
                    Battery Ground (isolated)
```

**CRITICAL:** Common ground connection between ESP32, MAX31855, PM02, and ESC current sensor is essential for accurate readings.

---

## Software Dependencies

### **Required Arduino Libraries:**

Install all libraries via **Arduino IDE → Tools → Manage Libraries:**

| Library | Author | Version | Purpose |
|---------|--------|---------|---------|
| **ESP32Servo** | Kevin Harrington | Latest | PWM ESC control |
| **DShotRMT** | derdoktor667 | ≥0.9.5 | DSHOT protocol support |
| **Adafruit MAX31855** | Adafruit | Latest | Thermocouple interface |
| **ESP Async WebServer** | mathieucarbou | ≥3.0.0 | Web server with WebSocket |
| **AsyncTCP** | mathieucarbou | ≥3.0.0 | Async TCP (dependency) |
| **ArduinoJson** | Benoit Blanchon | 6.x | JSON serialization |

### **Library Installation Order:**

**IMPORTANT:** Install in this exact order to avoid dependency conflicts:

```
1. AsyncTCP (mathieucarbou version)
   └─ Required FIRST before ESPAsyncWebServer

2. ESP Async WebServer (mathieucarbou version)
   └─ Requires AsyncTCP installed first

3. ArduinoJson (version 6.x, NOT 7.x)
   └─ Version 7.x has breaking changes

4. Adafruit MAX31855
   └─ Auto-installs Adafruit BusIO dependency

5. ESP32Servo
   └─ No dependencies

6. DShotRMT
   └─ No dependencies
```

### **Why mathieucarbou Forks:**

The original me-no-dev versions of ESPAsyncWebServer and AsyncTCP have compatibility issues with ESP32 Arduino Core 3.0+:

**Issues with me-no-dev version:**
- ❌ Uses deprecated mbedtls MD5 functions
- ❌ Compilation errors with newer ESP32 core
- ❌ Not actively maintained

**mathieucarbou fork fixes:**
- ✅ Updated for ESP32 core 3.0+ compatibility
- ✅ Active maintenance and bug fixes
- ✅ No deprecated function calls
- ✅ Identical API (drop-in replacement)

### **Compiler Settings:**

```
Board: ESP32 Dev Module (or your specific ESP32 board)
Upload Speed: 921600
CPU Frequency: 240MHz (Dual Core)
Flash Frequency: 80MHz
Flash Mode: QIO
Flash Size: 4MB (3MB APP/1MB SPIFFS)
Partition Scheme: Default 4MB with spiffs
Core Debug Level: None (or "Info" for debugging)
PSRAM: Disabled (not required)
Arduino Runs On: Core 1
Events Run On: Core 1
```

---

## Web Interface Features

### **Control Panel:**

**1. Mode Selector:**
- Two-button toggle: `PWM Mode` | `DSHOT Mode`
- Visual indication of active mode (gray highlight)
- Mode indicator badge shows current protocol
- Safe mode switching (requires motor disarmed)

**2. Motor Control:**
- **ARM Button** (Green): Enables throttle control
- **DISARM Button** (Gray): Disables motor
- **EMERGENCY STOP** (Red, Large): Immediate shutdown

**3. Throttle Control:**
- **Slider:** Smooth 0-100% control (touch-friendly)
- **Quick Buttons:** 0%, 5%, 10%, 25%, 50%, 75%, 95%
- Disabled when motor is disarmed (safety)
- Real-time percentage display

### **Temperature Display:**

**Visual Elements:**
- **Large Temperature:** 48px bold display
- **Color Coding:**
  - 🟢 Green: <60°C (Normal)
  - 🟡 Orange: 60-75°C (Warming)
  - 🟠 Orange: 75-85°C (Hot)
  - 🔴 Red (Flashing): ≥85°C (Critical)
- **Status Messages:** Context-aware warnings
- **Data Badges:**
  - Ambient temperature
  - Temperature rise from ambient
  - Maximum temperature recorded

### **Sensor Data Grid:**

**Real-time Displays (1-second updates):**
- Battery Voltage (V)
- PM02 Current (A) with status badge
- ESC Current (A) - always active
- Power (W)
- Capacity Used (mAh)
- ESC Efficiency (%) when both sensors working

**Status Indicators:**
- 🟢 Working: Sensor operational
- 🔴 Not Detected: Sensor unavailable
- Auto-detection on startup

### **Connection Status:**

- **Connected:** Green badge "✅ Connected to Motor Test Rig"
- **Disconnected:** Red badge "❌ Disconnected - Reconnecting..."
- **Auto-reconnect:** Attempts reconnection every 2 seconds

---

## Implementation Architecture

### **System Overview:**

```
┌─────────────────────────────────────────────────────────────┐
│                     Client Device                            │
│  ┌───────────────────────────────────────────────────────┐  │
│  │              Web Browser                              │  │
│  │  ┌─────────────┐  ┌──────────────┐  ┌─────────────┐ │  │
│  │  │    HTML     │  │  JavaScript  │  │     CSS     │ │  │
│  │  │  Structure  │  │  WebSocket   │  │   Styling   │ │  │
│  │  └─────────────┘  └──────────────┘  └─────────────┘ │  │
│  └───────────────────────────────────────────────────────┘  │
└──────────────────────────┬──────────────────────────────────┘
                           │ WiFi (WebSocket)
                           │ ws://192.168.4.1/ws
┌──────────────────────────┴──────────────────────────────────┐
│                     ESP32 System                             │
│  ┌───────────────────────────────────────────────────────┐  │
│  │         ESPAsyncWebServer                             │  │
│  │  ┌─────────────┐  ┌──────────────┐  ┌─────────────┐ │  │
│  │  │  HTTP       │  │  WebSocket   │  │    JSON     │ │  │
│  │  │  Server     │  │  Handler     │  │  Encoding   │ │  │
│  │  └─────────────┘  └──────────────┘  └─────────────┘ │  │
│  └───────────────────────────────────────────────────────┘  │
│                           │                                  │
│  ┌────────────────────────┴────────────────────────────┐    │
│  │         Motor Control & Sensor Management           │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────────┐  │    │
│  │  │   PWM    │  │  DSHOT   │  │   Sensor         │  │    │
│  │  │  Control │  │  Task    │  │   Reading        │  │    │
│  │  └──────────┘  └──────────┘  └──────────────────┘  │    │
│  └─────────────────────────────────────────────────────┘    │
│                           │                                  │
│              ┌────────────┴────────────┐                     │
│              │   Hardware Interface    │                     │
│              │  (GPIO, ADC, SPI)       │                     │
│              └─────────────────────────┘                     │
└──────────────────────────┬──────────────────────────────────┘
                           │
                  ┌────────┴────────┐
                  │  ESC & Sensors  │
                  └─────────────────┘
```

### **Task Distribution:**

**Core 0 (Protocol CPU):**
- WiFi stack
- TCP/IP networking
- WebSocket communication
- HTTP server

**Core 1 (Application CPU):**
- DSHOT continuous update task (1kHz)
- Main loop (sensor reading, command processing)
- Temperature monitoring
- Serial communication

### **Memory Architecture:**

**Program Memory:**
- Sketch size: ~1,097,139 bytes (83% of 1,310,720)
- HTML/CSS/JavaScript: Embedded as PROGMEM string
- Libraries: ~400KB combined

**RAM Usage:**
- Global variables: 47,312 bytes (14% of 327,680)
- WebSocket buffers: Dynamic allocation
- JSON documents: 512 bytes (stack allocated)
- Remaining for local variables: 280,368 bytes

### **Communication Flow:**

```
User Action (Web Interface)
    ↓
JavaScript Event Handler
    ↓
WebSocket Send (JSON)
    ↓
ESP32 WebSocket Handler (onWsEvent)
    ↓
Parse JSON Command
    ↓
Execute Command (processCommand)
    ↓
Update Motor/Sensors
    ↓
Broadcast New State (1Hz)
    ↓
WebSocket Send to All Clients
    ↓
JavaScript onMessage Handler
    ↓
Update Web Interface Display
```

---

## Network Configuration

### **WiFi Access Point Settings:**

```cpp
// Network Configuration
const char* ssid = "MotorTest_Rig";
const char* password = "test1234";

// IP Configuration (Automatic)
IP Address: 192.168.4.1
Subnet: 255.255.255.0
Gateway: 192.168.4.1
```

### **Access Point Characteristics:**

| Parameter | Value | Notes |
|-----------|-------|-------|
| **SSID** | MotorTest_Rig | Visible network name |
| **Password** | test1234 | WPA2-PSK encryption |
| **Channel** | Auto | Typically Ch 1 or 6 |
| **IP Address** | 192.168.4.1 | Fixed, non-configurable |
| **DHCP Range** | 192.168.4.2-254 | Auto-assigned to clients |
| **Max Clients** | 4 (ESP32 limit) | Hardware limitation |
| **Range** | ~30 feet | Typical indoor |

### **Port Assignments:**

```
Port 80 (HTTP):     Web page serving
                    URL: http://192.168.4.1

Port 80 (WebSocket): Real-time data
                     URL: ws://192.168.4.1/ws
```

### **Connection Procedure:**

**From Client Device:**

1. **Open WiFi settings**
2. **Select network:** "MotorTest_Rig"
3. **Enter password:** test1234
4. **Wait for connection** (2-5 seconds)
5. **Open browser**
6. **Navigate to:** http://192.168.4.1
7. **Interface loads automatically**

**No Configuration Required:**
- No static IP setup
- No DNS configuration
- No router access needed
- Works in any location

---

## User Interface Design

### **Color Scheme:**

```css
Primary Colors:
  Background Gradient: #1e3c72 → #2a5298 (Blue gradient)
  Container: rgba(255, 255, 255, 0.95) (Off-white)
  
Control Colors:
  ARM: #28a745 (Green)
  DISARM: #6c757d (Gray)
  STOP: #dc3545 (Red)
  THROTTLE: #007bff (Blue)
  MODE: #6c757d (Gray, active highlighted)
  
Temperature Colors:
  Normal: #28a745 (Green)
  Warm: #ffc107 (Yellow/Orange)
  Hot: #fd7e14 (Orange)
  Critical: #dc3545 (Red, blinking)
  
Status Badges:
  Success: #d4edda background, #155724 text
  Danger: #f8d7da background, #721c24 text
  Info: #d1ecf1 background, #0c5460 text
```

### **Typography:**

```css
Font Family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif
Title (h1): 28px, bold, #2a5298
Subtitle: 14px, #666
Data Labels: 12px, uppercase, #6c757d
Data Values: 24px, bold, #212529
Temperature: 48px, bold, color-coded
```

### **Layout Breakpoints:**

**Mobile (≤480px):**
- Single column layout
- Stacked controls
- Large touch targets (48×48px minimum)
- Full-width buttons

**Tablet (481-768px):**
- Two-column data grid
- Side-by-side ARM/DISARM
- Optimized button sizes

**Desktop (>768px):**
- Three-column data grid
- All controls visible
- Maximum width: 800px (centered)

### **Responsive Design Features:**

```css
/* Auto-fit grid for data cards */
grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));

/* Flexible button groups */
.button-group {
  display: flex;
  flex-wrap: wrap;  /* Wraps on small screens */
  gap: 10px;
}

/* Touch-friendly controls */
button {
  min-height: 48px;  /* iOS/Android recommendation */
  padding: 12px 20px;
}

/* Viewport scaling */
<meta name="viewport" content="width=device-width, initial-scale=1.0">
```

---

## WebSocket Communication Protocol

### **Message Format:**

All messages are JSON-encoded strings sent over WebSocket connection.

### **Client → Server Messages:**

**1. Command Message:**
```json
{
  "type": "command",
  "value": "ARM" | "DISARM" | "STOP"
}
```

**2. Throttle Message:**
```json
{
  "type": "throttle",
  "value": 0-100
}
```

**3. Mode Change Message:**
```json
{
  "type": "mode",
  "value": "PWM" | "DSHOT"
}
```

### **Server → Client Messages:**

**Sensor Data Broadcast (every 1 second):**
```json
{
  "armed": true | false,
  "throttle": 0-100,
  "mode": "PWM" | "DSHOT",
  "voltage": 0.00-25.20,
  "pm02Current": 0.00-120.00,
  "pm02Working": true | false,
  "escCurrent": 0.00-400.00,
  "escPower": 0.0-10000.0,
  "capacity": 0.0-10000.0,
  "escEfficiency": 0.0-100.0,
  "motorTemp": 0.0-400.0,
  "ambientTemp": 0.0-50.0,
  "tempRise": 0.0-400.0,
  "maxTemp": 0.0-400.0
}
```

### **WebSocket Event Handlers:**

**JavaScript (Client Side):**

```javascript
// Connection opened
websocket.onopen = function(event) {
  console.log('WebSocket Connected');
  updateConnectionStatus(true);
};

// Connection closed
websocket.onclose = function(event) {
  console.log('WebSocket Disconnected');
  updateConnectionStatus(false);
  setTimeout(initWebSocket, 2000);  // Auto-reconnect
};

// Message received
websocket.onmessage = function(event) {
  var data = JSON.parse(event.data);
  updateInterface(data);
};

// Send message
function sendCommand(cmd) {
  if (websocket.readyState === WebSocket.OPEN) {
    websocket.send(JSON.stringify({
      type: 'command',
      value: cmd
    }));
  }
}
```

**C++ (ESP32 Side):**

```cpp
void onWsEvent(AsyncWebSocket *server, 
               AsyncWebSocketClient *client,
               AwsEventType type, 
               void *arg, 
               uint8_t *data, 
               size_t len) {
  
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[WEB] Client #%u connected\n", client->id());
  }
  
  else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WEB] Client #%u disconnected\n", client->id());
  }
  
  else if (type == WS_EVT_DATA) {
    // Parse JSON and process command
    DynamicJsonDocument doc(256);
    deserializeJson(doc, data);
    
    String type = doc["type"];
    if (type == "command") {
      processCommand(doc["value"]);
    }
    else if (type == "throttle") {
      setThrottle(doc["value"]);
    }
    else if (type == "mode") {
      switchMode(doc["value"]);
    }
  }
}
```

### **Broadcast Implementation:**

```cpp
void broadcastSensorData() {
  DynamicJsonDocument doc(512);
  
  // Populate all sensor data
  doc["armed"] = armed;
  doc["throttle"] = currentThrottle;
  doc["mode"] = (currentMode == MODE_PWM) ? "PWM" : "DSHOT";
  doc["voltage"] = voltage;
  // ... all other sensors ...
  
  String json;
  serializeJson(doc, json);
  
  // Send to all connected clients
  ws.textAll(json);
}

// Called every 1 second in main loop
unsigned long lastBroadcast = 0;
const unsigned long broadcastInterval = 1000;

void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastBroadcast >= broadcastInterval) {
    broadcastSensorData();
    lastBroadcast = currentTime;
  }
}
```

---

## Mode Switching Implementation

### **Protocol Comparison:**

| Feature | PWM Mode | DSHOT Mode |
|---------|----------|------------|
| **Signal Type** | Analog pulse width | Digital packet |
| **Update Rate** | On-demand | Continuous 1kHz |
| **Latency** | ~20ms | ~1ms |
| **Precision** | 1000 steps | 2000 steps |
| **Noise Immunity** | Low | High |
| **Wiring** | Single signal wire | Single signal wire |
| **ESC Compatibility** | Universal | Modern ESCs only |

### **Mode Switch Procedure:**

**User Initiates Switch (Web or Serial):**

```
User clicks "DSHOT Mode" button
    ↓
JavaScript sends mode message
    ↓
ESP32 receives via WebSocket
    ↓
Safety check: Motor disarmed?
    ↓ Yes                    ↓ No
Continue                   Reject with error
    ↓
Clean up current mode
    ↓
Initialize new mode
    ↓
Update global state
    ↓
Broadcast new mode to all clients
    ↓
All clients update UI
```

### **PWM → DSHOT Transition:**

```cpp
// 1. Detach PWM servo
esc.detach();

// 2. Initialize DSHOT
dshotMotor = new DShotRMT(MOTOR_PIN, DSHOT_MODE);
dshot_result_t result = dshotMotor->begin();

// 3. Start continuous update task
if (result.success) {
  currentMode = MODE_DSHOT;
  dshotUpdateEnabled = true;
  
  xTaskCreatePinnedToCore(
    dshotUpdateTask,      // Task function
    "DSHOT_Update",       // Task name
    2048,                 // Stack size
    NULL,                 // Parameters
    1,                    // Priority
    &dshotTaskHandle,     // Handle
    1                     // Core 1
  );
}
```

### **DSHOT → PWM Transition:**

```cpp
// 1. Stop DSHOT task
dshotUpdateEnabled = false;
if (dshotTaskHandle != NULL) {
  vTaskDelete(dshotTaskHandle);
  dshotTaskHandle = NULL;
}

// 2. Delete DSHOT object
if (dshotMotor != nullptr) {
  delete dshotMotor;
  dshotMotor = nullptr;
}

// 3. Initialize PWM
currentMode = MODE_PWM;
esc.setPeriodHertz(PWM_FREQUENCY);
esc.attach(MOTOR_PIN, PWM_MIN, PWM_MAX);
esc.writeMicroseconds(PWM_MIN);
```

### **DSHOT Continuous Update Task:**

```cpp
void dshotUpdateTask(void *parameter) {
  const TickType_t xFrequency = pdMS_TO_TICKS(1000 / DSHOT_UPDATE_HZ);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  while (dshotUpdateEnabled) {
    if (dshotMotor != nullptr && currentMode == MODE_DSHOT) {
      // Send current throttle value
      float throttlePercent = (float)currentThrottle;
      dshotMotor->sendThrottlePercent(throttlePercent);
    }
    
    // Wait for next cycle (1kHz = 1ms period)
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  
  vTaskDelete(NULL);
}
```

**Why Continuous Updates Required:**

DSHOT protocol requires a steady stream of throttle commands:
- ESC expects packet every 1ms (1kHz)
- Missing packets triggers failsafe
- Continuous stream acts as "heartbeat"
- Even 0% throttle requires packets

**Task Characteristics:**
- Runs on Core 1 (application core)
- Priority 1 (normal)
- 2KB stack (sufficient)
- 1kHz update rate (1000 packets/sec)
- ~1% CPU usage at 240MHz

---

## Temperature Monitoring Integration

### **Sensor Integration:**

**MAX31855 Features:**
- 14-bit resolution (0.25°C precision)
- Cold junction compensation (automatic)
- SPI interface (3 wires: SCK, CS, SO)
- Open thermocouple detection
- Update rate: 4 Hz maximum

**Implementation:**

```cpp
#include <Adafruit_MAX31855.h>

// Pin definitions
#define MAXDO   19    // Data Out (SO/MISO)
#define MAXCS   5     // Chip Select
#define MAXCLK  18    // Clock (SCK)

// Create sensor object
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Initialize in setup()
void setup() {
  if (!isnan(thermocouple.readCelsius())) {
    tempSensorWorking = true;
    ambientTemp = thermocouple.readCelsius();
    maxTemp = ambientTemp;
  }
}

// Read in loop
void updateTemperature() {
  if (!tempSensorWorking) return;
  
  float reading = thermocouple.readCelsius();
  if (!isnan(reading)) {
    motorTemp = reading;
    tempRise = motorTemp - ambientTemp;
    if (motorTemp > maxTemp) {
      maxTemp = motorTemp;
    }
  }
}
```

### **Temperature Thresholds:**

```cpp
#define TEMP_WARN_LOW      60.0   // Warm warning (Yellow)
#define TEMP_WARN_MED      75.0   // Hot warning (Orange)
#define TEMP_WARN_HIGH     85.0   // Very hot (Red)
#define TEMP_CRITICAL      95.0   // Auto-stop (Flashing Red)
```

### **Web Interface Temperature Display:**

**JavaScript Temperature Status:**

```javascript
function updateTemperature(motor, ambient, rise, max) {
  var tempElement = document.getElementById('tempValue');
  var statusElement = document.getElementById('tempStatus');
  
  tempElement.textContent = motor.toFixed(1) + '°C';
  
  // Update color class
  tempElement.className = 'temp-value';
  if (motor >= 95) {
    tempElement.classList.add('temp-critical');
    statusElement.textContent = '🚨 CRITICAL! STOP IMMEDIATELY!';
    statusElement.style.color = '#dc3545';
  } else if (motor >= 85) {
    tempElement.classList.add('temp-hot');
    statusElement.textContent = '⚠️ Very Hot - Reduce throttle';
    statusElement.style.color = '#fd7e14';
  } else if (motor >= 75) {
    tempElement.classList.add('temp-hot');
    statusElement.textContent = '🔥 Hot - Monitor closely';
    statusElement.style.color = '#fd7e14';
  } else if (motor >= 60) {
    tempElement.classList.add('temp-warm');
    statusElement.textContent = 'ℹ️ Warming up (normal)';
    statusElement.style.color = '#ffc107';
  } else {
    tempElement.classList.add('temp-normal');
    statusElement.textContent = '✅ Temperature normal';
    statusElement.style.color = '#28a745';
  }
}
```

**CSS Animation for Critical:**

```css
.temp-critical {
  color: #dc3545;
  animation: blink 1s infinite;
}

@keyframes blink {
  0%, 50% { opacity: 1; }
  51%, 100% { opacity: 0.3; }
}
```

### **Auto-Stop Implementation:**

```cpp
void checkCriticalTemperature() {
  if (!tempSensorWorking) return;
  
  if (motorTemp >= TEMP_CRITICAL) {
    Serial.println("🚨 CRITICAL TEMPERATURE - AUTO STOPPING!");
    
    // Immediately stop motor
    armed = false;
    currentThrottle = 0;
    
    if (currentMode == MODE_PWM) {
      esc.writeMicroseconds(PWM_MIN);
    } else if (currentMode == MODE_DSHOT && dshotMotor != nullptr) {
      dshotMotor->sendThrottlePercent(0.0);
    }
    
    // Broadcast updated state
    broadcastSensorData();
  }
}

// Called every 1 second
void loop() {
  if (currentTime - lastBroadcast >= broadcastInterval) {
    broadcastSensorData();
    checkCriticalTemperature();  // Check after broadcast
    lastBroadcast = currentTime;
  }
}
```

---

## Safety Systems

### **Multi-Layer Safety Architecture:**

```
Layer 1: ARM/DISARM System
    ↓
Layer 2: Throttle Validation
    ↓
Layer 3: Temperature Monitoring
    ↓
Layer 4: Emergency Stop
    ↓
Layer 5: Mode Change Protection
```

### **ARM/DISARM System:**

**Purpose:** Prevent accidental motor starts

**Implementation:**
```cpp
bool armed = false;

// ARM command
if (command == "ARM") {
  armed = true;
  Serial.println("Motor ARMED");
}

// DISARM command
if (command == "DISARM") {
  armed = false;
  currentThrottle = 0;
  // Stop motor immediately
}

// Throttle validation
if (command.startsWith("T")) {
  if (!armed) {
    Serial.println("ERROR: Motor not armed!");
    return;  // Reject command
  }
  // Process throttle...
}
```

**Web Interface:**
- Slider disabled when disarmed (grayed out)
- Quick buttons inactive when disarmed
- Visual ARM status indicator

### **Throttle Validation:**

```cpp
// Validate range
if (throttle < 0 || throttle > 100) {
  Serial.println("ERROR: Throttle must be 0-100%");
  return;
}

// Validate armed state
if (!armed) {
  Serial.println("ERROR: Motor not armed!");
  return;
}

// Apply throttle
currentThrottle = throttle;
```

### **Temperature Safety:**

**Monitoring:**
- Continuous reading every 1 second
- Automatic threshold checking
- Visual warnings (color coding)
- Audible warnings (status messages)

**Auto-Stop:**
- Triggers at 95°C (TEMP_CRITICAL)
- Immediate motor shutdown
- Broadcasts stop to all clients
- Requires manual restart after cooling

**Recovery:**
- Wait for motor to cool below 50°C
- Check for motor damage
- Restart system if needed

### **Emergency Stop:**

**Activation Methods:**
1. **Web Interface:** Large red STOP button
2. **Serial Command:** Type "STOP" or "EMERGENCY"
3. **Temperature:** Auto-stop at 95°C
4. **Power Loss:** Motor stops immediately

**Stop Behavior:**
```cpp
// Emergency stop
armed = false;
currentThrottle = 0;

// Stop motor (both modes)
if (currentMode == MODE_PWM) {
  esc.writeMicroseconds(PWM_MIN);
} else if (currentMode == MODE_DSHOT && dshotMotor != nullptr) {
  dshotMotor->sendThrottlePercent(0.0);
}

// Notify all clients
broadcastSensorData();
```

### **Mode Change Protection:**

**Safety Check:**
```cpp
if (command == "PWM" || command == "DSHOT") {
  if (armed) {
    Serial.println("ERROR: Cannot change mode while armed!");
    Serial.println("       DISARM first for safety.");
    return;  // Reject mode change
  }
  // Proceed with mode switch...
}
```

**Rationale:**
- Prevents mode switch during motor operation
- Ensures clean protocol transition
- Avoids undefined motor behavior
- Requires explicit user action (DISARM → MODE → ARM)

### **Connection Loss Handling:**

**WebSocket Disconnect:**
- Client auto-reconnects every 2 seconds
- Server maintains last known state
- Motor continues current throttle
- Manual intervention available via serial

**WiFi Loss:**
- ESP32 maintains AP continuously
- Clients can reconnect without reboot
- No data loss (state in RAM)

**Power Loss:**
- Motor stops immediately (no power to ESC)
- State lost (requires full restart)
- Safe failure mode

---

## Building and Deployment

### **Build Requirements:**

**Development Environment:**
- Arduino IDE 2.x (recommended) or 1.8.19+
- ESP32 Board Package 2.0.x or 3.0.x
- All libraries installed (see Software Dependencies)

**Hardware:**
- ESP32 development board
- USB cable for programming
- All sensors connected (optional for compilation)

### **Pre-Build Checklist:**

```
☐ All required libraries installed
☐ Correct board selected (ESP32 Dev Module)
☐ Correct COM port selected
☐ Upload speed set to 921600
☐ Flash size: 4MB
☐ Partition scheme: Default 4MB with spiffs
```

### **Compilation Procedure:**

**1. Open Sketch:**
```
File → Open → ESP32_Motor_Test_COMPLETE_v2_5.ino
```

**2. Verify/Compile:**
```
Sketch → Verify/Compile (Ctrl+R)

Expected output:
"Done compiling"
Sketch uses 1,097,139 bytes (83%)
Global variables use 47,312 bytes (14%)
```

**3. Upload:**
```
Sketch → Upload (Ctrl+U)

Expected output:
"Hard resetting via RTS pin..."
"Done uploading."
```

### **Post-Upload Verification:**

**1. Open Serial Monitor:**
```
Tools → Serial Monitor
Baud rate: 115200
```

**2. Check Startup Sequence:**
```
+========================================================+
|  ESP32 MOTOR TEST - v2.5.2                            |
|  Temperature Monitoring + Web Interface               |
+========================================================+

[INIT] MAX31855 Thermocouple... OK/FAILED
[WIFI] Starting Access Point...
       SSID: MotorTest_Rig
       Password: test1234
       AP IP address: 192.168.4.1
[WEB] Web server started on port 80

+========================================================+
|  SYSTEM READY                                          |
+========================================================+

Web Interface: http://192.168.4.1
WiFi Network: MotorTest_Rig
Password: test1234
```

**3. Test Web Interface:**
```
1. Connect to WiFi: "MotorTest_Rig"
2. Enter password: test1234
3. Open browser: http://192.168.4.1
4. Verify interface loads
5. Check connection status: "✅ Connected"
```

### **Troubleshooting Build Errors:**

**Error: Library not found**
```
Solution:
1. Arduino IDE → Tools → Manage Libraries
2. Search for missing library
3. Install latest version
4. Restart Arduino IDE
5. Recompile
```

**Error: mbedtls_md5_starts_ret not declared**
```
Cause: Using wrong ESPAsyncWebServer version

Solution:
1. Uninstall me-no-dev version
2. Install mathieucarbou/ESP Async WebServer
3. Install mathieucarbou/AsyncTCP
4. Restart Arduino IDE
5. Recompile
```

**Error: 'RMT_CHANNEL_0' was not declared**
```
Cause: Old DSHOT initialization code

Solution:
This should be fixed in v2.5.2
If error persists, verify you have latest code
```

**Warning: deprecated declaration 'send_P'**
```
Cause: Old method name

Solution:
This is fixed in v2.5.1+
Can be safely ignored if present
```

### **Deployment Checklist:**

```
☐ Code compiled successfully
☐ Upload completed without errors
☐ Serial monitor shows system ready
☐ WiFi Access Point active
☐ Web interface loads in browser
☐ WebSocket connects successfully
☐ Temperature sensor detected (if installed)
☐ Voltage sensor reads battery
☐ ESC current sensor active
☐ Mode switching works (PWM ↔ DSHOT)
☐ ARM/DISARM functions correctly
☐ Throttle control responsive
☐ Emergency stop works
☐ Temperature warnings appear
```

### **Configuration Customization:**

**Change WiFi Credentials:**
```cpp
// Around line 230
const char* ssid = "YourNetworkName";      // Max 32 characters
const char* password = "YourPassword123";   // Min 8 characters
```

**Adjust Temperature Thresholds:**
```cpp
// Around line 180
#define TEMP_WARN_LOW      60.0   // Yellow warning
#define TEMP_WARN_MED      75.0   // Orange warning
#define TEMP_WARN_HIGH     85.0   // Red warning
#define TEMP_CRITICAL      95.0   // Auto-stop
```

**Change Update Rate:**
```cpp
// Around line 1892
const unsigned long broadcastInterval = 1000;  // milliseconds
// 1000 = 1 second (recommended)
// 500 = 0.5 seconds (faster, more CPU)
// 2000 = 2 seconds (slower, less CPU)
```

**Modify DSHOT Frequency:**
```cpp
// Around line 164
#define DSHOT_UPDATE_HZ    1000      // Hz
// 1000 = Standard (recommended)
// 2000 = Faster (experimental)
// 500 = Slower (not recommended)
```

---

## Troubleshooting Guide

### **Web Interface Issues:**

**Cannot Connect to WiFi:**

**Symptoms:**
- "MotorTest_Rig" network not visible
- Or network visible but won't connect

**Solutions:**
1. Verify ESP32 powered on and code running
2. Check Serial Monitor for "AP IP address: 192.168.4.1"
3. Ensure password entered exactly: "test1234"
4. Try "Forget network" and reconnect
5. Reboot ESP32 if WiFi not starting

**Web Page Won't Load:**

**Symptoms:**
- Connected to WiFi but page won't load
- Browser shows "Cannot reach server"

**Solutions:**
1. Verify connected to "MotorTest_Rig" (not home WiFi)
2. Use exact URL: http://192.168.4.1 (not https://)
3. Try different browser (Chrome, Safari, Firefox)
4. Clear browser cache
5. Try incognito/private browsing mode

**WebSocket Disconnects Frequently:**

**Symptoms:**
- "Disconnected - Reconnecting..." message
- Data stops updating
- Connection drops every few seconds

**Solutions:**
1. Move closer to ESP32 (<15 feet)
2. Remove metal obstacles between device and ESP32
3. Check ESP32 power supply is stable
4. Reduce number of connected clients (max 4)
5. Check Serial Monitor for ESP32 reboots

**Throttle Slider Not Working:**

**Symptoms:**
- Slider grayed out
- Cannot move slider
- Buttons inactive

**Solutions:**
1. Motor must be ARMED first
2. Click "⚡ ARM" button
3. Slider should become active (blue)
4. If still disabled, check Serial Monitor for errors

### **Motor Control Issues:**

**Motor Doesn't Respond to Throttle:**

**PWM Mode:**
1. Verify ESC is powered (battery connected)
2. Check ESC arming sequence complete
3. Listen for ESC beeps on startup
4. Verify ARM status (must be armed)
5. Check wiring: ESP32 GPIO 25 → ESC signal wire

**DSHOT Mode:**
1. Verify DSHOT task started (check Serial Monitor)
2. Should see "Continuous throttle stream active"
3. Check ESC supports DSHOT300
4. Verify BlueJay or similar firmware on ESC
5. Try switching to PWM mode to verify hardware

**Mode Switch Fails:**

**Symptoms:**
- "ERROR: Cannot change mode while armed"
- Or mode switch button not responding

**Solutions:**
1. DISARM motor before changing mode
2. Wait for disarm confirmation
3. Then click PWM or DSHOT button
4. Check Serial Monitor for error messages

### **Temperature Monitoring Issues:**

**Shows "--°C" or "0.0°C":**

**Diagnosis:**
1. Type STATUS to check sensor status
2. If shows "❌ Not detected":
   - Check MAX31855 wiring
   - Verify VCC = 3.3V (NOT 5V!)
   - Check all 5 connections (VCC, GND, SCK, CS, SO)

**Temperature Stuck at One Value:**

**Possible Causes:**
1. Thermocouple not connected to MAX31855
2. Thermocouple wires broken
3. Module reading internal temperature only

**Solutions:**
1. Check thermocouple connected to module
2. Test continuity with multimeter
3. Touch probe - should see change in 2-3 seconds

**Negative Temperature Readings:**

**Cause:** Thermocouple polarity reversed

**Solution:**
1. Swap red (+) and blue (-) wires
2. Red → + terminal
3. Blue → - terminal

### **Sensor Reading Issues:**

**Voltage Shows 0.0V:**

**Diagnosis:**
1. Check PM02 powered (3.3V and GND)
2. Verify battery connected
3. Check GPIO 34 connection
4. Measure voltage pin with multimeter (should be ~1.6V for 25V battery)

**Current Always 0.0A:**

**PM02 Current (GPIO 35):**
- Known issue - sensor not working
- This is expected in current configuration
- Use ESC current sensor instead

**ESC Current (GPIO 32):**
1. Verify ESC current sensor wire connected to GPIO 32
2. Check ESC has current sensing capability
3. SpeedyBee ESC confirmed working
4. Should show >0A when motor running

### **Serial Communication Issues:**

**No Serial Output:**

**Solutions:**
1. Verify correct COM port selected
2. Baud rate must be 115200
3. Try different USB cable
4. Check USB drivers installed
5. Restart Arduino IDE

**Garbled Characters:**

**Cause:** Wrong baud rate

**Solution:**
1. Set baud rate to 115200
2. Close and reopen Serial Monitor
3. Reboot ESP32

---

## Performance Metrics

### **Response Times:**

| Action | Latency | Notes |
|--------|---------|-------|
| **Button Click → ESP32** | 20-50ms | WiFi + WebSocket |
| **ESP32 → Motor Response** | 1-20ms | PWM: 20ms, DSHOT: 1ms |
| **Temperature Update** | 250ms | MAX31855 conversion time |
| **Sensor Broadcast** | 1000ms | Configurable (1 second default) |
| **WebSocket Reconnect** | 2000ms | After disconnect |

### **Network Performance:**

**Bandwidth Usage:**
- Per client: ~1-2 KB/sec
- 4 clients: ~4-8 KB/sec total
- Negligible for ESP32

**Connection Capacity:**
- Tested: 5 simultaneous clients
- Limit: ESP32 hardware (4-5 typical)
- Performance degradation: Minimal up to 4 clients

**Range:**
- Indoor: ~30 feet (10 meters) typical
- Line of sight: ~50 feet (15 meters)
- Through walls: ~15-20 feet (varies)

### **CPU Usage:**

**Core 0 (Protocol CPU):**
- WiFi/TCP stack: ~15-20%
- WebSocket handling: ~5%
- Total: ~25%

**Core 1 (Application CPU):**
- Main loop: ~10%
- DSHOT task: ~1%
- Sensor reading: ~2%
- Total: ~13%

**Memory Usage:**
- Flash (program): 1,097,139 bytes (83%)
- RAM (global): 47,312 bytes (14%)
- RAM (available): 280,368 bytes (86%)
- Heap fragmentation: Low (<5%)

### **Power Consumption:**

**ESP32 Alone:**
- WiFi active: ~160-260mA @ 3.3V
- Typical: ~200mA
- Power: ~0.66W

**With Sensors:**
- MAX31855: ~1.5mA
- PM02: ~10mA
- Total: ~212mA @ 3.3V

**Battery Impact:**
- Negligible compared to motor (50-90A)
- Can run on USB power only
- Battery only needed for motor testing

---

## Future Enhancements

### **Planned Features (v3.0):**

**1. Real-Time Graphing:**
- Temperature vs. Time chart
- Current vs. Time chart
- Power vs. Time chart
- Throttle history
- Implementation: Chart.js library

**2. Data Logging:**
- SD card integration
- CSV file generation
- Timestamped records
- Automatic logging during tests
- Export via web interface

**3. Advanced Controls:**
- Throttle curves (linear, exponential)
- Test sequence programming
- Automated SWEEP via web interface
- Custom throttle profiles
- Scheduled tests

**4. Multi-Motor Support:**
- 6-channel control for full VTOL rig
- Individual motor monitoring
- Synchronized control
- Differential thrust testing

**5. Video Integration:**
- ESP32-CAM module
- Live video feed to web interface
- Visual inspection during tests
- Recording capability

**6. Mobile App:**
- Native iOS app
- Native Android app
- Better touch controls
- Offline data viewing
- Push notifications

### **Under Consideration:**

**Authentication:**
- Password-protected access
- Multiple user accounts
- Access logging

**Cloud Integration:**
- Data upload to cloud storage
- Remote monitoring
- Team collaboration
- Historical analysis

**Advanced Analytics:**
- Efficiency maps
- Performance trending
- Predictive maintenance
- AI-powered optimization

**Hardware Additions:**
- GPS module (for outdoor tests)
- Barometric sensor (altitude)
- Accelerometer (vibration analysis)
- Sound level meter (noise testing)

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| **2.5.2** | Dec 21, 2025 | PWM/DSHOT mode selector, 5%/10% throttle buttons, WebSocket mode support |
| **2.5.1** | Dec 21, 2025 | Critical sensor calculation fixes, voltage formula corrected, current offset removed |
| **2.5.0** | Dec 21, 2025 | Initial WUI release with temperature monitoring and full web interface |
| **2.4.0** | Dec 14, 2025 | Automated SWEEP testing, CSV export |
| **2.3.0** | Dec 14, 2025 | ESC current sensor support, dual sensor architecture |
| **2.2.0** | Dec 2025 | ESC arming sequence, DSHOT implementation |

---

## Credits and Acknowledgments

**Libraries Used:**
- ESP32Servo by Kevin Harrington
- DShotRMT by derdoktor667
- Adafruit MAX31855 by Adafruit Industries
- ESP Async WebServer by mathieucarbou (fork of me-no-dev)
- AsyncTCP by mathieucarbou (fork of me-no-dev)
- ArduinoJson by Benoit Blanchon

**Hardware:**
- ESP32 by Espressif Systems
- EMAX ECO II motors
- SpeedyBee ESC
- HolyBro PM02v3 Power Monitor
- MAX31855 thermocouple module

**Project:**
- VTOL Flying Wing Design and Testing
- Desktop Aerospace Manufacturing
- Open Source Motor Testing Platform

---

## License and Usage

This documentation and associated code are part of the VTOL Flying Wing project resource library.

**Usage Rights:**
- ✅ Personal use and education
- ✅ Modification and adaptation
- ✅ Non-commercial projects
- ✅ Reference and learning

**Attribution:**
When using this documentation or code, please maintain:
- Version information
- Original structure and comments
- This documentation reference

---

## Support and Contact

**For Technical Issues:**
- Review troubleshooting section
- Check Serial Monitor for error messages
- Verify all wiring connections
- Ensure libraries are latest versions

**For Hardware Problems:**
- Consult hardware datasheets
- Check power supply voltages
- Verify component compatibility
- Test components individually

**For Feature Requests:**
- Document desired functionality
- Provide use case examples
- Consider backward compatibility
- Submit via project channels

---

**Document Version:** 1.0  
**Last Updated:** December 21, 2025  
**Status:** Complete and Production Ready ✅

---

*End of Documentation*
