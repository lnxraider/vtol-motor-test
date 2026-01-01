# Troubleshooting Guide - VTOL Motor Test System

Comprehensive problem-solving guide for hardware and software issues.

---

## Table of Contents

- [Quick Diagnostics](#quick-diagnostics)
- [WiFi and Web Interface](#wifi-and-web-interface)
- [Motor Control Issues](#motor-control-issues)
- [Sensor Problems](#sensor-problems)
- [Compilation Errors](#compilation-errors)
- [Upload Issues](#upload-issues)
- [Performance Problems](#performance-problems)

---

## Quick Diagnostics

### First Steps for ANY Problem

```
1. Open Serial Monitor (115200 baud)
2. Type: STATUS
3. Read the output carefully
4. Check which sensors are working/failing
5. Verify mode (PWM or DSHOT)
6. Check armed state
```

### Emergency Recovery

**System completely unresponsive:**
```
1. Disconnect battery
2. Disconnect USB
3. Wait 10 seconds
4. Reconnect USB only
5. Upload sketch again
6. Check Serial Monitor for errors
```

---

## WiFi and Web Interface

### Cannot Find WiFi Network

**Symptom:** "MotorTest_Rig" network not visible

**Diagnosis:**
```
1. Open Serial Monitor
2. Look for:
   [WIFI] Starting Access Point...
   [WIFI] AP IP address: 192.168.4.1
```

**Solutions:**

**If you see "WIFI Failed" in serial:**
```cpp
// ESP32 WiFi issue - reflash entire chip
1. Arduino IDE → Tools → Erase Flash: "All Flash Contents"
2. Upload sketch again
```

**If WiFi starts but not visible:**
```
1. Check device WiFi settings
2. Scan for 2.4GHz networks (ESP32 doesn't support 5GHz)
3. Try different device (phone/tablet/laptop)
4. Move closer to ESP32 (<10 feet)
5. Reboot ESP32
```

**If network visible but can't connect:**
```
1. Verify password exactly: test1234 (all lowercase)
2. Forget network and reconnect
3. Disable VPN if active
4. Check device isn't blocking connection
```

---

### Web Page Won't Load

**Symptom:** Connected to WiFi but http://192.168.4.1 doesn't load

**Diagnosis:**
```
1. Verify connected to "MotorTest_Rig" (not home WiFi!)
2. Check Serial Monitor for:
   [WEB] Web server started on port 80
```

**Solutions:**

**Check connection:**
```bash
# From computer/phone browser
ping 192.168.4.1

# Should respond with replies
```

**Try different approaches:**
```
1. Use exact URL: http://192.168.4.1 (not https://)
2. Try different browser (Chrome, Safari, Firefox)
3. Clear browser cache
4. Try incognito/private mode
5. Disable any ad-blockers
6. Turn off mobile data (use WiFi only)
```

**Force refresh:**
```
Ctrl+F5 (Windows)
Cmd+Shift+R (Mac)
```

---

### WebSocket Disconnects

**Symptom:** "Disconnected - Reconnecting..." message

**Diagnosis:**
```
Serial Monitor shows:
[WEB] Client #X disconnected
```

**Common Causes:**

**1. Weak WiFi signal:**
```
Solution: Move closer to ESP32 (<15 feet)
```

**2. ESP32 rebooting:**
```
Check Serial Monitor for:
- "Guru Meditation Error"
- Unexpected restarts
- Stack traces

If restarting:
1. Check power supply (use quality USB cable)
2. Check for memory leaks in custom code
3. Monitor temperature (ESP32 shouldn't be hot)
```

**3. Too many clients:**
```
Max 4-5 simultaneous connections

Solution: Disconnect unused clients
```

**4. Browser sleeping:**
```
Mobile browsers may sleep background tabs

Solution: Keep browser in foreground
```

---

## Motor Control Issues

### Motor Doesn't Respond

**Symptom:** Throttle changes but motor doesn't spin

**Step-by-Step Diagnosis:**

**1. Check ESC Power:**
```
- Battery connected?
- Battery charged? (6S = 22.2V nominal, 25.2V full)
- ESC power LED on?
- Listen for ESC startup beeps
```

**2. Check ARM Status:**
```
Serial Monitor or Web Interface:
- Status shows: ARMED?
- If DISARMED: Click ARM button first!
```

**3. Verify Signal Connection:**
```
ESP32 GPIO 25 → ESC Signal Wire
ESP32 GND → ESC GND

Test with multimeter:
- Check continuity between ESP32 GND and ESC GND
```

**4. Test in PWM Mode:**
```
DSHOT may not work with all ESCs

1. Type: PWM (switch to PWM mode)
2. ARM motor
3. Set T10 (10% throttle)
4. Motor should spin
```

**5. ESC Calibration (if still not working):**
```
Standard ESC calibration procedure:
1. DISARM motor
2. Set throttle to 100% (T100)
3. ARM motor
4. Wait for ESC beeps
5. Set throttle to 0% (T0)
6. Wait for ESC confirmation beeps
7. DISARM
8. Try normal operation (T10-20)
```

---

### Motor Spins Briefly Then Stops

**Symptom:** Motor starts but immediately stops

**Causes:**

**1. DSHOT stream stopped:**
```
Check Serial Monitor:
[DSHOT] Task running: YES/NO

If NO:
1. Type: DSHOT (reinitialize DSHOT mode)
2. Should see: "Continuous throttle stream active"
```

**2. ESC arming timeout:**
```
Some ESCs need continuous signal

Solution: Use DSHOT mode (continuous 1kHz stream)
```

**3. Low battery voltage:**
```
Check Serial Monitor: Battery voltage

6S Battery voltage:
- Full: 25.2V
- Nominal: 22.2V
- Low: 21.0V
- Critical: <20.0V (STOP!)

Solution: Charge battery
```

---

### Throttle Laggy or Unresponsive

**Symptom:** Delay between throttle change and motor response

**PWM Mode:**
```
Normal latency: ~20ms
If worse:
1. Check WiFi signal strength
2. Reduce distance to ESP32
3. Disconnect other WiFi clients
```

**DSHOT Mode:**
```
Normal latency: ~1-2ms
If laggy:
1. Check DSHOT task running
2. Type: STATUS
3. Verify: "Control Mode: DSHOT"
4. Should be nearly instantaneous
```

**Web Interface:**
```
WiFi adds 20-50ms latency (normal)

For fastest response: Use Serial commands
Example: T50 (instant)
```

---

## Sensor Problems

### Temperature Shows "--°C" or "0.0°C"

**Symptom:** No temperature reading

**Quick Test:**
```
Type: TEMP

Should show:
✅ Motor Temperature: XX.X°C
❌ Temperature sensor not detected
```

**Diagnosis Steps:**

**1. Check Wiring:**
```
MAX31855 → ESP32
VCC → 3.3V (NOT 5V!)
GND → GND
SCK → GPIO 18
CS → GPIO 5
SO → GPIO 19
```

**2. Verify Voltage:**
```
Use multimeter:
MAX31855 VCC pin = 3.3V ±0.1V

If 5V: STOP! This will damage module!
If 0V: No power, check connection
```

**3. Test Thermocouple:**
```
1. Touch probe with hand
2. Temperature should increase in 2-3 seconds
3. If no change: Probe disconnected or broken
```

**4. Swap Components:**
```
If you have spare:
1. Try different MAX31855 module
2. Try different thermocouple probe
3. Identify failing component
```

**See:** [MAX31855_DIAGNOSIS.md](MAX31855_DIAGNOSIS.md) for detailed troubleshooting

---

### Voltage Reading Incorrect

**Symptom:** Voltage shows wrong value or 0.0V

**Expected Readings:**
```
6S Battery (fully charged): 25.0-25.2V
6S Battery (nominal): 22.2V
6S Battery (empty): 21.0V
```

**If showing 0.0V:**
```
1. Battery connected?
2. PM02 powered? (check 3.3V to PM02)
3. Check GPIO 34 connection
4. Verify PM02 not damaged
```

**If showing wrong voltage:**
```
Calibration needed:

1. Measure actual battery voltage with multimeter
2. Note voltage shown in Serial Monitor
3. Calculate calibration:
   VOLTAGE_CAL = (actual_voltage / shown_voltage) * current_cal
   
Example:
- Actual: 25.2V
- Shown: 23.5V
- Current cal: 1.3811
- New cal: (25.2 / 23.5) * 1.3811 = 1.48

4. Update in code (line ~150):
   #define VOLTAGE_CAL 1.48
```

---

### Current Always Reads 0.0A

**PM02 Current (GPIO 35):**
```
Known issue - this sensor not working
This is expected behavior
Use ESC current sensor instead
```

**ESC Current (GPIO 32):**
```
Should show >0A when motor running

If always 0.0A:
1. Verify ESC has current sensing
2. Check telemetry wire connected
3. Check GPIO 32 connection
4. Verify common ground (ESP32 ↔ ESC)
5. Some ESCs may need configuration
```

---

## Compilation Errors

### Library Not Found

**Error:** `fatal error: ESPAsyncWebServer.h: No such file or directory`

**Solution:**
```
1. Arduino IDE → Tools → Manage Libraries
2. Search for: "ESP Async WebServer"
3. Install: mathieucarbou/ESP Async WebServer (v3.0+)
4. Also install: mathieucarbou/AsyncTCP
5. Restart Arduino IDE
6. Recompile
```

---

### mbedtls_md5_starts_ret Error

**Error:** `'mbedtls_md5_starts_ret' was not declared`

**Cause:** Using wrong library version (me-no-dev instead of mathieucarbou)

**Solution:**
```
1. Uninstall me-no-dev libraries:
   - ESPAsyncWebServer
   - AsyncTCP

2. Install mathieucarbou versions:
   - mathieucarbou/ESP Async WebServer
   - mathieucarbou/AsyncTCP

3. Restart Arduino IDE
4. Recompile
```

---

### RMT_CHANNEL_0 Not Declared

**Error:** `'RMT_CHANNEL_0' was not declared in this scope`

**Cause:** Old DSHOT initialization code

**Solution:**
```
Already fixed in v2.5.2
Download latest code from GitHub
```

---

### Sketch Too Large

**Error:** `Sketch uses XXXXX bytes (XX%) of program storage space. Maximum is 1310720 bytes.`

**If over 100%:**
```
Solution 1: Use larger partition
1. Tools → Partition Scheme → "Huge APP (3MB No OTA/1MB SPIFFS)"
2. Recompile

Solution 2: Reduce features
1. Remove unused code
2. Reduce HTML size in index_html
3. Remove debug Serial.print statements
```

---

## Upload Issues

### Port Not Found

**Error:** `Serial port 'COMX' not found`

**Solutions:**

**Windows:**
```
1. Check Device Manager → Ports (COM & LPT)
2. Look for "CP210x" or "CH340"
3. If missing: Install drivers
   - CP2102: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - CH340: http://www.wch-ic.com/downloads/CH341SER_ZIP.html
4. Restart computer
5. Reconnect ESP32
```

**Mac/Linux:**
```
1. Check: ls /dev/tty.*
2. Look for: /dev/tty.usbserial-XXXX
3. Select correct port in Arduino IDE
```

---

### Upload Fails / Timeout

**Error:** `A fatal error occurred: Failed to connect to ESP32`

**Solutions:**

**Method 1: Hold BOOT button:**
```
1. Hold BOOT button on ESP32
2. Click Upload in Arduino IDE
3. Keep holding until "Connecting..." appears
4. Release button
5. Upload should proceed
```

**Method 2: Different upload speed:**
```
Tools → Upload Speed → 115200
(Try slower speed if 921600 fails)
```

**Method 3: Different USB cable:**
```
Some cables are power-only (no data)
Use a known-good data cable
```

**Method 4: Different USB port:**
```
Try different port on computer
USB 2.0 ports often more reliable than USB 3.0
```

---

## Performance Problems

### Web Interface Slow

**Symptom:** Laggy updates, slow response

**Solutions:**

**1. Reduce WiFi distance:**
```
Move device closer to ESP32 (<10 feet)
```

**2. Disconnect extra clients:**
```
Max 4-5 clients for best performance
Close unnecessary browser tabs
```

**3. Increase update interval:**
```
Modify code (line ~1892):
const unsigned long broadcastInterval = 2000; // 2 seconds instead of 1
```

---

### ESP32 Reboots Randomly

**Symptom:** System restarts unexpectedly

**Check Serial Monitor for:**
```
Guru Meditation Error: Core X panic'ed
Brownout detector was triggered
```

**Common Causes:**

**1. Insufficient power:**
```
Solution:
- Use quality USB cable
- Try different USB power source
- Don't power from laptop battery (use wall adapter)
```

**2. Memory overflow:**
```
Check available heap:
Serial.println(ESP.getFreeHeap());

Should be >100KB typically
If low: Memory leak in custom code
```

**3. Stack overflow:**
```
Increase stack size for DSHOT task:
xTaskCreatePinnedToCore(
  dshotUpdateTask,
  "DSHOT_Update",
  4096,  // Increase from 2048
  ...
);
```

---

### Temperature Reading Fluctuates

**Symptom:** Temperature jumps around ±5-10°C

**Causes:**

**1. EMI from motor/ESC:**
```
Solutions:
- Route thermocouple away from power wires
- Add ferrite bead on thermocouple cable
- Use shielded thermocouple cable
- Ground shield at ESP32 end only
```

**2. Loose connections:**
```
Check all MAX31855 wire connections
Secure with solder instead of jumpers
```

**3. Damaged thermocouple:**
```
Test with known-good probe
Replace if damaged
```

---

## Error Messages Reference

### Common Error Messages and Solutions

```
"ERROR: Motor not armed!"
→ Click ARM button first

"ERROR: Throttle must be 0-100%"
→ Use valid throttle range

"ERROR: Cannot change mode while armed"
→ DISARM before switching PWM/DSHOT

"Temperature sensor not detected"
→ Check MAX31855 wiring

"CRITICAL TEMPERATURE - AUTO STOPPING"
→ Normal safety feature, let motor cool

"WiFi connection failed"
→ Reboot ESP32, check WiFi configuration
```

---

## Advanced Diagnostics

### Serial Monitor Verbose Mode

Add to setup():
```cpp
Serial.setDebugOutput(true);
```

Shows detailed WiFi and system messages.

---

### Check Memory Usage

Add to loop():
```cpp
if (millis() % 10000 == 0) {
  Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
}
```

Helps identify memory leaks.

---

### Test Individual Components

**Test MAX31855 only:**
```cpp
void loop() {
  float temp = thermocouple.readCelsius();
  Serial.printf("Temp: %.2f°C\n", temp);
  delay(1000);
}
```

**Test WiFi only:**
```cpp
// Comment out all sensor code
// Just run WiFi and web server
```

Isolate which component is causing issues.

---

## Getting Further Help

**Before Asking for Help:**

1. Read this troubleshooting guide
2. Check Serial Monitor for errors
3. Try basic solutions (reboot, reconnect, etc.)
4. Note exact error messages
5. Document what you've tried

**When Reporting Issues:**

Include:
- ESP32 board model
- Arduino IDE version
- ESP32 core version
- Library versions
- Complete error message
- Serial Monitor output
- Steps to reproduce
- Photos of wiring (if hardware issue)

**Where to Get Help:**

- GitHub Issues: Best for bugs
- README.md: Installation and setup
- Other docs: Specific topics

---

**Document Version:** 1.0  
**Last Updated:** December 21, 2025  
**Status:** Production Ready ✅
