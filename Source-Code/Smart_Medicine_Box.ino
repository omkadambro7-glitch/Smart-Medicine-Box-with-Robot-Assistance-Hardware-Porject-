/**
 * ============================================================================
 * PROJECT: Smart Medicine Box with Robot Assistance
 * INSTITUTION: St. John College of Engineering & Management
 * DEPARTMENT: Computer Science and Engineering (Data Science)
 * ACADEMIC YEAR: 2026-2027
 * 
 * AUTHORS & ROLES:
 *   - Om Kadam         : Coordination, requirements and integration
 *   - Rishabh Sonawane : Assembly support and interface documentation
 *   - Pravin Tiwari    : Controller, motor-control and firmware support
 *   - Om Tripathi      : Testing records, diagrams and report support
 * 
 * REPOSITORY:
 *   https://github.com/omkadam7-glitch/Smart-Medicine-Box-with-Robot-Assistance-Hardware-Project-
 * ============================================================================
 * 
 * SYSTEM ARCHITECTURE & IMPLEMENTATION DESCRIPTION:
 * This embedded firmware implements the dual-function prototype:
 *   1. Medicine Reminder Engine:
 *      - Configurable scheduled alerts for Compartment 1.
 *      - Non-blocking state machine driving synchronous audible (buzzer)
 *        and visual (LED) alert bursts.
 *      - Support for manual trigger, snoozing, acknowledge, and auto-timeout.
 *   2. Robotic Platform Controller:
 *      - Differential drive motor control via L298N dual H-bridge.
 *      - Startup failsafe (motors default to STOP state).
 *      - Auto-stop watchdog timeout if communication is lost.
 *      - SG90 mini servo head pan/tilt control with mechanical safety limits.
 *   3. Wired USB Serial Interface:
 *      - Real-time command interface for hardware testing, status monitoring,
 *        and scheduled alert demonstration.
 * ============================================================================
 */

#include <Arduino.h>
#include "config.h"

// ============================================================================
// SYSTEM ENUMS & DATA STRUCTURES
// ============================================================================

enum ReminderState {
  STATE_IDLE_WAITING,       // Counting down until next scheduled alert
  STATE_ALERTING,           // Buzzer sounding and LED blinking
  STATE_ACKNOWLEDGED,       // User confirmed medicine taken
  STATE_SNOOZED             // Temporarily silenced, alert re-arms after snooze duration
};

enum RobotDirection {
  ROBOT_STOPPED,
  ROBOT_FORWARD,
  ROBOT_BACKWARD,
  ROBOT_LEFT,
  ROBOT_RIGHT,
  ROBOT_PIVOT_LEFT,
  ROBOT_PIVOT_RIGHT
};

// ============================================================================
// GLOBAL STATE VARIABLES
// ============================================================================

// Medicine Reminder State
ReminderState currentReminderState = STATE_IDLE_WAITING;
unsigned long reminderIntervalMs = (unsigned long)DEFAULT_REMINDER_INTERVAL_SEC * 1000UL;
unsigned long lastReminderTimeMs = 0;
unsigned long alertStartTimeMs = 0;
unsigned long lastAlertToggleMs = 0;

uint8_t alertPulseCount = 0;
bool alertOutputActive = false;
unsigned long totalDosesAlerted = 0;
unsigned long totalDosesAcknowledged = 0;

// Robot Movement & Failsafe State
RobotDirection currentDirection = ROBOT_STOPPED;
unsigned long lastMotorCommandTimeMs = 0;
uint8_t currentMotorSpeed = MOTOR_DEFAULT_SPEED;

// Head Servo State
int currentHeadAngle = SERVO_ANGLE_CENTER;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
void initPins();
void initServoLEDC();
void setHeadAngle(int angleDegrees);

void motorStop();
void motorForward(uint8_t speed);
void motorBackward(uint8_t speed);
void motorTurnLeft(uint8_t speed);
void motorTurnRight(uint8_t speed);
void motorPivotLeft(uint8_t speed);
void motorPivotRight(uint8_t speed);

void startAlert();
void stopAlert(const char* reason);
void snoozeAlert(unsigned long snoozeSec);
void updateMedicineReminder();
void updateMotorFailsafe();

void processSerialCommands();
void printBanner();
void printHelpMenu();
void printSystemStatus();

// ============================================================================
// HARDWARE INITIALISATION
// ============================================================================

void initPins() {
  // Alert Outputs
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_ALERT, OUTPUT);
  
  // Safe initial state: Alert OFF
  digitalWrite(PIN_BUZZER, LOW);
  #if (TARGET_BOARD == BOARD_ESP32_CAM)
    // ESP32-CAM onboard red LED (GPIO 33) is Active LOW
    digitalWrite(PIN_LED_ALERT, HIGH);
  #else
    digitalWrite(PIN_LED_ALERT, LOW);
  #endif

  // L298N Motor Driver Inputs
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_MOTOR_IN3, OUTPUT);
  pinMode(PIN_MOTOR_IN4, OUTPUT);

  // CRITICAL FAILSAFE: Ensure motors are completely stopped on boot
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);

  // Optional PWM pins if enabled and valid
  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcSetup(MOTOR_PWM_CHANNEL_LEFT, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
      ledcSetup(MOTOR_PWM_CHANNEL_RIGHT, MOTOR_PWM_FREQ, MOTOR_PWM_RESOLUTION);
      ledcAttachPin(PIN_MOTOR_ENA, MOTOR_PWM_CHANNEL_LEFT);
      ledcAttachPin(PIN_MOTOR_ENB, MOTOR_PWM_CHANNEL_RIGHT);
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, 0);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, 0);
    #endif
  #endif
}

#if defined(ESP_ARDUINO_VERSION_MAJOR) && (ESP_ARDUINO_VERSION_MAJOR >= 3)
  #define USE_LEDC_V3 1
#else
  #define USE_LEDC_V2 1
#endif

/**
 * Configure hardware timer PWM (LEDC) for SG90 servo motor.
 * Uses native ESP32 LEDC peripheral (no external library required).
 * Fully compatible with both ESP32 Arduino Core 2.x and 3.x.
 */
void initServoLEDC() {
  #if (PIN_SERVO_HEAD >= 0)
    #if defined(USE_LEDC_V3)
      ledcAttach(PIN_SERVO_HEAD, SERVO_PWM_FREQ, SERVO_PWM_RESOLUTION);
    #else
      ledcSetup(SERVO_PWM_CHANNEL, SERVO_PWM_FREQ, SERVO_PWM_RESOLUTION);
      ledcAttachPin(PIN_SERVO_HEAD, SERVO_PWM_CHANNEL);
    #endif
    setHeadAngle(SERVO_ANGLE_CENTER);
  #endif
}

/**
 * Set Head/Neck Servo position (Degrees: SERVO_ANGLE_MIN to SERVO_ANGLE_MAX)
 */
void setHeadAngle(int angleDegrees) {
  // Enforce mechanical limit boundaries
  if (angleDegrees < SERVO_ANGLE_MIN) angleDegrees = SERVO_ANGLE_MIN;
  if (angleDegrees > SERVO_ANGLE_MAX) angleDegrees = SERVO_ANGLE_MAX;
  currentHeadAngle = angleDegrees;

  // Map angle (0 to 180) to pulse width (500us to 2500us)
  long pulseUs = map(angleDegrees, 0, 180, SERVO_MIN_PULSE_US, SERVO_MAX_PULSE_US);
  
  // Calculate 16-bit LEDC duty cycle: Duty = (pulseUs * 65535) / 20000us
  uint32_t duty = (uint32_t)((pulseUs * 65535ULL) / 20000ULL);

  #if defined(USE_LEDC_V3)
    ledcWrite(PIN_SERVO_HEAD, duty);
  #else
    ledcWrite(SERVO_PWM_CHANNEL, duty);
  #endif
}

// ============================================================================
// MOTOR CONTROL PRIMITIVES (L298N DUAL H-BRIDGE)
// ============================================================================

void motorStop() {
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, 0);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, 0);
    #endif
  #endif

  currentDirection = ROBOT_STOPPED;
}

void motorForward(uint8_t speed) {
  // Left Motor Forward: IN1 = HIGH, IN2 = LOW
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  // Right Motor Forward: IN3 = HIGH, IN4 = LOW
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, speed);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, speed);
    #endif
  #endif

  currentDirection = ROBOT_FORWARD;
  lastMotorCommandTimeMs = millis();
}

void motorBackward(uint8_t speed) {
  // Left Motor Backward: IN1 = LOW, IN2 = HIGH
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  // Right Motor Backward: IN3 = LOW, IN4 = HIGH
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, speed);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, speed);
    #endif
  #endif

  currentDirection = ROBOT_BACKWARD;
  lastMotorCommandTimeMs = millis();
}

void motorTurnLeft(uint8_t speed) {
  // Smooth turn left: left motor stopped, right motor forward
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, 0);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, speed);
    #endif
  #endif

  currentDirection = ROBOT_LEFT;
  lastMotorCommandTimeMs = millis();
}

void motorTurnRight(uint8_t speed) {
  // Smooth turn right: left motor forward, right motor stopped
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, LOW);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, speed);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, 0);
    #endif
  #endif

  currentDirection = ROBOT_RIGHT;
  lastMotorCommandTimeMs = millis();
}

void motorPivotLeft(uint8_t speed) {
  // Pivot left on axis: left motor backward, right motor forward
  digitalWrite(PIN_MOTOR_IN1, LOW);
  digitalWrite(PIN_MOTOR_IN2, HIGH);
  digitalWrite(PIN_MOTOR_IN3, HIGH);
  digitalWrite(PIN_MOTOR_IN4, LOW);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, speed);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, speed);
    #endif
  #endif

  currentDirection = ROBOT_PIVOT_LEFT;
  lastMotorCommandTimeMs = millis();
}

void motorPivotRight(uint8_t speed) {
  // Pivot right on axis: left motor forward, right motor backward
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_MOTOR_IN3, LOW);
  digitalWrite(PIN_MOTOR_IN4, HIGH);

  #if (MOTOR_ENABLE_PWM == true)
    #if (PIN_MOTOR_ENA >= 0 && PIN_MOTOR_ENB >= 0)
      ledcWrite(MOTOR_PWM_CHANNEL_LEFT, speed);
      ledcWrite(MOTOR_PWM_CHANNEL_RIGHT, speed);
    #endif
  #endif

  currentDirection = ROBOT_PIVOT_RIGHT;
  lastMotorCommandTimeMs = millis();
}

// ============================================================================
// MEDICINE REMINDER LOGIC (NON-BLOCKING)
// ============================================================================

void startAlert() {
  currentReminderState = STATE_ALERTING;
  alertStartTimeMs = millis();
  lastAlertToggleMs = millis();
  alertPulseCount = 0;
  alertOutputActive = true;
  totalDosesAlerted++;

  // Turn ON buzzer and LED
  digitalWrite(PIN_BUZZER, HIGH);
  #if (TARGET_BOARD == BOARD_ESP32_CAM)
    digitalWrite(PIN_LED_ALERT, LOW); // Active LOW on ESP32-CAM GPIO 33
  #else
    digitalWrite(PIN_LED_ALERT, HIGH);
  #endif

  Serial.println("\n[ALERT TRIGGERED] >>> Compartment 1: TIME TO TAKE MEDICINE! <<<");
  Serial.println("[ALERT] Audible buzzer active, visual LED flashing.");
  Serial.println("[ALERT] Press 'K' to acknowledge dose, or 'Z' to snooze.");
}

void stopAlert(const char* reason) {
  currentReminderState = STATE_IDLE_WAITING;
  lastReminderTimeMs = millis();

  // De-energise outputs
  digitalWrite(PIN_BUZZER, LOW);
  #if (TARGET_BOARD == BOARD_ESP32_CAM)
    digitalWrite(PIN_LED_ALERT, HIGH);
  #else
    digitalWrite(PIN_LED_ALERT, LOW);
  #endif

  Serial.print("[ALERT STOPPED] Reason: ");
  Serial.println(reason);
}

void snoozeAlert(unsigned long snoozeSec) {
  currentReminderState = STATE_SNOOZED;
  // Calculate last reminder time such that interval expires after snoozeSec
  lastReminderTimeMs = millis() + (snoozeSec * 1000UL) - reminderIntervalMs;

  digitalWrite(PIN_BUZZER, LOW);
  #if (TARGET_BOARD == BOARD_ESP32_CAM)
    digitalWrite(PIN_LED_ALERT, HIGH);
  #else
    digitalWrite(PIN_LED_ALERT, LOW);
  #endif

  Serial.print("[ALERT SNOOZED] Reminder silenced for ");
  Serial.print(snoozeSec);
  Serial.println(" seconds.");
}

/**
 * Non-blocking state machine updating audible/visual alert cadence.
 * Alert pattern: 3 pulses of (200ms ON, 150ms OFF), then 1000ms pause.
 */
void updateMedicineReminder() {
  unsigned long now = millis();

  switch (currentReminderState) {
    case STATE_IDLE_WAITING:
    case STATE_SNOOZED:
      // Check if scheduled interval has elapsed
      if (now - lastReminderTimeMs >= reminderIntervalMs) {
        startAlert();
      }
      break;

    case STATE_ALERTING:
      // Auto-silence safety timeout
      if (now - alertStartTimeMs >= ((unsigned long)ALERT_AUTO_TIMEOUT_SEC * 1000UL)) {
        stopAlert("Auto-silence safety timeout elapsed.");
        break;
      }

      // Cadence timing generator
      if (alertOutputActive) {
        // Currently in ON pulse
        if (now - lastAlertToggleMs >= ALERT_PULSE_ON_MS) {
          // Turn OFF
          digitalWrite(PIN_BUZZER, LOW);
          #if (TARGET_BOARD == BOARD_ESP32_CAM)
            digitalWrite(PIN_LED_ALERT, HIGH);
          #else
            digitalWrite(PIN_LED_ALERT, LOW);
          #endif
          alertOutputActive = false;
          lastAlertToggleMs = now;
          alertPulseCount++;
        }
      } else {
        // Currently in OFF interval
        if (alertPulseCount < ALERT_BURST_PULSES) {
          // Short pause between burst pulses
          if (now - lastAlertToggleMs >= ALERT_PULSE_OFF_MS) {
            digitalWrite(PIN_BUZZER, HIGH);
            #if (TARGET_BOARD == BOARD_ESP32_CAM)
              digitalWrite(PIN_LED_ALERT, LOW);
            #else
              digitalWrite(PIN_LED_ALERT, HIGH);
            #endif
            alertOutputActive = true;
            lastAlertToggleMs = now;
          }
        } else {
          // Long pause after full burst of pulses
          if (now - lastAlertToggleMs >= ALERT_CYCLE_PAUSE_MS) {
            alertPulseCount = 0;
            digitalWrite(PIN_BUZZER, HIGH);
            #if (TARGET_BOARD == BOARD_ESP32_CAM)
              digitalWrite(PIN_LED_ALERT, LOW);
            #else
              digitalWrite(PIN_LED_ALERT, HIGH);
            #endif
            alertOutputActive = true;
            lastAlertToggleMs = now;
          }
        }
      }
      break;

    case STATE_ACKNOWLEDGED:
      // Armed for next scheduled cycle
      currentReminderState = STATE_IDLE_WAITING;
      lastReminderTimeMs = now;
      break;
  }
}

/**
 * Watchdog failsafe: automatically stop motors if no command received
 * within MOTOR_COMMAND_TIMEOUT_MS during active driving.
 */
void updateMotorFailsafe() {
  if (currentDirection != ROBOT_STOPPED) {
    if (millis() - lastMotorCommandTimeMs >= MOTOR_COMMAND_TIMEOUT_MS) {
      motorStop();
      Serial.println("[SAFETY WATCHDOG] Motor command timeout - Robot stopped.");
    }
  }
}

// ============================================================================
// SERIAL USER INTERFACE & COMMAND PARSER
// ============================================================================

void printBanner() {
  Serial.println("\n=======================================================");
  Serial.println("   SMART MEDICINE BOX WITH ROBOT ASSISTANCE");
  Serial.println("   St. John College of Engineering & Management");
  Serial.println("   Dept. of CSE (Data Science) | Academic Year 2026-2027");
  Serial.println("   Team: Om Kadam, Rishabh Sonawane,");
  Serial.println("         Pravin Tiwari, Om Tripathi");
  Serial.println("=======================================================");
  #if (TARGET_BOARD == BOARD_ESP32_CAM)
    Serial.println("Hardware Profile: AI-Thinker ESP32-CAM");
  #else
    Serial.println("Hardware Profile: ESP32 DevKit (30/38-pin)");
  #endif
  Serial.print("Medicine Compartment 1 Interval: ");
  Serial.print(reminderIntervalMs / 1000UL);
  Serial.println(" seconds");
  Serial.println("Type '?' or 'M' to view available control commands.");
  Serial.println("=======================================================\n");
}

void printHelpMenu() {
  Serial.println("\n----------------- COMMAND HELP MENU -----------------");
  Serial.println("--- ROBOT CHASSIS MOVEMENT ---");
  Serial.println("  F / f : Move Forward");
  Serial.println("  B / b : Move Backward");
  Serial.println("  L / l : Turn Left (Steer)");
  Serial.println("  R / r : Turn Right (Steer)");
  Serial.println("  < / , : Pivot Left (Spin on axis)");
  Serial.println("  > / . : Pivot Right (Spin on axis)");
  Serial.println("  X / x / S / s : STOP ALL MOTORS");
  Serial.println("  + / - : Increase / Decrease motor PWM speed");
  Serial.println("\n--- ROBOT HEAD / SERVO POSITION ---");
  Serial.println("  1     : Look Left (135 deg)");
  Serial.println("  2     : Look Center (90 deg - Forward)");
  Serial.println("  3     : Look Right (45 deg)");
  Serial.println("  H<deg>: Set custom head angle (e.g., 'H90', 'H45', 'H120')");
  Serial.println("\n--- MEDICINE REMINDER CONTROL ---");
  Serial.println("  A / a : Trigger Manual Alert (Test buzzer & LED)");
  Serial.println("  K / k : Acknowledge Dose (Medicine taken & silence alert)");
  Serial.println("  Z / z : Snooze Alert (Silence for 5 minutes / 300 sec)");
  Serial.println("  T<sec>: Set reminder interval (e.g. 'T60' for 60 seconds)");
  Serial.println("\n--- SYSTEM DIAGNOSTICS ---");
  Serial.println("  I / i : Print System Status, Uptime & Hardware Readings");
  Serial.println("  ? / M : Print this Help Menu");
  Serial.println("-----------------------------------------------------");
}

void printSystemStatus() {
  unsigned long now = millis();
  unsigned long uptimeSec = now / 1000UL;
  
  Serial.println("\n================ SYSTEM STATUS ================");
  Serial.print("Uptime: ");
  Serial.print(uptimeSec / 3600);
  Serial.print("h ");
  Serial.print((uptimeSec % 3600) / 60);
  Serial.print("m ");
  Serial.print(uptimeSec % 60);
  Serial.println("s");

  Serial.print("Medicine Reminder State: ");
  switch (currentReminderState) {
    case STATE_IDLE_WAITING:
      Serial.print("WAITING (Next alert in ");
      if (now - lastReminderTimeMs < reminderIntervalMs) {
        Serial.print((reminderIntervalMs - (now - lastReminderTimeMs)) / 1000UL);
      } else {
        Serial.print(0);
      }
      Serial.println(" s)");
      break;
    case STATE_ALERTING:
      Serial.println("ACTIVE ALERTING! (Buzzer ON, LED Blinking)");
      break;
    case STATE_SNOOZED:
      Serial.println("SNOOZED");
      break;
    case STATE_ACKNOWLEDGED:
      Serial.println("ACKNOWLEDGED");
      break;
  }

  Serial.print("Compartment 1 Interval: ");
  Serial.print(reminderIntervalMs / 1000UL);
  Serial.println(" seconds");
  Serial.print("Total Doses Alerted: ");
  Serial.println(totalDosesAlerted);
  Serial.print("Total Doses Acknowledged: ");
  Serial.println(totalDosesAcknowledged);

  Serial.print("Robot Direction: ");
  switch (currentDirection) {
    case ROBOT_STOPPED:     Serial.println("STOPPED"); break;
    case ROBOT_FORWARD:     Serial.println("FORWARD"); break;
    case ROBOT_BACKWARD:    Serial.println("BACKWARD"); break;
    case ROBOT_LEFT:        Serial.println("TURN LEFT"); break;
    case ROBOT_RIGHT:       Serial.println("TURN RIGHT"); break;
    case ROBOT_PIVOT_LEFT:  Serial.println("PIVOT LEFT"); break;
    case ROBOT_PIVOT_RIGHT: Serial.println("PIVOT RIGHT"); break;
  }

  Serial.print("Current Motor Speed: ");
  Serial.println(currentMotorSpeed);
  Serial.print("Head Servo Angle: ");
  Serial.print(currentHeadAngle);
  Serial.println(" deg");
  Serial.println("===============================================");
}

void processSerialCommands() {
  if (!Serial.available()) return;

  String input = Serial.readStringUntil('\n');
  input.trim();
  if (input.length() == 0) return;

  char cmd = input.charAt(0);

  switch (cmd) {
    // Movement Commands
    case 'F':
    case 'f':
      motorForward(currentMotorSpeed);
      Serial.println("Robot: FORWARD");
      break;

    case 'B':
    case 'b':
      motorBackward(currentMotorSpeed);
      Serial.println("Robot: BACKWARD");
      break;

    case 'L':
    case 'l':
      motorTurnLeft(currentMotorSpeed);
      Serial.println("Robot: TURN LEFT");
      break;

    case 'R':
    case 'r':
      motorTurnRight(currentMotorSpeed);
      Serial.println("Robot: TURN RIGHT");
      break;

    case '<':
    case ',':
      motorPivotLeft(currentMotorSpeed);
      Serial.println("Robot: PIVOT LEFT");
      break;

    case '>':
    case '.':
      motorPivotRight(currentMotorSpeed);
      Serial.println("Robot: PIVOT RIGHT");
      break;

    case 'X':
    case 'x':
    case 'S':
    case 's':
      motorStop();
      Serial.println("Robot: STOPPED");
      break;

    case '+':
      if (currentMotorSpeed <= 235) currentMotorSpeed += 20;
      else currentMotorSpeed = 255;
      Serial.print("Speed set to: ");
      Serial.println(currentMotorSpeed);
      break;

    case '-':
      if (currentMotorSpeed >= 40) currentMotorSpeed -= 20;
      else currentMotorSpeed = 20;
      Serial.print("Speed set to: ");
      Serial.println(currentMotorSpeed);
      break;

    // Servo Head Control
    case '1':
      setHeadAngle(135);
      Serial.println("Head: LOOK LEFT (135 deg)");
      break;

    case '2':
      setHeadAngle(SERVO_ANGLE_CENTER);
      Serial.println("Head: LOOK CENTER (90 deg)");
      break;

    case '3':
      setHeadAngle(45);
      Serial.println("Head: LOOK RIGHT (45 deg)");
      break;

    case 'H':
    case 'h': {
      if (input.length() > 1) {
        int angle = input.substring(1).toInt();
        setHeadAngle(angle);
        Serial.print("Head angle set to: ");
        Serial.print(currentHeadAngle);
        Serial.println(" deg");
      } else {
        Serial.println("Usage: H<angle> (e.g. H90, H45, H135)");
      }
      break;
    }

    // Medicine Reminder Commands
    case 'A':
    case 'a':
      startAlert();
      break;

    case 'K':
    case 'k':
      if (currentReminderState == STATE_ALERTING) {
        totalDosesAcknowledged++;
        stopAlert("User acknowledged dose.");
        Serial.println("[DOSE CONFIRMED] Compartment 1 medicine taken.");
      } else {
        Serial.println("No active alert to acknowledge.");
      }
      break;

    case 'Z':
    case 'z':
      if (currentReminderState == STATE_ALERTING) {
        snoozeAlert(300); // Snooze for 5 minutes (300 seconds)
      } else {
        Serial.println("No active alert to snooze.");
      }
      break;

    case 'T':
    case 't': {
      if (input.length() > 1) {
        long sec = input.substring(1).toInt();
        if (sec >= 5 && sec <= 86400) {
          reminderIntervalMs = (unsigned long)sec * 1000UL;
          lastReminderTimeMs = millis();
          Serial.print("Reminder interval updated to: ");
          Serial.print(sec);
          Serial.println(" seconds.");
        } else {
          Serial.println("Error: Interval must be between 5 and 86400 seconds.");
        }
      } else {
        Serial.println("Usage: T<seconds> (e.g. T30, T60, T3600)");
      }
      break;
    }

    // Diagnostics & Help
    case 'I':
    case 'i':
      printSystemStatus();
      break;

    case '?':
    case 'M':
    case 'm':
      printHelpMenu();
      break;

    default:
      Serial.print("Unknown command: '");
      Serial.print(cmd);
      Serial.println("'. Type '?' for help.");
      break;
  }
}

// ============================================================================
// ARDUINO SETUP & MAIN LOOP
// ============================================================================

void setup() {
  // Initialize Serial Monitor
  Serial.begin(SERIAL_BAUD_RATE);
  delay(500); // Allow UART voltage stabilisation

  // Initialize Hardware Peripherals
  initPins();
  initServoLEDC();

  // Print startup identification
  printBanner();

  // Arm initial reminder timer
  lastReminderTimeMs = millis();
  Serial.println("[SYSTEM READY] Medicine Reminder Engine & Robot Subsystems Initialised.");
}

void loop() {
  // 1. Update Medicine Reminder Engine (Non-blocking buzzer & LED state machine)
  updateMedicineReminder();

  // 2. Watchdog timeout for motor safety
  updateMotorFailsafe();

  // 3. Process incoming wired USB serial commands
  processSerialCommands();

  // Yield to ESP32 RTOS background housekeeping tasks
  yield();
}
