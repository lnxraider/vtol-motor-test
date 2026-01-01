# Contributing to VTOL Motor Test System

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

---

## 📋 Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
- [Development Setup](#development-setup)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Testing Requirements](#testing-requirements)
- [Documentation](#documentation)

---

## 🤝 Code of Conduct

### Our Pledge

We are committed to providing a welcoming and inclusive environment for all contributors.

### Expected Behavior

- Be respectful and constructive
- Focus on what is best for the project
- Show empathy towards others
- Accept constructive criticism gracefully

### Unacceptable Behavior

- Harassment or discrimination of any kind
- Trolling or inflammatory comments
- Publishing others' private information
- Other conduct which could reasonably be considered inappropriate

---

## 🛠️ How Can I Contribute?

### Reporting Bugs

**Before Submitting:**
1. Check existing issues to avoid duplicates
2. Verify the bug on latest version
3. Test on actual hardware if possible

**Bug Report Should Include:**
- Clear, descriptive title
- Steps to reproduce
- Expected vs actual behavior
- Hardware configuration
- Software versions (Arduino IDE, ESP32 core, libraries)
- Serial monitor output
- Screenshots if applicable

**Template:**
```markdown
**Description:**
Brief description of the issue

**Steps to Reproduce:**
1. Connect hardware...
2. Upload sketch...
3. Click ARM button...
4. Observe error...

**Expected Behavior:**
What should happen

**Actual Behavior:**
What actually happens

**Environment:**
- ESP32 Board: [e.g., ESP32 DevKit v1]
- Arduino IDE: [e.g., 2.2.1]
- ESP32 Core: [e.g., 2.0.14]
- Library Versions: [list key libraries]

**Additional Context:**
Serial monitor output, photos, etc.
```

### Suggesting Features

**Before Suggesting:**
1. Check if feature already requested
2. Consider if it fits project scope
3. Think about implementation complexity

**Feature Request Should Include:**
- Clear use case
- Expected benefits
- Potential drawbacks
- Implementation ideas (optional)

### Contributing Code

**Types of Contributions Welcome:**
- Bug fixes
- Feature implementations
- Performance improvements
- Documentation improvements
- Test coverage
- Example sketches

---

## 💻 Development Setup

### Prerequisites

```bash
# Required software
- Arduino IDE 2.x or 1.8.19+
- Git
- Text editor (VSCode recommended)

# Hardware for testing
- ESP32 development board
- ESC with DSHOT support
- Test motor
- Power supply (6S battery)
```

### Initial Setup

```bash
# 1. Fork repository on GitHub
# 2. Clone your fork
git clone https://github.com/YOUR_USERNAME/vtol-motor-test.git
cd vtol-motor-test

# 3. Add upstream remote
git remote add upstream https://github.com/ORIGINAL_OWNER/vtol-motor-test.git

# 4. Install required libraries (see README.md)

# 5. Create development branch
git checkout -b feature/my-feature
```

### Keeping Your Fork Updated

```bash
# Fetch upstream changes
git fetch upstream

# Merge upstream main into your local main
git checkout main
git merge upstream/main

# Rebase your feature branch
git checkout feature/my-feature
git rebase main
```

---

## 🔄 Pull Request Process

### Before Submitting

- [ ] Code compiles without errors or warnings
- [ ] Tested on actual hardware
- [ ] Documentation updated (if applicable)
- [ ] CHANGELOG.md updated
- [ ] Code follows project style
- [ ] Commits are logical and well-described

### Submission Process

1. **Push to your fork:**
   ```bash
   git push origin feature/my-feature
   ```

2. **Create Pull Request on GitHub:**
   - Use clear, descriptive title
   - Reference related issues (#123)
   - Describe changes in detail
   - Include testing performed
   - Add screenshots/videos if UI changes

3. **PR Template:**
   ```markdown
   ## Description
   Brief description of changes
   
   ## Type of Change
   - [ ] Bug fix
   - [ ] New feature
   - [ ] Breaking change
   - [ ] Documentation update
   
   ## Testing
   - [ ] Compiled successfully
   - [ ] Tested on hardware
   - [ ] No regression issues
   
   ## Checklist
   - [ ] Code follows style guidelines
   - [ ] Self-reviewed code
   - [ ] Commented complex sections
   - [ ] Updated documentation
   - [ ] Updated CHANGELOG.md
   
   ## Related Issues
   Fixes #123
   ```

4. **Respond to review feedback:**
   - Address all comments
   - Push additional commits if needed
   - Request re-review when ready

5. **Merge:**
   - Maintainer will merge after approval
   - Delete your feature branch after merge

---

## 📝 Coding Standards

### General Principles

- **Clarity over cleverness**
- **Comments explain WHY, not WHAT**
- **Consistent formatting**
- **Meaningful names**

### C++ Style

**Naming Conventions:**
```cpp
// Variables: camelCase
int currentThrottle = 0;
float motorTemp = 0.0;

// Constants: UPPER_SNAKE_CASE
#define TEMP_CRITICAL 95.0
const int PWM_MIN = 1000;

// Functions: camelCase
void updateSensors() { }
bool checkTemperature() { }

// Classes: PascalCase
class MotorController { };
```

**Formatting:**
```cpp
// Indentation: 2 spaces (not tabs)
void myFunction() {
  if (condition) {
    doSomething();
  }
}

// Braces: K&R style
if (armed) {
  currentThrottle = 50;
} else {
  currentThrottle = 0;
}

// Line length: Prefer <100 characters
// Break long lines logically
String longMessage = "This is a very long message that should be "
                     "broken across multiple lines for readability";
```

**Comments:**
```cpp
// Good: Explains WHY
// Set timeout to 2 seconds because ESP32 needs time to initialize WiFi
const int WIFI_TIMEOUT = 2000;

// Bad: States the obvious
// Set timeout to 2000
const int WIFI_TIMEOUT = 2000;

// Section headers for organization
// ============================================================================
// SENSOR READING FUNCTIONS
// ============================================================================
```

### HTML/CSS/JavaScript

**HTML:**
```html
<!-- Use semantic HTML -->
<button class="btn-arm" onclick="sendCommand('ARM')">
  ARM
</button>

<!-- Clear class names -->
<div class="temperature-display">
  <span class="temp-value">25.5°C</span>
</div>
```

**CSS:**
```css
/* Group related properties */
.btn-arm {
  background: #28a745;
  color: white;
  padding: 12px 20px;
  border-radius: 8px;
}

/* Use meaningful names */
.temp-critical { color: #dc3545; }
.temp-normal { color: #28a745; }
```

**JavaScript:**
```javascript
// Use descriptive variable names
let isArmed = false;
let currentThrottle = 0;

// Clear function names
function sendCommand(cmd) {
  // Implementation
}

// Comments for complex logic
// WebSocket may not be open yet, check first
if (websocket.readyState === WebSocket.OPEN) {
  websocket.send(data);
}
```

---

## 🧪 Testing Requirements

### Compilation Testing

```bash
# Must compile without errors
✓ Compilation successful
✓ No errors
⚠ Warnings acceptable if documented

# Test on multiple ESP32 core versions
- ESP32 Core 2.0.x
- ESP32 Core 3.0.x
```

### Hardware Testing

**Minimum Testing:**
- [ ] Code uploads successfully
- [ ] Serial monitor shows correct startup
- [ ] WiFi Access Point created
- [ ] Web interface loads
- [ ] WebSocket connects
- [ ] ARM/DISARM works
- [ ] Throttle control responsive
- [ ] Temperature reading works
- [ ] Mode switching works (if applicable)

**Extended Testing (for major features):**
- [ ] Test on multiple ESP32 boards
- [ ] Test with different ESCs
- [ ] Test across full throttle range
- [ ] Test temperature safety features
- [ ] Test emergency stop
- [ ] Test multiple simultaneous clients
- [ ] Long-duration stability test (30+ min)

### Regression Testing

Before submitting PR:
- [ ] All existing features still work
- [ ] No new warnings introduced
- [ ] Performance not degraded
- [ ] Memory usage acceptable

---

## 📚 Documentation

### Code Documentation

**Required Comments:**
```cpp
// At top of file
/*
 * Brief description of file purpose
 * Major functions/features
 * Dependencies
 */

// For complex functions
/**
 * Calculate motor efficiency from power and thrust data
 * 
 * @param thrust_g Thrust in grams
 * @param power_w Power consumption in watts
 * @return Efficiency in grams per watt
 */
float calculateEfficiency(int thrust_g, float power_w) {
  // Implementation
}

// For non-obvious code sections
// DSHOT requires continuous 1kHz stream
// Missing packets trigger ESC failsafe
while (dshotUpdateEnabled) {
  dshotMotor->sendThrottlePercent(throttlePercent);
  vTaskDelayUntil(&xLastWakeTime, xFrequency);
}
```

### README Updates

If your changes affect:
- Features → Update features list
- Hardware → Update hardware requirements
- Software → Update library requirements
- Installation → Update installation steps
- Usage → Update usage instructions

### CHANGELOG Updates

**Always update CHANGELOG.md:**

```markdown
## [Unreleased]

### Added
- Feature X that does Y

### Changed
- Modified Z to improve W

### Fixed
- Bug in A that caused B
```

### Documentation Files

If creating new features:
- Add usage examples to README
- Create detailed doc in `/docs` if complex
- Update troubleshooting guide if needed
- Add screenshots/diagrams if helpful

---

## 🎯 Specific Guidelines

### Web Interface Changes

- Test on mobile (phone/tablet)
- Test on desktop browsers
- Verify responsive design
- Check all breakpoints
- Test touch interactions
- Verify WebSocket updates

### Safety Feature Changes

**Extra scrutiny required!**
- Document safety implications
- Test failure modes
- Verify emergency stop works
- Test auto-stop at critical temperature
- Ensure ARM/DISARM enforced

### Performance Changes

If claiming performance improvement:
- Provide before/after metrics
- Test with actual hardware
- Verify no functionality lost
- Document methodology

---

## ❓ Questions?

**For Development Questions:**
- Open a discussion on GitHub
- Reference specific code sections
- Provide context for your question

**For Feature Ideas:**
- Open a feature request issue
- Explain use case clearly
- Discuss before implementing

---

## 🏆 Recognition

Contributors will be recognized in:
- GitHub contributors list
- CHANGELOG.md
- README.md acknowledgments section

---

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to the VTOL Motor Test System! Your efforts help make this project better for everyone. 🚀
