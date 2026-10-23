#ifndef CONFIG_H
#define CONFIG_H

/**
 * ============================================================================
 * PROJECT: Smart Medicine Box with Robot Assistance
 * INSTITUTION: St. John College of Engineering & Management
 * DEPARTMENT: Computer Science and Engineering (Data Science)
 * ACADEMIC YEAR: 2026-2027
 * AUTHORS: Om Kadam, Rishabh Sonawane, Pravin Tiwari, Om Tripathi
 * ============================================================================
 * 
 * HARDWARE CONFIGURATION & PINOUT ASSIGNMENTS
 * 
 * IMPORTANT NOTICE REGARDING WIRING VERIFICATION:
 * As documented in the project engineering report, the physical prototype
 * combines an ESP32 microcontroller with an L298N dual H-bridge motor driver,
 * two 12V 300 RPM DC gear motors, an active piezo buzzer, an alert LED,
 * and an SG90 mini servo for robot head/neck positioning.
 * 
 * Pin assignments marked [CONFIRMED] are mapped to verified controller pins.
 * Pin assignments marked [UNCONFIRMED - DEFAULT PROVISIONAL] are sensible,
 * safe default engineering assignments for the ESP32 DevKit and ESP32-CAM.
 * Verify your physical jumper wire connections before applying motor power!
 */

// ============================================================================
// 1. BOARD SELECTION
// ============================================================================
// Set TARGET_BOARD to either BOARD_ESP32_DEVKIT or BOARD_ESP32_CAM
#define BOARD_ESP32_DEVKIT  1  // Standard 30/38-pin NodeMCU ESP-WROOM-32
#define BOARD_ESP32_CAM     2  // AI-Thinker ESP32-CAM (as mounted on prototype chassis front)

// Select active target board:
#ifndef TARGET_BOARD
#define TARGET_BOARD BOARD_ESP32_DEVKIT
#endif

// ============================================================================
// 2. PIN DEFINITIONS BY BOARD TYPE
// ============================================================================

#if (TARGET_BOARD == BOARD_ESP32_DEVKIT)
  // --- ESP32 DEVKIT V1 / NODE-32S GPIO ASSIGNMENTS ---
  
  // Alert Module (Compartment 1 Medicine Reminder)
  // [UNCONFIRMED - DEFAULT PROVISIONAL]: Verify jumper on breadboard/header
  #define PIN_BUZZER          23  // Active Piezo Buzzer control pin (Digital Output)
  #define PIN_LED_ALERT       22  // High-brightness Alert LED (Digital Output)
  
  // L298N Dual H-Bridge Motor Driver Inputs
  // [UNCONFIRMED - DEFAULT PROVISIONAL]: Verify pins connected to L298N IN1..IN4
  #define PIN_MOTOR_IN1       18  // Left Motor Forward
  #define PIN_MOTOR_IN2       19  // Left Motor Backward
  #define PIN_MOTOR_IN3       16  // Right Motor Forward (or GPIO 16 / RX2)
  #define PIN_MOTOR_IN4       17  // Right Motor Backward (or GPIO 17 / TX2)
  
  // Optional PWM Speed Control Pins (ENA / ENB on L298N)
  // If jumper caps remain on ENA/ENB headers, motors run at full voltage (100% speed)
  #define PIN_MOTOR_ENA       25  // Left Motor Speed PWM (Optional)
  #define PIN_MOTOR_ENB       26  // Right Motor Speed PWM (Optional)

  // Servo Actuator (WALL-E Neck/Head Movement)
  // [UNCONFIRMED - DEFAULT PROVISIONAL]: Verify signal wire to SG90 servo
  #define PIN_SERVO_HEAD      13  // Head pan/tilt servo PWM signal

#elif (TARGET_BOARD == BOARD_ESP32_CAM)
  // --- AI-THINKER ESP32-CAM GPIO ASSIGNMENTS ---
  // Note: On ESP32-CAM, GPIOs 0, 2, 4, 12, 13, 14, 15, 16 are exposed on headers.
  // Pins 1 (TX) and 3 (RX) are used for USB-UART programming/console.
  // Pin 4 is connected to the onboard high-power Flash LED.
  // Pin 33 is the small onboard red status LED (Active LOW).
  
  #define PIN_BUZZER          12  // Active buzzer (Digital Output)
  #define PIN_LED_ALERT       33  // Onboard red LED (Active LOW) or external on GPIO 2
  
  // L298N Motor Driver Inputs
  #define PIN_MOTOR_IN1       14  // Left Motor Direction 1
  #define PIN_MOTOR_IN2       15  // Left Motor Direction 2
  #define PIN_MOTOR_IN3       13  // Right Motor Direction 1
  #define PIN_MOTOR_IN4       16  // Right Motor Direction 2
  
  #define PIN_MOTOR_ENA       -1  // Disabled on ESP32-CAM (keep jumpers installed on L298N)
  #define PIN_MOTOR_ENB       -1  // Disabled on ESP32-CAM (keep jumpers installed on L298N)

  // Servo Actuator
  #define PIN_SERVO_HEAD      2   // Servo signal pin (IO2 has internal pull-down; keep clear at boot)

#else
  #error "Unknown TARGET_BOARD specified in config.h!"
#endif

// ============================================================================
// 3. MEDICINE REMINDER CONFIGURATION
// ============================================================================
// Default schedule for Compartment 1:
// In a clinical application, this is typically hours (e.g. 8 hours = 28800s).
// For academic viva demonstration, default interval is set to 30 seconds for quick testing.
#define DEFAULT_REMINDER_INTERVAL_SEC   30UL    // Demonstration interval in seconds
#define ALERT_BURST_PULSES              3       // Number of beeps/flashes per alert cycle
#define ALERT_PULSE_ON_MS               200     // Buzzer & LED ON duration (ms)
#define ALERT_PULSE_OFF_MS              150     // Buzzer & LED OFF duration between pulses (ms)
#define ALERT_CYCLE_PAUSE_MS            1000    // Pause between pulse bursts (ms)
#define ALERT_AUTO_TIMEOUT_SEC          60UL    // Auto-silence timeout if user does not acknowledge

// ============================================================================
// 4. MOTOR CONTROL & SAFETY CONFIGURATION
// ============================================================================
#define MOTOR_ENABLE_PWM                false   // false = L298N jumpers installed; true = PWM speed via ENA/ENB
#define MOTOR_DEFAULT_SPEED             200     // PWM duty (0 to 255) when PWM is enabled
#define MOTOR_PWM_FREQ                  1000    // PWM frequency in Hz
#define MOTOR_PWM_RESOLUTION            8       // 8-bit resolution (0-255)
#define MOTOR_PWM_CHANNEL_LEFT          0       // LEDC Channel 0
#define MOTOR_PWM_CHANNEL_RIGHT         1       // LEDC Channel 1

// Safety Failsafe: Stop motors automatically if no command received for X seconds
#define MOTOR_COMMAND_TIMEOUT_MS        3000UL  // Auto-stop timeout in milliseconds

// ============================================================================
// 5. SERVO (HEAD PAN/TILT) CONFIGURATION
// ============================================================================
// Safe mechanical travel limits to protect 3D-printed neck joints
#define SERVO_ANGLE_MIN                 30      // Minimum safe pan angle (degrees)
#define SERVO_ANGLE_MAX                 150     // Maximum safe pan angle (degrees)
#define SERVO_ANGLE_CENTER              90      // Facing forward center position (degrees)
#define SERVO_PWM_CHANNEL               2       // LEDC Channel 2 for Servo PWM
#define SERVO_PWM_FREQ                  50      // Standard 50Hz RC servo frequency (20ms period)
#define SERVO_PWM_RESOLUTION           16       // 16-bit timer resolution (0 to 65535)

// Microsecond pulse widths for 50Hz (20000us period):
// Duty = (pulse_us * 65535) / 20000
#define SERVO_MIN_PULSE_US              500     // Pulse width for 0 degrees (us)
#define SERVO_MAX_PULSE_US              2500    // Pulse width for 180 degrees (us)

// ============================================================================
// 6. SERIAL COMMUNICATION CONFIGURATION
// ============================================================================
#define SERIAL_BAUD_RATE                115200  // Recommended baud rate for ESP32 console

#endif // CONFIG_H
