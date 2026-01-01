# Changelog

All notable changes to the VTOL Motor Test System will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [2.5.2] - 2025-12-21

### Added
- PWM/DSHOT mode selector buttons in web interface
- Mode indicator badge showing current protocol
- 5% and 10% throttle quick buttons
- Mode switching commands (PWM, DSHOT) via serial
- WebSocket mode change message type
- Mode status in sensor data broadcasts
- DSHOT throttle control via web interface

### Changed
- Updated HELP command to include mode switching
- Throttle buttons now: 0%, 5%, 10%, 25%, 50%, 75%, 95%
- Improved mode switching safety (requires disarm)

### Fixed
- DSHOT initialization API updated for current DShotRMT library
- Continuous DSHOT update task restored (1kHz stream required)
- Motor response issues in DSHOT mode resolved

---

## [2.5.1] - 2025-12-21

### Fixed
- **CRITICAL:** Voltage calculation formula corrected (division not multiplication)
- **CRITICAL:** Removed incorrect 1.65V offset from current sensors
- Changed `abs()` to `fabs()` for proper float comparison
- Fixed deprecated `send_P()` warning in web server
- Updated for mathieucarbou ESP Async WebServer v3.0+ compatibility

### Changed
- Sensor calculations now accurate for unidirectional sensors
- PM02 and ESC current sensors properly calibrated

### Documentation
- Added detailed explanation of sensor calculation fixes
- Documented historical context of 1.65V offset error

---

## [2.5.0] - 2025-12-21

### Added
- **Web User Interface** - Complete mobile-responsive control interface
- **Temperature Monitoring** - MAX31855 K-type thermocouple integration
- **WiFi Access Point** - No router required (MotorTest_Rig/test1234)
- **WebSocket Communication** - Real-time sensor data updates (1 Hz)
- **Visual Controls** - Touch-friendly buttons and slider
- **Color-Coded Temperature** - Green/Orange/Red/Flashing indicators
- **Multi-Client Support** - Up to 5 simultaneous viewers
- **Auto-Stop Safety** - Automatic shutdown at 95°C critical temperature
- **Mode Display** - Current protocol shown in web interface
- **Connection Status** - Visual WiFi connection indicator

### Changed
- Complete system architecture redesign for web integration
- Sensor reading integrated with WebSocket broadcast
- Temperature monitoring integrated throughout all commands

### Documentation
- Complete Web Interface documentation added
- Hardware wiring diagrams updated
- Troubleshooting guide expanded

---

## [2.4.0] - 2025-12-14

### Added
- Automated thrust sweep function (SWEEP command)
- Data storage for up to 20 test points
- Automatic efficiency calculation (g/W)
- Results table with statistics
- CSV export function (EXPORT command)
- VTOL performance calculations (6 motors)
- Thrust-to-weight ratio analysis

### Changed
- Improved data presentation with formatted tables
- Enhanced test result analysis

---

## [2.3.0] - 2025-12-14

### Added
- ESC current sensor support (GPIO 32)
- Dual sensor architecture (PM02 + ESC)
- Raw pin voltage display for debugging
- Diagnostic commands (DIAG, RAW, PINTEST)

### Changed
- Added 1.65V offset compensation for bidirectional sensors (later found incorrect in v2.5.1)
- Updated POWER display to show both current sensors
- Improved sensor status indicators

### Fixed
- ESC current sensor readings enabled
- Sensor auto-detection improved

---

## [2.2.0] - 2025-12-02

### Added
- Proper ESC arming sequence (extended zero throttle)
- Continuous DSHOT throttle update task (1kHz)
- DSHOT600/300/150 fallback support
- Improved initialization feedback

### Fixed
- Motor not spinning issue resolved
- DSHOT timing and initialization
- GPIO 33 vs GPIO 35 inconsistency corrected
- ESC arming sequence implementation

### Documentation
- Added explanation of why motor wasn't spinning
- Documented ESC arming requirements

---

## [2.1.0] - 2025-11-27

### Added
- PM02 voltage sensor calibration
- Voltage reading accuracy improvements

### Changed
- Voltage calibration constant updated (1.3811)
- Improved voltage reading stability

---

## [2.0.0] - 2025-11-20

### Added
- DSHOT protocol support (DSHOT300)
- PWM/DSHOT hybrid operation
- Protocol switching capability
- DShotRMT library integration

### Changed
- Major architecture update for dual protocol support
- Motor control abstraction layer

---

## [1.0.0] - 2025-11-01

### Added
- Initial release
- PWM motor control
- Basic sensor reading (voltage, current)
- Serial command interface
- Power and capacity calculations
- ARM/DISARM safety system

---

## Version Numbering

**Format:** MAJOR.MINOR.PATCH

- **MAJOR:** Incompatible API changes
- **MINOR:** New features (backward compatible)
- **PATCH:** Bug fixes (backward compatible)

---

## Upcoming Features

### Planned for v3.0
- Real-time temperature graphing
- SD card data logging
- Multi-motor support (6 channels)
- Advanced test sequencing
- Video integration (ESP32-CAM)

### Under Consideration
- Mobile app (iOS/Android)
- Cloud data upload
- Advanced analytics
- Authentication system

---

## Support

For bug reports and feature requests, please open an issue on GitHub.

**Project Repository:** https://github.com/YOUR_USERNAME/vtol-motor-test
