/*
 * ============================================================================
 * ESP32 DSHOT/PWM Hybrid Motor Test with Temperature & Web Interface
 * Version 2.5.2 - Web Interface Mode Selector & Additional Throttle Buttons
 * ============================================================================
 * 
 * Date: December 21, 2025
 * Project: VTOL Flying Wing Motor Thrust Test Rig
 * Hardware: EMAX ECO II 2807 1500KV, SpeedyBee 60A ESC, PM02v3, 6S Battery
 * 
 * ============================================================================
 * VERSION 2.5.2 CHANGES (December 21, 2025):
 * ============================================================================
 * 
 * WEB INTERFACE ENHANCEMENTS:
 * + Added PWM/DSHOT mode selector buttons in web interface
 * + Mode indicator badge shows current protocol (PWM or DSHOT)
 * + Added 5% and 10% throttle quick buttons
 * + Throttle buttons now: 0%, 5%, 10%, 25%, 50%, 75%, 95%
 * + Mode changes reflected in real-time across all connected clients
 * 
 * SERIAL COMMAND ADDITIONS:
 * + PWM command - Switch to PWM mode (motor must be disarmed)
 * + DSHOT command - Switch to DSHOT mode (motor must be disarmed)
 * + Mode switching properly cleans up previous protocol before initializing new
 * + Safety check: Cannot change mode while motor is armed
 * + Updated HELP command to document mode switching
 * 
 * WEBSOCKET ENHANCEMENTS:
 * + Added "mode" message type for protocol switching via web
 * + Mode status included in all sensor data broadcasts
 * + DSHOT throttle control now functional via web interface
 * 
 * ============================================================================
 * VERSION 2.5.1 CHANGES (December 21, 2025):
 * ============================================================================
 * 
 * CRITICAL SENSOR CALCULATION FIXES:
 * 
 * 1. VOLTAGE CALCULATION CORRECTED:
 *    - Old (WRONG): voltage = voltagePin * VOLTAGE_SCALE * VOLTAGE_CAL
 *    - New (CORRECT): voltage = (voltagePin / VOLTAGE_SCALE) * VOLTAGE_CAL
 *    - Reason: PM02 uses voltage divider, must divide to get battery voltage
 *    - This matches calibration from November 27, 2025 testing
 * 
 * 2. CURRENT SENSOR OFFSET REMOVED:
 *    - Old (WRONG): current = (currentPin - 1.65) * CURRENT_SCALE * CAL
 *    - New (CORRECT): current = (currentPin - 0) * CURRENT_SCALE * CAL
 *    - PM02 and ESC sensors are UNIDIRECTIONAL (0V = 0A, 3.3V = max)
 *    - NOT bidirectional sensors (which would use 1.65V center point)
 *    - 1.65V offset was causing phantom current readings
 * 
 * 3. FLOAT COMPARISON FIX:
 *    - Changed abs() to fabs() for proper float absolute value
 *    - Ensures accurate zero-threshold detection
 * 
 * 4. WEB SERVER COMPATIBILITY:
 *    - Fixed deprecated send_P() warning - now uses send() method
 *    - Compatible with mathieucarbou ESP Async WebServer v3.0+
 * 
 * IMPACT: These fixes restore accurate voltage and current readings.
 * The 1.65V offset was from v2.3 when troubleshooting ESC sensor.
 * Testing confirmed sensors are unidirectional, not bidirectional.
 * 
 * ============================================================================
 * VERSION 2.5 CHANGES (December 21, 2025):
 * ============================================================================
 * 
 * MAJOR FEATURES ADDED:
 * 
 * 1. MAX31855 TEMPERATURE MONITORING:
 *    + Real-time motor bell temperature monitoring
 *    + K-type thermocouple support (0-400°C range)
 *    + ±2°C accuracy
 *    + Ambient baseline temperature recording
 *    + Temperature rise tracking from ambient
 *    + Maximum temperature recording per session
 *    + Four-level warning system (60/75/85/95°C)
 *    + Automatic shutdown at 95°C critical temperature
 *    + Integration throughout all commands (POWER, SWEEP, STATUS, etc.)
 * 
 * 2. COMPLETE WEB INTERFACE:
 *    + WiFi Access Point mode (no router needed)
 *    + Network: "MotorTest_Rig" / Password: "test1234"
 *    + IP Address: 192.168.4.1
 *    + Mobile-responsive design (phone/tablet/computer)
 *    + Real-time WebSocket updates (1 second refresh)
 *    + Visual throttle control (slider + quick buttons: 0/25/50/75/95%)
 *    + Color-coded temperature display (Green/Orange/Red/Flashing Red)
 *    + ARM/DISARM/STOP buttons for safety
 *    + Live dual sensor monitoring (PM02 + ESC current)
 *    + ESC efficiency display (when PM02 working)
 *    + Sensor status badges (Working/Not Detected)
 *    + Auto-reconnect on disconnect
 *    + Professional visual interface
 * 
 * 3. ENHANCED COMMAND SET:
 *    + TEMP: Display detailed temperature information
 *    + All commands now include temperature monitoring
 *    + SWEEP: Temperature monitoring throughout test with abort on critical temp
 *    + STATUS: Comprehensive system status including temperature
 * 
 * 4. DUAL INTERFACE OPERATION:
 *    + Serial Monitor: Full diagnostic access via USB
 *    + Web Interface: Convenient WiFi control from any device
 *    + Both interfaces active simultaneously
 *    + No conflicts between control methods
 * 
 * 5. ENHANCED SAFETY SYSTEMS:
 *    + Temperature-based auto-stop at 95°C
 *    + Color-coded visual warnings in web interface
 *    + ARM/DISARM system prevents accidental starts
 *    + Emergency stop always available (both interfaces)
 *    + Sensor status monitoring and alerts
 * 
 * RATIONALE FOR TEMPERATURE MONITORING:
 * + ESC current sensor readings found unreliable/inconsistent
 * + Temperature provides ground truth for motor stress
 * + Essential safety indicator for motor protection
 * + Enables validation of current sensor accuracy
 * + Professional motor testing standard
 * 
 * RATIONALE FOR WEB INTERFACE:
 * + Safe distance operation (away from spinning propeller)
 * + Visual feedback (color-coded warnings)
 * + Mobile-friendly (use phone/tablet)
 * + No USB cable required
 * + Easy to use (no commands to remember)
 * + Multiple simultaneous viewers
 * 
 * ============================================================================
 * VERSION 2.4 (Previous - Automated Sweep):
 * ============================================================================
 * + Automated thrust sweep function (SWEEP command)
 * + Data storage for up to 20 thrust test points
 * + Automatic efficiency calculation (g/W)
 * + Results table with statistics
 * + CSV export function (EXPORT command)
 * 
 * ============================================================================
 * VERSION 2.3 (Previous - ESC Sensor Fix):
 * ============================================================================
 * + Enabled ESC current sensor (GPIO 32)
 * + Added 1.65V offset compensation (LATER FOUND TO BE INCORRECT - see v2.5.1)
 * + Dual sensor support (PM02 + ESC)
 * 
 * NOTE: The 1.65V offset was based on assumption of bidirectional sensors.
 * Testing in v2.5.1 confirmed sensors are unidirectional (0V = 0A).
 * 
 * ============================================================================
 * VERSION 2.2 (Previous - ESC Arming):
 * ============================================================================
 * + Proper ESC arming sequence
 * + Continuous DSHOT stream (1kHz)
 * + DSHOT600/300/150 auto-detection
 * 
 * ============================================================================
 * HARDWARE CONFIGURATION:
 * ============================================================================
 * 
 * MOTOR CONTROL:
 * - GPIO 25: ESC Signal Output (PWM/DSHOT300)
 * 
 * ANALOG SENSORS:
 * - GPIO 34: PM02 Voltage Sensor (Working)
 * - GPIO 35: PM02 Current Sensor (Status: Not working - investigating)
 * - GPIO 32: ESC Current Sensor (Working - Use for motor testing!)
 * 
 * TEMPERATURE (MAX31855 SPI):
 * - GPIO 18: MAX31855 SCK (SPI Clock)
 * - GPIO 5:  MAX31855 CS (Chip Select)
 * - GPIO 19: MAX31855 SO (MISO/Data Out)
 * - 3.3V:    MAX31855 VCC
 * - GND:     MAX31855 GND
 * 
 * POWER:
 * - USB: ESP32 5V Power (isolated from battery)
 * - GND: Common Ground (PM02 + ESC + ESP32 + MAX31855) **CRITICAL**
 * 
 * REQUIRED LIBRARIES:
 * - ESP32Servo by Kevin Harrington (for PWM mode)
 * - DShotRMT by derdoktor667 v0.9.5+ (for DSHOT mode)
 * - Adafruit MAX31855 library (for temperature)
 * - ESPAsyncWebServer by me-no-dev (for web interface)
 * - AsyncTCP by me-no-dev (dependency for ESPAsyncWebServer)
 * - ArduinoJson by Benoit Blanchon v6.x (for JSON)
 * 
 * INSTALLATION:
 * Arduino IDE → Tools → Manage Libraries → Install all libraries above
 * 
 * ============================================================================
 * WEB INTERFACE ACCESS:
 * ============================================================================
 * 
 * WiFi Network: MotorTest_Rig
 * Password: test1234
 * URL: http://192.168.4.1
 * 
 * 1. Connect to WiFi network "MotorTest_Rig"
 * 2. Enter password: test1234
 * 3. Open browser, navigate to: http://192.168.4.1
 * 4. Web interface loads automatically
 * 
 * ============================================================================
 */

// ============================================================================
// LIBRARY INCLUDES
// ============================================================================

#include <ESP32Servo.h>
#include <DShotRMT.h>
#include <Adafruit_MAX31855.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>

// ============================================================================
// PIN DEFINITIONS
// ============================================================================

// Motor Control
#define MOTOR_PIN          25    // GPIO 25: ESC Signal (PWM or DSHOT)

// Analog Sensors
#define ESC_CURRENT_PIN    32    // GPIO 32: ESC Current Sensor (ADC1_CH4) - WORKING!
#define VOLTAGE_PIN        34    // GPIO 34: PM02 Voltage Sensor (ADC1_CH6) - Working
#define CURRENT_PIN        35    // GPIO 35: PM02 Current Sensor (ADC1_CH7) - NOT working

// Temperature (MAX31855 SPI)
#define MAXDO              19    // GPIO 19: MAX31855 Data Out (MISO/SO)
#define MAXCS              5     // GPIO 5:  MAX31855 Chip Select
#define MAXCLK             18    // GPIO 18: MAX31855 Clock (SCK)

// ============================================================================
// CALIBRATION CONSTANTS
// ============================================================================

// PM02 Voltage calibration
#define VOLTAGE_SCALE      0.0660    // PM02 scaling (V/V)
#define VOLTAGE_CAL        1.3811    // Calibrated value (Nov 27, 2025)

// PM02 Current calibration (NOT WORKING - sensor output 0V)
#define CURRENT_SCALE      36.3600   // 3.3V = 120A (PM02 spec)
#define CURRENT_CAL        1.0000    // User adjustable
#define CURRENT_OFFSET     0.0000    // User adjustable

// ESC Current calibration (WORKING - Use this for motor testing!)
#define ESC_CURRENT_SCALE  121.2100  // 3.3V = 400A (SpeedyBee spec, Scale=400)
#define ESC_CURRENT_CAL    1.0000    // **ENABLED** - Sensor verified working
#define ESC_CURRENT_OFFSET 0.0000    // User adjustable

// ADC Resolution
#define ADC_MAX_12BIT 4095.0000

// Sensor
#define SENSOR_VOLTAGE_MAX 3.3000

#define CURRENT_ZERO_THRESHOLD 0.0001  // Noise floor
#define CURRENT_DISPLAY_THRESHOLD 0.01  // Minimum for efficiency calc

// Battery parameters
#define BATTERY_CELLS      6
#define BATTERY_CAPACITY   6000      // mAh

// Temperature thresholds (°C)
#define TEMP_WARN_LOW      60.0   // Warm warning
#define TEMP_WARN_MED      75.0   // Hot warning
#define TEMP_WARN_HIGH     85.0   // Very hot warning
#define TEMP_CRITICAL      95.0   // Critical - auto stop

// ============================================================================
// PROTOCOL CONFIGURATION
// ============================================================================

enum ProtocolMode {
  MODE_PWM,
  MODE_DSHOT
};

ProtocolMode currentMode = MODE_PWM;

// PWM Configuration
#define PWM_MIN            1000
#define PWM_MAX            2000
#define PWM_FREQUENCY      50

// DSHOT Configuration
#define DSHOT_MODE         DSHOT300  // DSHOT300 works with BlueJay firmware
#define DSHOT_UPDATE_HZ    1000      // 1kHz throttle updates

// DSHOT Commands
#define DSHOT_CMD_MOTOR_STOP         0
#define DSHOT_CMD_BEEP1              1
#define DSHOT_CMD_BEEP2              2
#define DSHOT_CMD_BEEP3              3
#define DSHOT_CMD_BEEP4              4
#define DSHOT_CMD_BEEP5              5
#define DSHOT_CMD_ESC_INFO           6
#define DSHOT_CMD_SPIN_DIRECTION_1   7
#define DSHOT_CMD_SPIN_DIRECTION_2   8
#define DSHOT_CMD_SAVE_SETTINGS      12
#define DSHOT_CMD_SPIN_NORMAL        20
#define DSHOT_CMD_SPIN_REVERSED      21

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================

const char* ssid = "MotorTest_Rig";
const char* password = "test1234";

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

Servo esc;
DShotRMT* dshotMotor = nullptr;
TaskHandle_t dshotTaskHandle = NULL;

// Temperature sensor
Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);

// Web server
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Motor control
bool armed = false;
int currentThrottle = 0;
bool dshotUpdateEnabled = false;

// Power measurements
float voltage = 0.0000;
float current = 0.0000;        // PM02 current (not working)
float power = 0.0000;
float capacityUsed = 0.0000;
unsigned long lastUpdate = 0;

// ESC current sensor (WORKING)
float escCurrent = 0.0000;
float escPower = 0.0000;

// Temperature measurements
float motorTemp = 0.0;
float ambientTemp = 0.0;
float tempRise = 0.0;
float maxTemp = 0.0;
bool tempSensorWorking = false;

// Sensor status flags
bool voltageWorking = false;
bool pm02CurrentWorking = false;
bool escCurrentWorking = false;

// ============================================================================
// THRUST SWEEP DATA STORAGE
// ============================================================================

struct ThrustDataPoint {
  int throttle;
  float voltage;
  float current;
  float power;
  float motorTemp;
  int thrust_g;
  float efficiency;
};

#define MAX_DATA_POINTS 20
ThrustDataPoint thrustData[MAX_DATA_POINTS];
int dataPointCount = 0;

// ============================================================================
// HTML WEB INTERFACE
// ============================================================================

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Motor Test Rig</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
      color: #fff;
      padding: 20px;
      min-height: 100vh;
    }
    .container {
      max-width: 800px;
      margin: 0 auto;
      background: rgba(255, 255, 255, 0.95);
      border-radius: 15px;
      padding: 30px;
      box-shadow: 0 10px 40px rgba(0,0,0,0.3);
      color: #333;
    }
    h1 {
      text-align: center;
      color: #2a5298;
      margin-bottom: 10px;
      font-size: 28px;
    }
    .subtitle {
      text-align: center;
      color: #666;
      margin-bottom: 25px;
      font-size: 14px;
    }
    .status-bar {
      background: #f8f9fa;
      border: 2px solid #dee2e6;
      border-radius: 8px;
      padding: 15px;
      margin-bottom: 20px;
      text-align: center;
    }
    .status-indicator {
      display: inline-block;
      padding: 8px 16px;
      border-radius: 20px;
      font-weight: bold;
      font-size: 14px;
    }
    .status-connected { background: #d4edda; color: #155724; }
    .status-disconnected { background: #f8d7da; color: #721c24; }
    .control-panel {
      background: #f8f9fa;
      border-radius: 10px;
      padding: 20px;
      margin-bottom: 20px;
    }
    .mode-selector {
      display: flex;
      gap: 10px;
      margin-bottom: 15px;
      justify-content: center;
    }
    .mode-button {
      flex: 1;
      max-width: 150px;
      padding: 10px 20px;
      border: 2px solid #6c757d;
      border-radius: 8px;
      background: white;
      color: #6c757d;
      font-size: 14px;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s;
    }
    .mode-button.active {
      background: #6c757d;
      color: white;
      border-color: #6c757d;
    }
    .mode-button:hover {
      transform: translateY(-2px);
      box-shadow: 0 4px 8px rgba(0,0,0,0.2);
    }
    .button-group {
      display: flex;
      gap: 10px;
      margin-bottom: 15px;
      flex-wrap: wrap;
    }
    button {
      flex: 1;
      min-width: 100px;
      padding: 12px 20px;
      border: none;
      border-radius: 8px;
      font-size: 16px;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s;
    }
    button:hover { transform: translateY(-2px); box-shadow: 0 4px 8px rgba(0,0,0,0.2); }
    button:active { transform: translateY(0); }
    .btn-arm { background: #28a745; color: white; }
    .btn-arm:hover { background: #218838; }
    .btn-disarm { background: #6c757d; color: white; }
    .btn-disarm:hover { background: #5a6268; }
    .btn-stop { background: #dc3545; color: white; font-size: 18px; }
    .btn-stop:hover { background: #c82333; }
    .btn-throttle { background: #007bff; color: white; font-size: 14px; }
    .btn-throttle:hover { background: #0056b3; }
    .slider-container {
      margin: 20px 0;
    }
    .slider-label {
      display: flex;
      justify-content: space-between;
      margin-bottom: 8px;
      font-weight: bold;
      color: #495057;
    }
    input[type="range"] {
      width: 100%;
      height: 8px;
      border-radius: 5px;
      background: #dee2e6;
      outline: none;
      -webkit-appearance: none;
    }
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      width: 24px;
      height: 24px;
      border-radius: 50%;
      background: #007bff;
      cursor: pointer;
    }
    input[type="range"]:disabled {
      opacity: 0.5;
      cursor: not-allowed;
    }
    .data-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
      gap: 15px;
      margin-bottom: 20px;
    }
    .data-card {
      background: white;
      border: 2px solid #dee2e6;
      border-radius: 8px;
      padding: 15px;
      text-align: center;
    }
    .data-label {
      font-size: 12px;
      color: #6c757d;
      text-transform: uppercase;
      letter-spacing: 0.5px;
      margin-bottom: 5px;
    }
    .data-value {
      font-size: 24px;
      font-weight: bold;
      color: #212529;
    }
    .data-unit {
      font-size: 14px;
      color: #6c757d;
      margin-left: 2px;
    }
    .temp-display {
      background: white;
      border-radius: 10px;
      padding: 20px;
      margin-bottom: 20px;
      text-align: center;
    }
    .temp-value {
      font-size: 48px;
      font-weight: bold;
      margin: 10px 0;
    }
    .temp-normal { color: #28a745; }
    .temp-warm { color: #ffc107; }
    .temp-hot { color: #fd7e14; }
    .temp-critical { color: #dc3545; animation: blink 1s infinite; }
    @keyframes blink {
      0%, 50% { opacity: 1; }
      51%, 100% { opacity: 0.3; }
    }
    .temp-status {
      font-size: 16px;
      margin-top: 10px;
      font-weight: bold;
    }
    .sensor-badges {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
      margin-top: 15px;
    }
    .badge {
      display: inline-block;
      padding: 6px 12px;
      border-radius: 15px;
      font-size: 12px;
      font-weight: bold;
    }
    .badge-success { background: #d4edda; color: #155724; }
    .badge-danger { background: #f8d7da; color: #721c24; }
    .badge-info { background: #d1ecf1; color: #0c5460; }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚁 Motor Test Rig</h1>
    <div class="subtitle">VTOL Flying Wing - Real-time Monitoring</div>
    
    <div class="status-bar">
      <span id="connStatus" class="status-indicator status-disconnected">⌛ Connecting...</span>
    </div>

    <div class="temp-display">
      <div class="data-label">Motor Temperature</div>
      <div id="tempValue" class="temp-value temp-normal">--°C</div>
      <div id="tempStatus" class="temp-status">Initializing...</div>
      <div class="sensor-badges">
        <span class="badge badge-success">Ambient: <span id="ambientTemp">--</span>°C</span>
        <span class="badge badge-success">Rise: <span id="tempRise">--</span>°C</span>
        <span class="badge badge-success">Max: <span id="maxTemp">--</span>°C</span>
      </div>
    </div>

    <div class="control-panel">
      <div class="mode-selector">
        <button class="mode-button active" id="modePWM" onclick="setMode('PWM')">PWM Mode</button>
        <button class="mode-button" id="modeDSHOT" onclick="setMode('DSHOT')">DSHOT Mode</button>
      </div>
      
      <div class="sensor-badges" style="justify-content: center; margin-bottom: 15px;">
        <span id="modeIndicator" class="badge badge-info">Mode: PWM</span>
      </div>
      
      <div class="button-group">
        <button class="btn-arm" onclick="sendCommand('ARM')">⚡ ARM</button>
        <button class="btn-disarm" onclick="sendCommand('DISARM')">🛑 DISARM</button>
      </div>
      <button class="btn-stop" onclick="sendCommand('STOP')">🚨 EMERGENCY STOP</button>
      
      <div class="slider-container">
        <div class="slider-label">
          <span>Throttle</span>
          <span id="throttleDisplay">0%</span>
        </div>
        <input type="range" id="throttleSlider" min="0" max="100" value="0" 
               oninput="updateThrottle(this.value)" disabled>
      </div>
      
      <div class="button-group">
        <button class="btn-throttle" onclick="setThrottle(0)">0%</button>
        <button class="btn-throttle" onclick="setThrottle(5)">5%</button>
        <button class="btn-throttle" onclick="setThrottle(10)">10%</button>
        <button class="btn-throttle" onclick="setThrottle(25)">25%</button>
        <button class="btn-throttle" onclick="setThrottle(50)">50%</button>
        <button class="btn-throttle" onclick="setThrottle(75)">75%</button>
        <button class="btn-throttle" onclick="setThrottle(95)">95%</button>
      </div>
    </div>

    <div class="data-grid">
      <div class="data-card">
        <div class="data-label">Battery Voltage</div>
        <div class="data-value"><span id="voltage">--</span><span class="data-unit">V</span></div>
      </div>
      <div class="data-card">
        <div class="data-label">PM02 Current</div>
        <div class="data-value"><span id="pm02Current">--</span><span class="data-unit">A</span></div>
        <span id="pm02Status" class="badge badge-danger">Not Detected</span>
      </div>
      <div class="data-card">
        <div class="data-label">ESC Current</div>
        <div class="data-value"><span id="escCurrent">--</span><span class="data-unit">A</span></div>
        <span class="badge badge-success">Active</span>
      </div>
      <div class="data-card">
        <div class="data-label">Power</div>
        <div class="data-value"><span id="power">--</span><span class="data-unit">W</span></div>
      </div>
      <div class="data-card">
        <div class="data-label">Capacity Used</div>
        <div class="data-value"><span id="capacity">--</span><span class="data-unit">mAh</span></div>
      </div>
      <div class="data-card">
        <div class="data-label">ESC Efficiency</div>
        <div class="data-value"><span id="efficiency">--</span><span class="data-unit">%</span></div>
      </div>
    </div>
  </div>

  <script>
    var gateway = `ws://${window.location.hostname}/ws`;
    var websocket;
    var isArmed = false;
    var currentMode = 'PWM';

    function initWebSocket() {
      console.log('Connecting to WebSocket...');
      websocket = new WebSocket(gateway);
      websocket.onopen = onOpen;
      websocket.onclose = onClose;
      websocket.onmessage = onMessage;
    }

    function onOpen(event) {
      console.log('WebSocket Connected');
      document.getElementById('connStatus').textContent = '✅ Connected to Motor Test Rig';
      document.getElementById('connStatus').className = 'status-indicator status-connected';
    }

    function onClose(event) {
      console.log('WebSocket Disconnected');
      document.getElementById('connStatus').textContent = '❌ Disconnected - Reconnecting...';
      document.getElementById('connStatus').className = 'status-indicator status-disconnected';
      setTimeout(initWebSocket, 2000);
    }

    function onMessage(event) {
      var data = JSON.parse(event.data);
      
      // Update armed status
      isArmed = data.armed;
      document.getElementById('throttleSlider').disabled = !isArmed;
      
      // Update mode
      if (data.mode !== undefined) {
        currentMode = data.mode;
        updateModeDisplay(data.mode);
      }
      
      // Update throttle display
      document.getElementById('throttleDisplay').textContent = data.throttle + '%';
      document.getElementById('throttleSlider').value = data.throttle;
      
      // Update voltage and current
      document.getElementById('voltage').textContent = data.voltage.toFixed(2);
      document.getElementById('pm02Current').textContent = data.pm02Current.toFixed(2);
      document.getElementById('escCurrent').textContent = data.escCurrent.toFixed(2);
      document.getElementById('power').textContent = data.escPower.toFixed(1);
      document.getElementById('capacity').textContent = data.capacity.toFixed(0);
      
      // Update PM02 status badge
      if (data.pm02Working) {
        document.getElementById('pm02Status').textContent = 'Working';
        document.getElementById('pm02Status').className = 'badge badge-success';
      } else {
        document.getElementById('pm02Status').textContent = 'Not Detected';
        document.getElementById('pm02Status').className = 'badge badge-danger';
      }
      
      // Update ESC efficiency
      if (data.escEfficiency > 0) {
        document.getElementById('efficiency').textContent = data.escEfficiency.toFixed(1);
      } else {
        document.getElementById('efficiency').textContent = '--';
      }
      
      // Update temperature
      updateTemperature(data.motorTemp, data.ambientTemp, data.tempRise, data.maxTemp);
    }

    function updateTemperature(motor, ambient, rise, max) {
      var tempElement = document.getElementById('tempValue');
      var statusElement = document.getElementById('tempStatus');
      
      if (motor > 0) {
        tempElement.textContent = motor.toFixed(1) + '°C';
        document.getElementById('ambientTemp').textContent = ambient.toFixed(1);
        document.getElementById('tempRise').textContent = rise.toFixed(1);
        document.getElementById('maxTemp').textContent = max.toFixed(1);
        
        // Update color and status based on temperature
        tempElement.className = 'temp-value';
        if (motor >= 95) {
          tempElement.classList.add('temp-critical');
          statusElement.textContent = '🚨 CRITICAL! STOP MOTOR IMMEDIATELY!';
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
          statusElement.textContent = 'ℹ️ Motor warming up (normal)';
          statusElement.style.color = '#ffc107';
        } else {
          tempElement.classList.add('temp-normal');
          statusElement.textContent = '✅ Temperature normal';
          statusElement.style.color = '#28a745';
        }
      } else {
        tempElement.textContent = '--°C';
        statusElement.textContent = 'Reading temperature...';
        statusElement.style.color = '#6c757d';
      }
    }

    function updateModeDisplay(mode) {
      currentMode = mode;
      document.getElementById('modeIndicator').textContent = 'Mode: ' + mode;
      
      // Update button states
      if (mode === 'PWM') {
        document.getElementById('modePWM').classList.add('active');
        document.getElementById('modeDSHOT').classList.remove('active');
      } else {
        document.getElementById('modePWM').classList.remove('active');
        document.getElementById('modeDSHOT').classList.add('active');
      }
    }

    function setMode(mode) {
      if (websocket.readyState === WebSocket.OPEN) {
        websocket.send(JSON.stringify({type: 'mode', value: mode}));
      }
    }

    function sendCommand(cmd) {
      if (websocket.readyState === WebSocket.OPEN) {
        websocket.send(JSON.stringify({type: 'command', value: cmd}));
      }
    }

    function updateThrottle(value) {
      document.getElementById('throttleDisplay').textContent = value + '%';
      if (isArmed && websocket.readyState === WebSocket.OPEN) {
        websocket.send(JSON.stringify({type: 'throttle', value: parseInt(value)}));
      }
    }

    function setThrottle(value) {
      document.getElementById('throttleSlider').value = value;
      updateThrottle(value);
    }

    window.addEventListener('load', initWebSocket);
  </script>
</body>
</html>
)rawliteral";

// ============================================================================
// DSHOT CONTINUOUS UPDATE TASK
// ============================================================================

void dshotUpdateTask(void *parameter) {
  const TickType_t xFrequency = pdMS_TO_TICKS(1000 / DSHOT_UPDATE_HZ);
  TickType_t xLastWakeTime = xTaskGetTickCount();
  
  while (dshotUpdateEnabled) {
    if (dshotMotor != nullptr && currentMode == MODE_DSHOT) {
      float throttlePercent = (float)currentThrottle;
      dshotMotor->sendThrottlePercent(throttlePercent);
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
  
  vTaskDelete(NULL);
}

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

void setupWiFi();
void setupWebServer();
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len);
void broadcastSensorData();
void updateSensors();
void updateTemperature();
String getTemperatureStatus();
void checkCriticalTemperature();
void displayTemperature();
void displayPowerData();
void performThrustSweep();
void printThrustResults();
void exportToCSV();
void processCommand(String command);

// ============================================================================
// SETUP
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("+========================================================+");
  Serial.println("|  ESP32 MOTOR TEST - v2.5                              |");
  Serial.println("|  Temperature Monitoring + Web Interface               |");
  Serial.println("+========================================================+");
  Serial.println();
  
  // Initialize temperature sensor
  Serial.print("[INIT] MAX31855 Thermocouple... ");
  if (!isnan(thermocouple.readCelsius())) {
    tempSensorWorking = true;
    ambientTemp = thermocouple.readCelsius();
    maxTemp = ambientTemp;
    Serial.println("OK");
    Serial.print("       Ambient temperature: ");
    Serial.print(ambientTemp, 1);
    Serial.println("°C");
  } else {
    tempSensorWorking = false;
    Serial.println("FAILED!");
    Serial.println("       [WARNING] Temperature sensor not detected");
    Serial.println("       Motor will run without temperature monitoring!");
  }
  Serial.println();
  
  // Initialize WiFi
  setupWiFi();
  
  // Initialize web server
  setupWebServer();
  
  // Initialize motor control (PWM mode by default)
  Serial.print("[INIT] Motor Control (PWM mode)... ");
  esc.setPeriodHertz(PWM_FREQUENCY);
  esc.attach(MOTOR_PIN, PWM_MIN, PWM_MAX);
  esc.writeMicroseconds(PWM_MIN);
  Serial.println("OK");
  Serial.println();
  
  // Sensor initialization
  Serial.println("[INIT] Analog Sensors:");
  analogReadResolution(12);
  
  delay(500);
  updateSensors();
  
  Serial.print("       Voltage Sensor (GPIO 34): ");
  Serial.print(voltage, 2);
  Serial.print("V ");
  voltageWorking = (voltage > 10.0);
  Serial.println(voltageWorking ? "✅ Working" : "❌ Check connection");
  
  Serial.print("       PM02 Current (GPIO 35): ");
  Serial.print(current, 2);
  Serial.print("A ");
  pm02CurrentWorking = (abs(current) > 0.1);
  Serial.println(pm02CurrentWorking ? "✅ Working" : "⚠️ Not detected");
  
  Serial.print("       ESC Current (GPIO 32): ");
  Serial.print(escCurrent, 2);
  Serial.print("A ");
  escCurrentWorking = true;  // ESC sensor always enabled
  Serial.println("✅ Active");
  
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println("|  SYSTEM READY                                          |");
  Serial.println("+========================================================+");
  Serial.println();
  Serial.println("Web Interface: http://192.168.4.1");
  Serial.println("WiFi Network: MotorTest_Rig");
  Serial.println("Password: test1234");
  Serial.println();
  Serial.println("Serial Commands: ARM, DISARM, STOP, T<0-100>, POWER,");
  Serial.println("                 TEMP, SWEEP, EXPORT, STATUS, HELP");
  Serial.println();
  Serial.println("Type HELP for command reference");
  Serial.println();
}

// ============================================================================
// WIFI SETUP
// ============================================================================

void setupWiFi() {
  Serial.println("[WIFI] Starting Access Point...");
  Serial.print("       SSID: ");
  Serial.println(ssid);
  Serial.print("       Password: ");
  Serial.println(password);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("       AP IP address: ");
  Serial.println(IP);
  Serial.println("       [OK] WiFi Access Point active");
  Serial.println();
}

// ============================================================================
// WEB SERVER SETUP
// ============================================================================

void setupWebServer() {
  // WebSocket handler
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  
  // Serve main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });
  
  server.begin();
  Serial.println("[WEB] Web server started on port 80");
  Serial.println();
}

// ============================================================================
// WEBSOCKET EVENT HANDLER
// ============================================================================

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  
  if (type == WS_EVT_CONNECT) {
    Serial.print("[WEB] Client connected: ");
    Serial.println(client->id());
    
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.print("[WEB] Client disconnected: ");
    Serial.println(client->id());
    
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      String message = (char*)data;
      
      // Parse JSON command
      DynamicJsonDocument doc(256);
      DeserializationError error = deserializeJson(doc, message);
      
      if (!error) {
        String type = doc["type"];
        
        if (type == "command") {
          String cmd = doc["value"];
          processCommand(cmd);
          
        } else if (type == "throttle") {
          int throttle = doc["value"];
          if (armed && throttle >= 0 && throttle <= 100) {
            currentThrottle = throttle;
            if (currentMode == MODE_PWM) {
              int pulseWidth = map(throttle, 0, 100, PWM_MIN, PWM_MAX);
              esc.writeMicroseconds(pulseWidth);
            }
            // DSHOT updates continuously via task, just set currentThrottle
          }
          
        } else if (type == "mode") {
          String modeStr = doc["value"];
          if (modeStr == "PWM") {
            processCommand("PWM");
          } else if (modeStr == "DSHOT") {
            processCommand("DSHOT");
          }
        }
      }
    }
  }
}

// ============================================================================
// BROADCAST SENSOR DATA
// ============================================================================

void broadcastSensorData() {
  updateSensors();
  updateTemperature();
  
  DynamicJsonDocument doc(512);
  
  doc["armed"] = armed;
  doc["throttle"] = currentThrottle;
  doc["mode"] = (currentMode == MODE_PWM) ? "PWM" : "DSHOT";
  doc["voltage"] = voltage;
  doc["pm02Current"] = current;
  doc["pm02Working"] = pm02CurrentWorking;
  doc["escCurrent"] = escCurrent;
  doc["escPower"] = escPower;
  doc["capacity"] = capacityUsed;
  doc["motorTemp"] = motorTemp;
  doc["ambientTemp"] = ambientTemp;
  doc["tempRise"] = tempRise;
  doc["maxTemp"] = maxTemp;
  
  // Calculate ESC efficiency if both sensors working
  float escEfficiency = 0.0;
  if (pm02CurrentWorking && escPower > 1.0 && power > 1.0) {
    escEfficiency = (escPower / power) * 100.0;
  }
  doc["escEfficiency"] = escEfficiency;
  
  String json;
  serializeJson(doc, json);
  ws.textAll(json);
}

// ============================================================================
// UPDATE SENSORS
// ============================================================================

void updateSensors() {
  // Read voltage (PM02)
  int voltageRaw = analogRead(VOLTAGE_PIN);
  float voltagePin = (voltageRaw / ADC_MAX_12BIT) * SENSOR_VOLTAGE_MAX;
  voltage = (voltagePin / VOLTAGE_SCALE) * VOLTAGE_CAL;
  
  // Read PM02 current (no offset - sensor outputs 0V at 0A)
  int currentRaw = analogRead(CURRENT_PIN);
  float currentPin = (currentRaw / ADC_MAX_12BIT) * SENSOR_VOLTAGE_MAX;
  current = (currentPin - 0) * CURRENT_SCALE * CURRENT_CAL + CURRENT_OFFSET;
  if (fabs(current) < CURRENT_ZERO_THRESHOLD) current = 0.0;
  
  // Read ESC current (no offset - sensor outputs 0V at 0A)
  int escCurrentRaw = analogRead(ESC_CURRENT_PIN);
  float escCurrentPin = (escCurrentRaw / ADC_MAX_12BIT) * SENSOR_VOLTAGE_MAX;
  escCurrent = (escCurrentPin - 0) * ESC_CURRENT_SCALE * ESC_CURRENT_CAL + ESC_CURRENT_OFFSET;
  if (fabs(escCurrent) < CURRENT_ZERO_THRESHOLD) escCurrent = 0.0;
  
  // Calculate power (use ESC current as it's more reliable)
  power = voltage * current;
  escPower = voltage * escCurrent;
  
  // Update capacity (mAh)
  unsigned long currentTime = millis();
  if (lastUpdate > 0) {
    float deltaTime = (currentTime - lastUpdate) / 3600000.0;  // hours
    capacityUsed += escCurrent * deltaTime * 1000.0;  // mAh
  }
  lastUpdate = currentTime;
}

// ============================================================================
// UPDATE TEMPERATURE
// ============================================================================

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

// ============================================================================
// CHECK CRITICAL TEMPERATURE
// ============================================================================

void checkCriticalTemperature() {
  if (!tempSensorWorking) return;
  
  if (motorTemp >= TEMP_CRITICAL) {
    Serial.println();
    Serial.println("+========================================================+");
    Serial.println("|  🚨 CRITICAL TEMPERATURE ALERT! 🚨                     |");
    Serial.println("+========================================================+");
    Serial.print("Motor temperature: ");
    Serial.print(motorTemp, 1);
    Serial.println("°C");
    Serial.println("AUTO-STOPPING MOTOR FOR SAFETY!");
    Serial.println("+========================================================+");
    Serial.println();
    
    armed = false;
    currentThrottle = 0;
    if (currentMode == MODE_PWM) {
      esc.writeMicroseconds(PWM_MIN);
    } else if (currentMode == MODE_DSHOT && dshotMotor != nullptr) {
      dshotMotor->sendThrottlePercent(0.0);
    }
    
    Serial.println("[SAFETY] Motor stopped due to critical temperature");
    Serial.println("         Wait for motor to cool below 50°C before restarting");
    Serial.println();
  }
}

// ============================================================================
// GET TEMPERATURE STATUS
// ============================================================================

String getTemperatureStatus() {
  if (!tempSensorWorking) return "Sensor not detected";
  
  if (motorTemp >= TEMP_CRITICAL) {
    return "🚨 CRITICAL! STOP IMMEDIATELY!";
  } else if (motorTemp >= TEMP_WARN_HIGH) {
    return "⚠️ Very hot - Reduce throttle";
  } else if (motorTemp >= TEMP_WARN_MED) {
    return "🔥 Hot - Monitor closely";
  } else if (motorTemp >= TEMP_WARN_LOW) {
    return "ℹ️ Warming up (normal)";
  } else {
    return "✅ Normal operating temperature";
  }
}

// ============================================================================
// DISPLAY TEMPERATURE
// ============================================================================

void displayTemperature() {
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println("|  TEMPERATURE STATUS                                    |");
  Serial.println("+========================================================+");
  
  if (!tempSensorWorking) {
    Serial.println();
    Serial.println("  ⚠️ Temperature sensor not detected!");
    Serial.println("  Motor running without temperature monitoring");
    Serial.println();
    Serial.println("+========================================================+");
    return;
  }
  
  updateTemperature();
  
  Serial.println();
  Serial.print("  Motor Temperature:     ");
  Serial.print(motorTemp, 1);
  Serial.println("°C");
  
  Serial.print("  Ambient Temperature:   ");
  Serial.print(ambientTemp, 1);
  Serial.println("°C");
  
  Serial.print("  Temperature Rise:      ");
  Serial.print(tempRise, 1);
  Serial.println("°C");
  
  Serial.print("  Maximum Recorded:      ");
  Serial.print(maxTemp, 1);
  Serial.println("°C");
  
  Serial.println();
  Serial.print("  Status: ");
  Serial.println(getTemperatureStatus());
  
  Serial.println();
  Serial.println("  Temperature Warnings:");
  Serial.println("  • 60°C:  Warm (normal operation)");
  Serial.println("  • 75°C:  Hot (monitor closely)");
  Serial.println("  • 85°C:  Very hot (reduce throttle)");
  Serial.println("  • 95°C:  CRITICAL (auto-stop)");
  
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println();
}

// ============================================================================
// DISPLAY POWER DATA
// ============================================================================

void displayPowerData() {
  updateSensors();
  updateTemperature();
  
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println("|  POWER & TEMPERATURE DATA                              |");
  Serial.println("+========================================================+");
  Serial.println();
  
  Serial.print("  Throttle:           ");
  Serial.print(currentThrottle);
  Serial.println("%");
  
  Serial.print("  Battery Voltage:    ");
  Serial.print(voltage, 2);
  Serial.println(" V");
  
  Serial.println();
  Serial.println("  --- PM02 Sensor (GPIO 35) ---");
  Serial.print("  Current:            ");
  Serial.print(current, 2);
  Serial.print(" A ");
  Serial.println(pm02CurrentWorking ? "✅" : "⚠️ Not working");
  
  Serial.print("  Power:              ");
  Serial.print(power, 2);
  Serial.println(" W");
  
  Serial.println();
  Serial.println("  --- ESC Sensor (GPIO 32) ---");
  Serial.print("  Current:            ");
  Serial.print(escCurrent, 2);
  Serial.println(" A ✅ Active");
  
  Serial.print("  Power:              ");
  Serial.print(escPower, 2);
  Serial.println(" W");
  
  Serial.println();
  Serial.print("  Capacity Used:      ");
  Serial.print(capacityUsed, 0);
  Serial.println(" mAh");
  
  if (pm02CurrentWorking && escPower > 1.0 && power > 1.0) {
    float escEfficiency = (escPower / power) * 100.0;
    Serial.print("  ESC Efficiency:     ");
    Serial.print(escEfficiency, 1);
    Serial.println("%");
  }
  
  // Temperature section
  Serial.println();
  Serial.println("  --- Temperature ---");
  if (tempSensorWorking) {
    Serial.print("  Motor Temp:         ");
    Serial.print(motorTemp, 1);
    Serial.println("°C");
    
    Serial.print("  Ambient Temp:       ");
    Serial.print(ambientTemp, 1);
    Serial.println("°C");
    
    Serial.print("  Temperature Rise:   ");
    Serial.print(tempRise, 1);
    Serial.println("°C");
    
    Serial.print("  Max Recorded:       ");
    Serial.print(maxTemp, 1);
    Serial.println("°C");
    
    Serial.println();
    Serial.print("  Status: ");
    Serial.println(getTemperatureStatus());
  } else {
    Serial.println("  Temperature sensor not detected");
  }
  
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println();
}

// ============================================================================
// THRUST SWEEP TEST
// ============================================================================

void performThrustSweep() {
  if (!armed) {
    Serial.println("\n[ERROR] Cannot start sweep - motor not armed!");
    Serial.println("Type ARM first, then SWEEP\n");
    return;
  }
  
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println("|  AUTOMATED THRUST SWEEP TEST                           |");
  Serial.println("+========================================================+");
  Serial.println();
  Serial.println("This test will run the motor at:");
  Serial.println("  • 5%, 10%, 25%, 50%, 75%, 95% throttle");
  Serial.println("  • Hold each for 2-3 seconds");
  Serial.println("  • You enter thrust reading at each step");
  Serial.println();
  
  if (tempSensorWorking) {
    Serial.println("Temperature Monitoring: ACTIVE");
    Serial.println("  • Test will abort if temperature exceeds 95°C");
    Serial.println("  • Monitor temperature closely");
    Serial.println();
  }
  
  Serial.println("SAFETY:");
  Serial.println("  • Ensure motor/propeller is secure");
  Serial.println("  • Keep hands clear");
  Serial.println("  • Type STOP at any time to abort");
  Serial.println();
  Serial.print("Ready to begin? (Y/N): ");
  
  while (!Serial.available()) delay(100);
  String confirm = Serial.readStringUntil('\n');
  confirm.trim();
  confirm.toUpperCase();
  Serial.println(confirm);
  
  if (confirm != "Y" && confirm != "YES") {
    Serial.println("\n[CANCELLED] Sweep test cancelled\n");
    return;
  }
  
  Serial.println();
  Serial.println("[OK] Starting sweep test...");
  Serial.println();
  
  // Clear previous data
  dataPointCount = 0;
  
  // Test throttle points
  int throttlePoints[] = {5, 10, 25, 50, 75, 95};
  int numPoints = 6;
  
  for (int i = 0; i < numPoints; i++) {
    int throttle = throttlePoints[i];
    
    Serial.println("+------------------------------------------------------+");
    Serial.print("  Test Point ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numPoints);
    Serial.print(": ");
    Serial.print(throttle);
    Serial.println("% Throttle");
    Serial.println("+------------------------------------------------------+");
    
    // Set throttle
    currentThrottle = throttle;
    if (currentMode == MODE_PWM) {
      int pulseWidth = map(throttle, 0, 100, PWM_MIN, PWM_MAX);
      esc.writeMicroseconds(pulseWidth);
    }
    
    // Wait for motor to stabilize
    Serial.println("Stabilizing...");
    for (int j = 0; j < 25; j++) {
      delay(100);
      updateSensors();
      updateTemperature();
      checkCriticalTemperature();
      
      // Check if motor was stopped due to critical temp
      if (!armed) {
        Serial.println("\n[ABORTED] Sweep cancelled due to critical temperature!");
        return;
      }
    }
    
    // Read and display current data
    Serial.println();
    Serial.print("Voltage:     ");
    Serial.print(voltage, 2);
    Serial.println(" V");
    
    Serial.print("ESC Current: ");
    Serial.print(escCurrent, 2);
    Serial.println(" A");
    
    Serial.print("Power:       ");
    Serial.print(escPower, 1);
    Serial.println(" W");
    
    if (tempSensorWorking) {
      Serial.print("Temperature: ");
      Serial.print(motorTemp, 1);
      Serial.print("°C (Rise: ");
      Serial.print(tempRise, 1);
      Serial.println("°C)");
    }
    
    Serial.println();
    Serial.print("Enter thrust reading (grams): ");
    
    // Wait for user input
    while (!Serial.available()) delay(100);
    String thrustStr = Serial.readStringUntil('\n');
    thrustStr.trim();
    int thrust = thrustStr.toInt();
    
    Serial.print(thrust);
    Serial.println(" g");
    
    // Validate input
    if (thrust <= 0 || thrust > 10000) {
      Serial.println();
      Serial.println("[WARNING] Invalid thrust value!");
      Serial.println("Using 0g for this data point.");
      thrust = 0;
    }
    
    // Calculate efficiency
    float efficiency = 0.0;
    if (escPower > 0.1) {
      efficiency = (float)thrust / escPower;
    }
    
    // Store data
    if (dataPointCount < MAX_DATA_POINTS) {
      thrustData[dataPointCount].throttle = throttle;
      thrustData[dataPointCount].voltage = voltage;
      thrustData[dataPointCount].current = escCurrent;
      thrustData[dataPointCount].power = escPower;
      thrustData[dataPointCount].motorTemp = tempSensorWorking ? motorTemp : 0.0;
      thrustData[dataPointCount].thrust_g = thrust;
      thrustData[dataPointCount].efficiency = efficiency;
      dataPointCount++;
    }
    
    Serial.println();
    Serial.print("[OK] Data point ");
    Serial.print(i + 1);
    Serial.println(" recorded");
    Serial.println();
    
    delay(500);
  }
  
  // Stop motor
  currentThrottle = 0;
  if (currentMode == MODE_PWM) {
    esc.writeMicroseconds(PWM_MIN);
  }
  delay(500);
  
  Serial.println();
  Serial.println("+======================================================+");
  Serial.println("|  SWEEP TEST COMPLETE!                                |");
  Serial.println("+======================================================+");
  
  if (tempSensorWorking) {
    Serial.println();
    Serial.print("Final temperature: ");
    Serial.print(motorTemp, 1);
    Serial.println("°C");
    
    if (motorTemp > 80.0) {
      Serial.println();
      Serial.println("⚠️ MOTOR HOT - COOLING RECOMMENDATIONS:");
      Serial.println("  • Wait 15-20 minutes before next test");
      Serial.println("  • Use forced air cooling if available");
      Serial.println("  • Monitor temperature before restart");
    } else if (motorTemp > 60.0) {
      Serial.println();
      Serial.println("Motor warm - Wait 5-10 minutes before next test");
    }
  }
  
  Serial.println();
  
  // Display results
  printThrustResults();
}

// ============================================================================
// PRINT THRUST RESULTS
// ============================================================================

void printThrustResults() {
  if (dataPointCount == 0) {
    Serial.println("\nNo test data available. Run SWEEP first.\n");
    return;
  }
  
  Serial.println("\n+======================================================+");
  Serial.println("|  THRUST TEST RESULTS                                 |");
  Serial.println("+======================================================+");
  Serial.println();
  
  if (tempSensorWorking) {
    Serial.println("Throttle | Voltage | Current |  Power  | Temp  | Thrust | Eff(g/W)");
    Serial.println("---------|---------|---------|---------|-------|--------|----------");
    
    for (int i = 0; i < dataPointCount; i++) {
      char line[90];
      sprintf(line, "  %3d%%   |  %5.2f  |  %6.2f |  %6.1f | %5.1f | %4dg  |  %5.2f",
              thrustData[i].throttle,
              thrustData[i].voltage,
              thrustData[i].current,
              thrustData[i].power,
              thrustData[i].motorTemp,
              thrustData[i].thrust_g,
              thrustData[i].efficiency);
      Serial.println(line);
    }
  } else {
    Serial.println("Throttle | Voltage | Current |  Power  | Thrust | Eff(g/W)");
    Serial.println("---------|---------|---------|---------|--------|----------");
    
    for (int i = 0; i < dataPointCount; i++) {
      char line[80];
      sprintf(line, "  %3d%%   |  %5.2f  |  %6.2f |  %6.1f | %4dg  |  %5.2f",
              thrustData[i].throttle,
              thrustData[i].voltage,
              thrustData[i].current,
              thrustData[i].power,
              thrustData[i].thrust_g,
              thrustData[i].efficiency);
      Serial.println(line);
    }
  }
  
  Serial.println("========================================================");
  
  // Calculate statistics
  float avgEfficiency = 0;
  float maxEfficiency = 0;
  int bestThrottle = 0;
  int totalThrust = 0;
  float totalPower = 0;
  
  for (int i = 0; i < dataPointCount; i++) {
    avgEfficiency += thrustData[i].efficiency;
    totalThrust = thrustData[i].thrust_g;
    totalPower = thrustData[i].power;
    
    if (thrustData[i].efficiency > maxEfficiency && thrustData[i].throttle > 5) {
      maxEfficiency = thrustData[i].efficiency;
      bestThrottle = thrustData[i].throttle;
    }
  }
  
  if (dataPointCount > 0) {
    avgEfficiency /= dataPointCount;
  }
  
  Serial.println();
  Serial.println("ANALYSIS:");
  Serial.println("------------------------------------------------------------");
  Serial.print("  Average Efficiency:    ");
  Serial.print(avgEfficiency, 2);
  Serial.println(" g/W");
  
  Serial.print("  Best Efficiency:       ");
  Serial.print(maxEfficiency, 2);
  Serial.print(" g/W @ ");
  Serial.print(bestThrottle);
  Serial.println("%");
  
  Serial.print("  Max Thrust (95%):      ");
  Serial.print(totalThrust);
  Serial.println(" g");
  
  Serial.print("  Max Power (95%):       ");
  Serial.print(totalPower, 1);
  Serial.println(" W");
  
  // VTOL performance
  Serial.println();
  Serial.println("VTOL PERFORMANCE (6 Motors):");
  Serial.println("------------------------------------------------------------");
  
  int totalThrustAll6 = totalThrust * 6;
  Serial.print("  Total Thrust:          ");
  Serial.print(totalThrustAll6);
  Serial.print(" g (");
  Serial.print(totalThrustAll6 / 1000.0, 2);
  Serial.println(" kg)");
  
  Serial.print("  Aircraft Weight:       5500 g (5.5 kg)");
  Serial.println();
  
  float thrustToWeight = (float)totalThrustAll6 / 5500.0;
  Serial.print("  Thrust-to-Weight:      ");
  Serial.print(thrustToWeight, 2);
  Serial.print(":1");
  
  if (thrustToWeight >= 1.5) {
    Serial.println(" [EXCELLENT]");
  } else if (thrustToWeight >= 1.2) {
    Serial.println(" [GOOD]");
  } else if (thrustToWeight >= 1.0) {
    Serial.println(" [MARGINAL]");
  } else {
    Serial.println(" [INSUFFICIENT]");
  }
  
  float hoverThrottle = (5500.0 / totalThrustAll6) * 95.0;
  Serial.print("  Est. Hover Throttle:   ");
  Serial.print(hoverThrottle, 1);
  Serial.println("%");
  
  Serial.println();
  Serial.println("+======================================================+");
  Serial.println("Type EXPORT to save data as CSV");
  Serial.println("+======================================================+\n");
}

// ============================================================================
// EXPORT TO CSV
// ============================================================================

void exportToCSV() {
  if (dataPointCount == 0) {
    Serial.println("\nNo test data available. Run SWEEP first.\n");
    return;
  }
  
  Serial.println("\n+======================================================+");
  Serial.println("|  CSV DATA EXPORT                                     |");
  Serial.println("+======================================================+");
  Serial.println();
  Serial.println("Copy the data below and paste into Excel/Google Sheets:");
  Serial.println();
  Serial.println("--- START CSV ---");
  
  if (tempSensorWorking) {
    Serial.println("Throttle(%),Voltage(V),Current(A),Power(W),Temp(C),Thrust(g),Efficiency(g/W)");
    
    for (int i = 0; i < dataPointCount; i++) {
      Serial.print(thrustData[i].throttle);
      Serial.print(",");
      Serial.print(thrustData[i].voltage, 2);
      Serial.print(",");
      Serial.print(thrustData[i].current, 2);
      Serial.print(",");
      Serial.print(thrustData[i].power, 2);
      Serial.print(",");
      Serial.print(thrustData[i].motorTemp, 1);
      Serial.print(",");
      Serial.print(thrustData[i].thrust_g);
      Serial.print(",");
      Serial.println(thrustData[i].efficiency, 3);
    }
  } else {
    Serial.println("Throttle(%),Voltage(V),Current(A),Power(W),Thrust(g),Efficiency(g/W)");
    
    for (int i = 0; i < dataPointCount; i++) {
      Serial.print(thrustData[i].throttle);
      Serial.print(",");
      Serial.print(thrustData[i].voltage, 2);
      Serial.print(",");
      Serial.print(thrustData[i].current, 2);
      Serial.print(",");
      Serial.print(thrustData[i].power, 2);
      Serial.print(",");
      Serial.print(thrustData[i].thrust_g);
      Serial.print(",");
      Serial.println(thrustData[i].efficiency, 3);
    }
  }
  
  Serial.println("--- END CSV ---");
  Serial.println();
  Serial.println("Instructions:");
  Serial.println("  1. Select all text from START to END (including headers)");
  Serial.println("  2. Copy (Ctrl+C)");
  Serial.println("  3. Paste into spreadsheet");
  Serial.println("  4. Create graphs: Thrust vs Throttle, Efficiency vs Throttle");
  Serial.println();
  Serial.println("+======================================================+\n");
}

// ============================================================================
// PROCESS COMMAND
// ============================================================================

void processCommand(String command) {
  command.trim();
  command.toUpperCase();
  
  if (command == "ARM") {
    if (!armed) {
      armed = true;
      Serial.println("\n[OK] Motor ARMED - Ready for throttle control");
      Serial.println("     Use throttle slider or T<0-100> command\n");
    } else {
      Serial.println("\n[INFO] Motor already armed\n");
    }
    
  } else if (command == "DISARM") {
    armed = false;
    currentThrottle = 0;
    if (currentMode == MODE_PWM) {
      esc.writeMicroseconds(PWM_MIN);
    } else if (currentMode == MODE_DSHOT && dshotMotor != nullptr) {
      dshotMotor->sendThrottlePercent(0.0);
    }
    Serial.println("\n[OK] Motor DISARMED\n");
    
  } else if (command == "STOP" || command == "EMERGENCY") {
    armed = false;
    currentThrottle = 0;
    if (currentMode == MODE_PWM) {
      esc.writeMicroseconds(PWM_MIN);
    } else if (currentMode == MODE_DSHOT && dshotMotor != nullptr) {
      dshotMotor->sendThrottlePercent(0.0);
    }
    Serial.println("\n[STOP] EMERGENCY STOP - Motor disarmed\n");
    
  } else if (command.startsWith("T")) {
    if (!armed) {
      Serial.println("\n[ERROR] Motor not armed! Type ARM first.\n");
      return;
    }
    
    int throttle = command.substring(1).toInt();
    if (throttle < 0 || throttle > 100) {
      Serial.println("\n[ERROR] Throttle must be 0-100%\n");
      return;
    }
    
    currentThrottle = throttle;
    if (currentMode == MODE_PWM) {
      int pulseWidth = map(throttle, 0, 100, PWM_MIN, PWM_MAX);
      esc.writeMicroseconds(pulseWidth);
    }
    // DSHOT updates continuously via task, just set currentThrottle
    
    Serial.print("\n[OK] Throttle set to ");
    Serial.print(throttle);
    Serial.println("%\n");
    
  } else if (command == "POWER" || command == "P") {
    displayPowerData();
    
  } else if (command == "TEMP" || command == "TEMPERATURE") {
    displayTemperature();
    
  } else if (command == "SWEEP") {
    performThrustSweep();
    
  } else if (command == "EXPORT") {
    exportToCSV();
    
  } else if (command == "HELP" || command == "H" || command == "?") {
    Serial.println();
    Serial.println("+========================================================+");
    Serial.println("|  COMMAND REFERENCE                                     |");
    Serial.println("+========================================================+");
    Serial.println();
    Serial.println("MOTOR CONTROL:");
    Serial.println("  ARM           - Arm motor (enable throttle control)");
    Serial.println("  DISARM        - Disarm motor (disable throttle)");
    Serial.println("  STOP          - Emergency stop");
    Serial.println("  T<0-100>      - Set throttle (0-100%)");
    Serial.println("                  Example: T50 = 50% throttle");
    Serial.println();
    Serial.println("MODE SWITCHING:");
    Serial.println("  PWM           - Switch to PWM mode (must be disarmed)");
    Serial.println("  DSHOT         - Switch to DSHOT mode (must be disarmed)");
    Serial.println();
    Serial.println("DATA DISPLAY:");
    Serial.println("  POWER / P     - Show voltage, current, power, temperature");
    Serial.println("  TEMP          - Show detailed temperature information");
    Serial.println("  STATUS        - Show complete system status");
    Serial.println();
    Serial.println("THRUST TESTING:");
    Serial.println("  SWEEP         - Automated thrust sweep test");
    Serial.println("                  (5%, 10%, 25%, 50%, 75%, 95%)");
    Serial.println("  EXPORT        - Export test data as CSV");
    Serial.println();
    Serial.println("WEB INTERFACE:");
    Serial.println("  URL: http://192.168.4.1");
    Serial.println("  WiFi: MotorTest_Rig (Password: test1234)");
    Serial.println();
    Serial.println("TEMPERATURE WARNINGS:");
    Serial.println("  60°C  - Warm (normal)");
    Serial.println("  75°C  - Hot (monitor)");
    Serial.println("  85°C  - Very hot (reduce throttle)");
    Serial.println("  95°C  - CRITICAL (auto-stop)");
    Serial.println();
    Serial.println("+========================================================+");
    Serial.println();
    
  } else if (command == "STATUS") {
    Serial.println();
    Serial.println("+========================================================+");
    Serial.println("|  SYSTEM STATUS                                         |");
    Serial.println("+========================================================+");
    Serial.println();
    
    Serial.print("  Motor State:        ");
    Serial.println(armed ? "ARMED ⚡" : "DISARMED 🛑");
    
    Serial.print("  Current Throttle:   ");
    Serial.print(currentThrottle);
    Serial.println("%");
    
    Serial.print("  Control Mode:       ");
    Serial.println(currentMode == MODE_PWM ? "PWM" : "DSHOT");
    
    Serial.println();
    Serial.println("  --- Sensor Status ---");
    
    Serial.print("  Voltage Sensor:     ");
    Serial.println(voltageWorking ? "✅ Working" : "❌ Not detected");
    
    Serial.print("  PM02 Current:       ");
    Serial.println(pm02CurrentWorking ? "✅ Working" : "⚠️ Not detected");
    
    Serial.print("  ESC Current:        ");
    Serial.println("✅ Active");
    
    Serial.print("  Temperature:        ");
    Serial.println(tempSensorWorking ? "✅ Working" : "❌ Not detected");
    
    Serial.println();
    Serial.println("  --- Current Readings ---");
    
    updateSensors();
    updateTemperature();
    
    Serial.print("  Battery Voltage:    ");
    Serial.print(voltage, 2);
    Serial.println(" V");
    
    Serial.print("  ESC Current:        ");
    Serial.print(escCurrent, 2);
    Serial.println(" A");
    
    Serial.print("  Power:              ");
    Serial.print(escPower, 1);
    Serial.println(" W");
    
    if (tempSensorWorking) {
      Serial.print("  Motor Temperature:  ");
      Serial.print(motorTemp, 1);
      Serial.println("°C");
      
      Serial.print("  Temperature Rise:   ");
      Serial.print(tempRise, 1);
      Serial.println("°C");
      
      Serial.println();
      Serial.print("  Temp Status: ");
      Serial.println(getTemperatureStatus());
    }
    
    Serial.println();
    Serial.println("  --- Web Interface ---");
    Serial.print("  WiFi Status:        ");
    Serial.println(WiFi.softAPgetStationNum() > 0 ? "Client connected" : "No clients");
    
    Serial.print("  IP Address:         ");
    Serial.println(WiFi.softAPIP());
    
    Serial.println();
    Serial.println("+========================================================+");
    Serial.println();
    
  } else if (command == "PWM") {
    if (armed) {
      Serial.println("\n[ERROR] Cannot change mode while armed. DISARM first.\n");
      return;
    }
    
    if (currentMode == MODE_PWM) {
      Serial.println("\n[INFO] Already in PWM mode\n");
      return;
    }
    
    // Clean up DSHOT if active
    if (dshotMotor != nullptr) {
      delete dshotMotor;
      dshotMotor = nullptr;
    }
    if (dshotTaskHandle != NULL) {
      vTaskDelete(dshotTaskHandle);
      dshotTaskHandle = NULL;
    }
    dshotUpdateEnabled = false;
    
    // Initialize PWM
    currentMode = MODE_PWM;
    esc.setPeriodHertz(PWM_FREQUENCY);
    esc.attach(MOTOR_PIN, PWM_MIN, PWM_MAX);
    esc.writeMicroseconds(PWM_MIN);
    
    Serial.println("\n[OK] Switched to PWM mode");
    Serial.println("     Motor control ready\n");
    
  } else if (command == "DSHOT") {
    if (armed) {
      Serial.println("\n[ERROR] Cannot change mode while armed. DISARM first.\n");
      return;
    }
    
    if (currentMode == MODE_DSHOT) {
      Serial.println("\n[INFO] Already in DSHOT mode\n");
      return;
    }
    
    // Detach PWM
    esc.detach();
    
    // Initialize DSHOT
    Serial.println("\n[INIT] Switching to DSHOT mode...");
    dshotMotor = new DShotRMT(MOTOR_PIN, DSHOT_MODE);
    
    dshot_result_t result = dshotMotor->begin();
    if (result.success) {
      currentMode = MODE_DSHOT;
      
      // Start continuous DSHOT update task
      dshotUpdateEnabled = true;
      xTaskCreatePinnedToCore(
        dshotUpdateTask,
        "DSHOT_Update",
        2048,
        NULL,
        1,
        &dshotTaskHandle,
        1
      );
      
      Serial.println("[OK] DSHOT300 initialized successfully");
      Serial.println("     Continuous throttle stream active");
      Serial.println("     Motor control ready\n");
    } else {
      Serial.println("[ERROR] DSHOT initialization failed!");
      Serial.println("        Reverting to PWM mode\n");
      delete dshotMotor;
      dshotMotor = nullptr;
      
      // Fall back to PWM
      currentMode = MODE_PWM;
      esc.setPeriodHertz(PWM_FREQUENCY);
      esc.attach(MOTOR_PIN, PWM_MIN, PWM_MAX);
      esc.writeMicroseconds(PWM_MIN);
    }
    
  } else if (command.length() > 0) {
    Serial.println("\n[ERROR] Unknown command. Type HELP for command list.\n");
  }
}

// ============================================================================
// MAIN LOOP
// ============================================================================

unsigned long lastBroadcast = 0;
const unsigned long broadcastInterval = 1000;  // 1 second

void loop() {
  // Handle serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }
  
  // Broadcast sensor data to web clients
  unsigned long currentTime = millis();
  if (currentTime - lastBroadcast >= broadcastInterval) {
    broadcastSensorData();
    checkCriticalTemperature();
    lastBroadcast = currentTime;
  }
  
  // Clean up WebSocket clients
  ws.cleanupClients();
  
  delay(10);
}
