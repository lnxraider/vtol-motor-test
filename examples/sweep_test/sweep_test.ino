/*
 * Automated Sweep Test Example
 * 
 * This example demonstrates automated throttle sweep testing
 * at predefined throttle points.
 * 
 * Hardware:
 * - ESP32 GPIO 25 → ESC Signal
 * - ESP32 GND → ESC GND
 * - ESC powered by battery
 * - Motor on thrust stand or scale
 * 
 * Usage:
 * 1. Upload sketch
 * 2. Place motor on scale
 * 3. Type SWEEP to start automated test
 * 4. Record thrust readings at each throttle point
 */

#include <ESP32Servo.h>

// Pin definitions
#define MOTOR_PIN 25

// PWM settings
#define PWM_MIN 1000
#define PWM_MAX 2000
#define PWM_FREQUENCY 50

// Test parameters
const int testPoints[] = {5, 10, 25, 50, 75, 95}; // Throttle percentages
const int numPoints = sizeof(testPoints) / sizeof(testPoints[0]);
const int testDuration = 3000;  // ms per test point
const int cooldownTime = 5000;  // ms between tests

// Servo object
Servo esc;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Automated Sweep Test ===\n");
  
  // Initialize PWM
  esc.setPeriodHertz(PWM_FREQUENCY);
  esc.attach(MOTOR_PIN, PWM_MIN, PWM_MAX);
  esc.writeMicroseconds(PWM_MIN);
  
  Serial.println("ESC initialized");
  Serial.println("Commands:");
  Serial.println("  SWEEP - Start automated sweep test");
  Serial.println();
  Serial.println("WARNING: Ensure propeller area is clear!");
  Serial.println();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();
    
    if (command == "SWEEP") {
      performSweepTest();
    }
  }
}

void performSweepTest() {
  Serial.println("\n+========================================================+");
  Serial.println("|  AUTOMATED THRUST SWEEP TEST                          |");
  Serial.println("+========================================================+");
  Serial.println();
  Serial.println("Test will run at following throttle points:");
  for (int i = 0; i < numPoints; i++) {
    Serial.printf("  %d%% ", testPoints[i]);
  }
  Serial.println();
  Serial.println();
  Serial.println("Starting in 5 seconds...");
  Serial.println("Type STOP to abort");
  Serial.println();
  
  delay(5000);
  
  // Run test sequence
  for (int i = 0; i < numPoints; i++) {
    int throttle = testPoints[i];
    
    Serial.println("+========================================================+");
    Serial.printf("  TEST POINT %d/%d: %d%% THROTTLE\n", i+1, numPoints, throttle);
    Serial.println("+========================================================+");
    
    // Set throttle
    int pulseWidth = map(throttle, 0, 100, PWM_MIN, PWM_MAX);
    esc.writeMicroseconds(pulseWidth);
    
    Serial.println();
    Serial.printf("Throttle: %d%%\n", throttle);
    Serial.println("Recording data for 3 seconds...");
    Serial.println("*** RECORD THRUST READING NOW ***");
    Serial.println();
    
    // Hold throttle for test duration
    unsigned long startTime = millis();
    while (millis() - startTime < testDuration) {
      // Check for abort command
      if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        cmd.toUpperCase();
        if (cmd == "STOP") {
          Serial.println("\n[ABORT] Test stopped by user");
          esc.writeMicroseconds(PWM_MIN);
          return;
        }
      }
      delay(100);
    }
    
    // Return to zero throttle
    esc.writeMicroseconds(PWM_MIN);
    
    Serial.println("[DONE] Test point complete");
    
    // Cooldown between tests (except after last test)
    if (i < numPoints - 1) {
      Serial.println();
      Serial.printf("Cooling down for %d seconds...\n", cooldownTime / 1000);
      Serial.println();
      delay(cooldownTime);
    }
  }
  
  // Final shutdown
  esc.writeMicroseconds(PWM_MIN);
  
  Serial.println();
  Serial.println("+========================================================+");
  Serial.println("|  SWEEP TEST COMPLETE                                  |");
  Serial.println("+========================================================+");
  Serial.println();
  Serial.println("All test points completed successfully!");
  Serial.println("Motor stopped and disarmed.");
  Serial.println();
}
