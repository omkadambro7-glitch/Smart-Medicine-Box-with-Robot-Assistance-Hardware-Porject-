# Source Code Documentation: Smart Medicine Box with Robot Assistance

**Project:** Smart Medicine Box with Robot Assistance  
**Institution:** St. John College of Engineering & Management  
**Department:** Computer Science and Engineering (Data Science)  
**Academic Year:** 2026–2027  
**Team:** Om Kadam, Rishabh Sonawane, Pravin Tiwari, Om Tripathi  

---

## 1. Directory Structure

```text
Source-Code/
├── Smart_Medicine_Box.ino       # Primary Arduino/ESP32 firmware implementation
├── config.h                     # Hardware pinout, board targets, timings, and safety settings
├── README.md                    # This documentation file
└── Smart_Medicine_Box/          # Arduino IDE compatible sketch folder
    ├── Smart_Medicine_Box.ino
    └── config.h
```

> **Note on Arduino IDE Compatibility:** Arduino IDE requires the `.ino` file to reside in a folder with the identical name (`Smart_Medicine_Box/Smart_Medicine_Box.ino`). Both locations are maintained for seamless command-line compilation and one-click IDE opening.

---

## 2. Architecture & Design Principles

The embedded firmware is structured into three decoupled, non-blocking operational engines:

```
+-----------------------------------------------------------------------------------+
|                                Arduino loop()                                     |
+------------------------------------+----------------------------------------------+
                                     |
       +-----------------------------+-----------------------------+
       |                             |                             |
       v                             v                             v
+-----------------------+   +-----------------------+   +-----------------------+
|  Medicine Reminder    |   |  Motor Safety Watchdog|   |  Serial Command Engine|
|  - Non-blocking timer |   |  - 3-second auto-stop |   |  - Non-blocking parser|
|  - Cadence generator  |   |  - Startup failsafe   |   |  - Real-time telemetry|
|  - Buzzer & LED pulse |   |  - L298N driver state |   |  - Manual actuation   |
+-----------------------+   +-----------------------+   +-----------------------+
```

### A. Medicine Reminder Engine
- **Independent Cadence:** Unlike naive implementations that use `delay()`, this firmware uses `millis()` timestamp tracking. The processor remains 100% responsive to movement commands and serial inputs during an active alarm.
- **Alert Cadence:** Generates an attention-grabbing pattern: 3 rapid pulses (200 ms ON, 150 ms OFF), followed by a 1000 ms pause, repeating until acknowledged or timed out.
- **Auto-Silence Safety:** Alerts automatically timeout after 60 seconds (`ALERT_AUTO_TIMEOUT_SEC`) to conserve power and prevent acoustic annoyance if the user is away.
- **Dose Confirmation:** Users can acknowledge taking medicine by sending `'K'`, snoozing for 5 minutes by sending `'Z'`, or configuring intervals with `'T<seconds>'`.

### B. Robotic Platform & Motor Safety Engine
- **L298N Dual H-Bridge Driver:** Controls two 12V 300 RPM DC gear motors for differential drive locomotion (Forward, Backward, Steer Left, Steer Right, Pivot Left, Pivot Right).
- **Startup Failsafe:** All motor control pins (`IN1`, `IN2`, `IN3`, `IN4`) are explicitly driven `LOW` during `initPins()` to prevent motor jerk or unintended runaway during microcontroller boot.
- **Watchdog Timeout:** If a movement command is issued and communication is lost or interrupted, the watchdog automatically invokes `motorStop()` after 3000 ms (`MOTOR_COMMAND_TIMEOUT_MS`).

### C. SG90 Head/Neck Servo Actuation
- **Native ESP32 LEDC PWM:** Eliminates external library dependencies. The firmware generates a 50 Hz PWM waveform with 16-bit resolution directly using the ESP32 hardware timer peripheral.
- **Cross-Core Compatibility:** Supports both ESP32 Arduino Core 2.x (`ledcSetup`, `ledcAttachPin`) and Core 3.x (`ledcAttach`, `ledcWrite`).
- **Mechanical Limit Bounds:** Pan angles are clamped between 30° and 150° (center at 90°) to protect 3D-printed neck brackets from mechanical strain.

---

## 3. Configuration & Board Profiles (`config.h`)

Select your hardware board in `config.h`:

```cpp
// Set TARGET_BOARD to either BOARD_ESP32_DEVKIT or BOARD_ESP32_CAM
#define TARGET_BOARD BOARD_ESP32_DEVKIT
```

### Pin Assignment Matrix

| Function | ESP32 DevKit (30/38 Pin) | AI-Thinker ESP32-CAM | Verification Status |
| :--- | :---: | :---: | :--- |
| **Active Buzzer** | GPIO 23 | GPIO 12 | Provisional default; verify breadboard jumper |
| **Alert LED** | GPIO 22 | GPIO 33 (Active LOW) | Provisional default; verify LED resistor pin |
| **Motor Left IN1** | GPIO 18 | GPIO 14 | Provisional default; verify L298N IN1 |
| **Motor Left IN2** | GPIO 19 | GPIO 15 | Provisional default; verify L298N IN2 |
| **Motor Right IN3** | GPIO 16 | GPIO 13 | Provisional default; verify L298N IN3 |
| **Motor Right IN4** | GPIO 17 | GPIO 16 | Provisional default; verify L298N IN4 |
| **Motor Speed ENA/ENB** | GPIO 25, 26 (Optional) | N/A (Keep jumpers on) | Optional PWM (Jumper caps enable 100% speed) |
| **Neck Servo Signal** | GPIO 13 | GPIO 2 | Provisional default; verify SG90 yellow/orange wire |

---

## 4. Serial Command Reference

Connect at **115200 baud** via the Arduino Serial Monitor or any serial terminal:

| Key | Action | Description |
| :---: | :--- | :--- |
| **`F` / `f`** | Move Forward | Both DC motors drive forward |
| **`B` / `b`** | Move Backward | Both DC motors drive reverse |
| **`L` / `l`** | Turn Left | Left motor stops, right motor drives forward |
| **`R` / `r`** | Turn Right | Left motor drives forward, right motor stops |
| **`<` / `,`** | Pivot Left | Left motor reverse, right motor forward (spins on axis) |
| **`>` / `.`** | Pivot Right | Left motor forward, right motor reverse (spins on axis) |
| **`X` / `S`** | Emergency Stop | Immediately de-energises all motor pins |
| **`+` / `-`** | Adjust Speed | Increments or decrements PWM duty cycle (when enabled) |
| **`1`** | Look Left | Moves neck servo to 135° |
| **`2`** | Look Center | Centers neck servo to 90° (straight ahead) |
| **`3`** | Look Right | Moves neck servo to 45° |
| **`H<deg>`** | Custom Angle | Sets servo angle directly (e.g. `H90`, `H60`, `H120`) |
| **`A` / `a`** | Trigger Alert | Manually starts Compartment 1 reminder alert |
| **`K` / `k`** | Acknowledge | Confirms medicine taken; silences alert and resets timer |
| **`Z` / `z`** | Snooze Alert | Silences active alert for 5 minutes (300 seconds) |
| **`T<sec>`** | Set Interval | Updates reminder schedule (e.g. `T30` for 30s demo, `T14400` for 4h) |
| **`I` / `i`** | System Info | Prints uptime, reminder state, motor status, and statistics |
| **`?` / `M`** | Help Menu | Prints full command cheatsheet |

---

## 5. Compilation & Upload Instructions

### Method A: Arduino IDE (GUI)
1. Launch **Arduino IDE** (v1.8.x or v2.x).
2. Open `Source-Code/Smart_Medicine_Box/Smart_Medicine_Box.ino`.
3. Under **Tools > Board**, select **ESP32 Dev Module** (or **AI Thinker ESP32-CAM**).
4. Under **Tools > Port**, select the USB serial port corresponding to your ESP32.
5. Click **Verify** (checkmark) to compile.
6. Click **Upload** (arrow) to flash the microcontroller.
7. Open **Tools > Serial Monitor** and set baud rate to **115200**.

### Method B: Arduino CLI (Terminal)
```bash
# Compile for ESP32 Dev Module
arduino-cli compile --fqbn esp32:esp32:esp32 Source-Code/Smart_Medicine_Box

# Upload to connected ESP32 (replace /dev/cu.usbserial-0001 with your port)
arduino-cli upload -p /dev/cu.usbserial-0001 --fqbn esp32:esp32:esp32 Source-Code/Smart_Medicine_Box
```
