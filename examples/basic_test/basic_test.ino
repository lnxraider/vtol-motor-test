/*
 * Basic PWM Motor Test Example
 * 
 * This is a simplified version demonstrating basic motor control
 * without web interface or advanced features.
 * 
 * Hardware:
 * - ESP32 GPIO 25 → ESC Signal
 * - ESP32 GND → ESC GND
 * - ESC powered by battery
 * - Motor connected to ESC
 */

#include <ESP32Servo.h>

// Pin definitions
#define MOTOR_PIN 25

// PWM settings
#define PWM_MIN 1000
#define PWM_MAX 2000
#define PWM_FREQUENCY 50

// Servo object for PWM control
Servo esc;

// State variables
bool armed = false;
int currentThrottle = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Basic PWM Motor Test ===\n");
  
  // Initialize PWM
  esc.setPeriodHertz(PWM_FREQUENCY);
  esc.attach(MOTOR_PIN, PWM_MIN, PWM_MAX);
  esc.writeMicroseconds(PWM_MIN);
  
  Serial.println("ESC initialized");
  Serial.println("Commands:");
  Serial.println("  ARM      - Arm motor");
  Serial.println("  DISARM   - Disarm motor");
  Serial.println("  T<0-100> - Set throttle (e.g., T50)");
  Serial.println();
}

void loop() {
  // Handle serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    
    if (command == "ARM") {
      armed = true;
      Serial.println("[OK] Motor ARMED");
      
    } else if (command == "DISARM") {
      armed = false;
      currentThrottle = 0;
      esc.writeMicroseconds(PWM_MIN);
      Serial.println("[OK] Motor DISARMED");
      
    } else if (command.startsWith("T")) {
      if (!armed) {
        Serial.println("[ERROR] Motor not armed! Type ARM first.");
        return;
      }
      
      int throttle = command.substring(1).toInt();
      if (throttle < 0 || throttle > 100) {
        Serial.println("[ERROR] Throttle must be 0-100%");
        return;
      }
      
      currentThrottle = throttle;
      int pulseWidth = map(throttle, 0, 100, PWM_MIN, PWM_MAX);
      esc.writeMicroseconds(pulseWidth);
      
      Serial.printf("[OK] Throttle set to %d%%\n", throttle);
    }
  }
}
