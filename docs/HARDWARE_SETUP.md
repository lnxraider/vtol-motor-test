# Hardware Setup Guide - VTOL Motor Test System

Complete wiring and hardware configuration guide for the ESP32 Motor Test System.

---

## Table of Contents

- [Required Components](#required-components)
- [Tools Needed](#tools-needed)
- [Pin Configuration](#pin-configuration)
- [Step-by-Step Wiring](#step-by-step-wiring)
- [Power Configuration](#power-configuration)
- [Safety Checks](#safety-checks)
- [Testing Procedure](#testing-procedure)

---

## Required Components

### Core Components

| Component | Specification | Quantity | Notes |
|-----------|--------------|----------|-------|
| **ESP32** | DevKit v1 or similar | 1 | Any ESP32 with WiFi |
| **ESC** | 60A+ with DSHOT support | 1 | SpeedyBee, BlueJay firmware |
| **Motor** | EMAX ECO II 2807 1500KV | 1 | Or similar brushless |
| **Battery** | 6S LiPo 6000-8000mAh | 1 | 22.2V nominal |
| **MAX31855** | Thermocouple module | 1 | SPI interface |
| **K-Type Probe** | 0-400°C range | 1 | For motor temperature |

### Optional Components

| Component | Purpose | Notes |
|-----------|---------|-------|
| **PM02v3** | Voltage/current monitoring | Optional but recommended |
| **Thrust Stand** | Motor testing | Kitchen scale works |
| **Propeller** | Testing load | 5-inch recommended |

### Wiring Materials

- Dupont jumper wires (male-to-male, male-to-female)
- Soldering iron and solder
- Heat shrink tubing
- Zip ties
- Electrical tape

---

## Tools Needed

- Wire strippers
- Soldering iron
- Multimeter
- Screwdriver set
- Hot glue gun (optional, for strain relief)
- Label maker or tape (for wire identification)

---

## Pin Configuration

### ESP32 Pinout

```
                    ESP32 DevKit v1
                    ┌─────────────┐
                    │   ┌─────┐   │
                    │   │ USB │   │
                    │   └─────┘   │
        ┌───────────┴─────────────┴───────────┐
        │                                     │
     3V3├─────────────────────────────────────┤GND
     EN │                                     │23
    VP36│                                     │22
    VN39│                                     │TX0
     34 ├─ PM02 Voltage                       │RX0
     35 │                                     │21
     32 ├─ ESC Current                        │19 ─┤ MAX31855 SO
     33 │                                     │18 ─┤ MAX31855 SCK
     25 ├─ ESC Signal (PWM/DSHOT)             │5  ─┤ MAX31855 CS
     26 │                                     │17
     27 │                                     │16
     14 │                                     │4
     12 │                                     │0
        │                                     │
     GND├─────────────────────────────────────┤2
     13 │                                     │15
     SD2│                                     │SD1
     SD3│                                     │SD0
     CMD│                                     │CLK
        └─────────────────────────────────────┘
             │                           │
            5V                          GND
```

### Complete Pin Assignments

```
MOTOR CONTROL:
  GPIO 25 → ESC Signal Wire (PWM/DSHOT300)

TEMPERATURE MONITORING (MAX31855):
  GPIO 18 → MAX31855 SCK (Clock)
  GPIO 5  → MAX31855 CS (Chip Select)
  GPIO 19 → MAX31855 SO (Data Out / MISO)
  3.3V    → MAX31855 VCC
  GND     → MAX31855 GND

POWER MONITORING (PM02v3):
  GPIO 34 → PM02 Voltage Sensor (0-3.3V)
  GPIO 35 → PM02 Current Sensor (currently non-functional)
  GPIO 32 → ESC Current Sensor (0-3.3V) - Primary sensor
  3.3V    → PM02 VCC
  GND     → PM02 GND

POWER:
  5V USB  → ESP32 VIN (via USB cable)
  GND     → Common ground (CRITICAL!)
```

---

## Step-by-Step Wiring

### Step 1: ESP32 Base Setup

**Connect USB cable only:**
```
1. ESP32 → USB cable → Computer
2. Do NOT connect battery yet
3. Verify ESP32 powers on (LED lights up)
```

### Step 2: MAX31855 Temperature Sensor

**Connections:**
```
MAX31855 Module    →    ESP32
─────────────────────────────
VCC (or +)         →    3.3V  (NOT 5V!)
GND (or -)         →    GND
SCK (Clock)        →    GPIO 18
CS (Chip Select)   →    GPIO 5
SO (Data Out)      →    GPIO 19
```

**Thermocouple Connections:**
```
K-Type Probe       →    MAX31855
─────────────────────────────
Red wire (+)       →    + terminal
Blue wire (-)      →    - terminal
```

⚠️ **CRITICAL:** 
- Use 3.3V, NOT 5V! (5V will damage MAX31855)
- Keep thermocouple wires separated (prevent shorts)

### Step 3: PM02 Power Monitor (Optional)

**Connections:**
```
PM02 Module        →    ESP32
─────────────────────────────
VCC                →    3.3V
GND                →    GND
V (Voltage Out)    →    GPIO 34
I (Current Out)    →    GPIO 35 (currently non-functional)
```

**PM02 Power Connections:**
```
Battery → PM02 → ESC

6S Battery +  →  PM02 BAT+
6S Battery -  →  PM02 BAT-
PM02 ESC+     →  ESC Power +
PM02 ESC-     →  ESC Power -
```

### Step 4: ESC Current Sensor

**SpeedyBee ESC Current Telemetry:**
```
ESC Telemetry Wire  →  GPIO 32 (via voltage divider if needed)
ESC GND             →  ESP32 GND
```

⚠️ Check your ESC datasheet - some output 5V telemetry, may need voltage divider!

### Step 5: ESC Signal Wire

**Connections:**
```
ESP32 GPIO 25  →  ESC Signal Wire (yellow/white)
ESP32 GND      →  ESC Ground (black/brown)
```

⚠️ **DO NOT connect ESC power (red wire) to ESP32!**

### Step 6: Motor Connection

**ESC to Motor:**
```
ESC Phase A  →  Motor Wire 1
ESC Phase B  →  Motor Wire 2
ESC Phase C  →  Motor Wire 3
```

(Order doesn't matter - motor will spin either direction, can swap any 2 wires to reverse)

---

## Power Configuration

### Power Architecture

```
┌─────────────┐
│   6S LiPo   │
│ 22.2V Nom   │
└──────┬──────┘
       │
       ↓
┌─────────────┐       ┌──────────────┐
│    PM02     │ →→→   │     ESC      │
│ (Optional)  │       │   60A+ 6S    │
└──────┬──────┘       └──────┬───────┘
       │                     │
       │                     ↓
       │              ┌─────────────┐
       │              │    Motor    │
       │              │ ECO II 2807 │
       │              └─────────────┘
       │
       ↓ (Sensor outputs)
┌─────────────┐
│    ESP32    │ ←─── USB 5V (separate power)
└─────────────┘
```

### Critical Power Notes

⚠️ **IMPORTANT:**
1. **ESP32 powered via USB** - NOT from battery!
2. **Common ground** - ESP32 GND, ESC GND, sensor GND all connected
3. **No power loops** - Don't connect battery voltage to ESP32
4. **Isolated supplies** - Battery powers motor, USB powers ESP32

---

## Safety Checks

### Before First Power-On

**Visual Inspection:**
- [ ] No bare wires touching
- [ ] All connections secure
- [ ] Polarity correct (especially MAX31855 - 3.3V NOT 5V!)
- [ ] No shorts between power and ground
- [ ] Propeller removed or secured

**Multimeter Checks:**
```
1. Check continuity: ESP32 GND ↔ ESC GND ↔ Sensor GND
2. Check voltage: ESP32 3.3V pin (should be 3.3V ±0.1V)
3. Check for shorts: Battery + to Battery - (should be OPEN)
4. Verify MAX31855 VCC = 3.3V (NOT 5V!)
```

### First Power-On Sequence

```
1. Connect USB to ESP32 (battery still disconnected)
2. Open Serial Monitor (115200 baud)
3. Verify system boots:
   - WiFi AP starts
   - Sensors initialize
   - No error messages
4. Disconnect USB
5. Now connect battery to ESC (propeller still OFF!)
6. Reconnect USB
7. Listen for ESC startup beeps
8. Test ARM/DISARM (throttle at 0%)
9. If all OK, install propeller
10. Test at low throttle (10-20%) first
```

---

## Testing Procedure

### Initial System Test (No Propeller)

```
1. Power on system
2. Connect to WiFi: MotorTest_Rig / test1234
3. Open web interface: http://192.168.4.1
4. Verify all sensors reading:
   - Battery voltage shows ~25.2V (6S fully charged)
   - Temperature shows ambient (~20-25°C)
   - Current shows 0.0A (motor off)
5. ARM motor
6. Set throttle to 5%
7. Listen for motor beep/movement
8. DISARM immediately
9. If successful, proceed to propeller testing
```

### Full Test with Propeller

⚠️ **DANGER: Spinning propeller can cause serious injury!**

**Safety precautions:**
- Secure motor to test stand
- Clear area around propeller (3 feet minimum)
- Wear safety glasses
- Keep hands away from propeller
- Use web interface from safe distance
- Have emergency stop ready

**Test sequence:**
```
1. Secure motor with propeller installed
2. Place on scale/thrust stand
3. Clear area (no loose items, wires, etc.)
4. Power on from safe distance
5. Connect to web interface
6. ARM motor
7. Slowly increase throttle (5% → 10% → 20%)
8. Monitor temperature continuously
9. Record thrust readings
10. DISARM after testing
11. Wait for propeller to stop completely
12. Wait for motor to cool (check temperature)
```

---

## Wiring Best Practices

### Wire Management

```
1. Route signal wires away from power wires (reduce EMI)
2. Keep thermocouple wires separated (prevent shorts)
3. Use zip ties for strain relief
4. Label wires with tape/labels
5. Leave slack for movement (don't pull tight)
6. Secure connections with hot glue (optional)
```

### EMI Reduction

```
1. Twist ESC signal and ground wires together
2. Keep high-current wires (battery, ESC) short
3. Route sensor wires away from motor/ESC
4. Use shielded cable for thermocouple (optional)
5. Add ferrite bead on thermocouple (optional)
```

### Serviceability

```
1. Use connectors for easy disassembly
2. Color-code wires consistently
3. Document any custom wiring
4. Take photos before modifying
5. Keep spare connectors/wires available
```

---

## Troubleshooting Hardware Issues

### No Power to ESP32
- Check USB cable
- Try different USB port
- Verify ESP32 not damaged

### MAX31855 Not Detected
- Verify VCC = 3.3V (NOT 5V!)
- Check all 5 wire connections
- Verify correct GPIO pins (18, 5, 19)
- Try different MAX31855 module

### ESC Not Arming
- Check ESC has power (battery connected)
- Verify signal wire connected to GPIO 25
- Check common ground (ESP32 ↔ ESC)
- Listen for ESC error beeps

### Erratic Sensor Readings
- Check common ground connections
- Verify 3.3V power stable
- Check for loose wires
- Test each sensor individually

### Motor Won't Spin
- Verify ESC armed (listen for beeps)
- Check motor connections (3 wires)
- Try swapping any 2 motor wires
- Test in PWM mode first (simpler)

---

## Advanced Configuration

### Adding Additional Sensors

**GPS Module:**
```
GPS TX  →  ESP32 RX (GPIO 16)
GPS RX  →  ESP32 TX (GPIO 17)
GPS VCC →  3.3V
GPS GND →  GND
```

**IMU/Accelerometer:**
```
IMU SDA →  GPIO 21
IMU SCL →  GPIO 22
IMU VCC →  3.3V
IMU GND →  GND
```

### Multiple Motors (VTOL Configuration)

For 6-motor setup:
```
Motor 1: GPIO 25
Motor 2: GPIO 26
Motor 3: GPIO 27
Motor 4: GPIO 14
Motor 5: GPIO 12
Motor 6: GPIO 13
```

(Requires code modification - see future v3.0)

---

## Reference Diagrams

### Simplified Wiring Diagram

```
                  ┌──────────────┐
    USB ─────────→│    ESP32     │
                  │              │
    ┌─────────────┤ GPIO 25      │
    │             │              │
    │             │ GPIO 18,5,19 │←─────── MAX31855 ←─── K-Type Probe
    │             │              │
    │             │ GPIO 34,32   │←─────── PM02 (optional)
    │             │              │
    │             │ GND          │←───┐
    │             └──────────────┘    │
    ↓                                 │
┌────────┐                           │
│  ESC   │←── Battery ←── PM02 ──────┘
└───┬────┘
    │
    ↓
┌────────┐
│ Motor  │
└────────┘
```

### Complete System Diagram

See `/images/wiring_diagram.png` (if available)

---

## Bill of Materials

### Core System (~$150)

| Item | Quantity | Unit Price | Total |
|------|----------|-----------|-------|
| ESP32 DevKit | 1 | $8 | $8 |
| SpeedyBee 60A ESC | 1 | $35 | $35 |
| EMAX ECO II 2807 | 1 | $16 | $16 |
| MAX31855 Module | 1 | $15 | $15 |
| K-Type Probe | 1 | $10 | $10 |
| 6S Battery 6000mAh | 1 | $60 | $60 |
| Wiring & Connectors | - | $10 | $10 |

### Optional Additions (~$30)

| Item | Quantity | Unit Price | Total |
|------|----------|-----------|-------|
| PM02v3 Monitor | 1 | $25 | $25 |
| Thrust Stand | 1 | DIY | $0 |

**Total: ~$180** (one-time setup)

---

## Maintenance

### Regular Checks

**After every 10 flights/tests:**
- [ ] Inspect all solder joints
- [ ] Check wire connections
- [ ] Clean dust from components
- [ ] Verify screw terminals tight

**Monthly:**
- [ ] Test emergency stop
- [ ] Calibrate temperature sensor
- [ ] Check battery health
- [ ] Update firmware if available

### Component Lifespan

- **ESP32:** Indefinite (barring damage)
- **ESC:** 200+ hours typical
- **Motor:** 100-200 hours (depends on usage)
- **Thermocouple:** 1000+ cycles
- **MAX31855:** Indefinite
- **Battery:** 300-500 cycles

---

## Support

**Hardware Issues:** Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)  
**Temperature Sensor:** See [MAX31855_DIAGNOSIS.md](MAX31855_DIAGNOSIS.md)  
**Software Setup:** See main [README.md](../README.md)

---

**Document Version:** 1.0  
**Last Updated:** December 21, 2025  
**Status:** Production Ready ✅
