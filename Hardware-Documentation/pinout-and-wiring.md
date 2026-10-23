# Pinout and Wiring Specification: Smart Medicine Box with Robot Assistance

**Project:** Smart Medicine Box with Robot Assistance  
**Institution:** St. John College of Engineering & Management  
**Department:** Computer Science and Engineering (Data Science)  
**Academic Year:** 2026–2027  
**Team Members:** Om Kadam, Rishabh Sonawane, Pravin Tiwari, Om Tripathi  

---

## 1. Electrical Architecture & Wiring Diagram

The physical prototype separates high-current motor power (12V) from sensitive logic signals (3.3V / 5V) to ensure electrical stability and prevent controller brownouts:

```text
       +-------------------------------------------------------------+
       |               POWER SUPPLY & DISTRIBUTION                   |
       +-------------------------------------------------------------+
       |                                                             |
       |  [12V Battery / Adapter (+)] ----------------> L298N (+12V) |
       |                                                             |
       |  [12V Battery / Adapter (-)] ----+-----------> L298N (GND)  |
       |                                  |                          |
       |                                  +-----------> ESP32 (GND)  |  <-- MANDATORY COMMON GND
       |                                                             |
       |  [5V USB Computer Port / Bank] --------------> ESP32 (5V/VIN)
       +-------------------------------------------------------------+

       +-------------------------------------------------------------+
       |             LOGIC SIGNAL CONNECTIONS (ESP32)                |
       +-------------------------------------------------------------+
       |                                                             |
       |   ESP32 DevKit (or CAM)            Target Subsystem         |
       |  -------------------------        ------------------------  |
       |   GPIO 23 (CAM: GPIO 12)  ------> Buzzer (+) [Active]       |
       |   GPIO 22 (CAM: GPIO 33)  ------> LED (+) [via 220 Ohm]     |
       |                                                             |
       |   GPIO 18 (CAM: GPIO 14)  ------> L298N IN1 (Left Fwd)      |
       |   GPIO 19 (CAM: GPIO 15)  ------> L298N IN2 (Left Rev)      |
       |   GPIO 16 (CAM: GPIO 13)  ------> L298N IN3 (Right Fwd)     |
       |   GPIO 17 (CAM: GPIO 16)  ------> L298N IN4 (Right Rev)     |
       |                                                             |
       |   GPIO 13 (CAM: GPIO 2)   ------> SG90 Servo Signal (PWM)   |
       +-------------------------------------------------------------+
```

---

## 2. GPIO Pinout Mapping Tables

### Table 2.1: Standard ESP32 DevKit V1 (30/38-Pin)

| Module / Component | ESP32 Pin | Signal Type | Electrical Characteristics | Verification Status | Notes |
| :--- | :---: | :---: | :--- | :--- | :--- |
| **Active Buzzer (+)** | **GPIO 23** | Digital Out | 3.3V Logic, Active HIGH | *Provisional default* | Buzzer (-) connects to ESP32 GND |
| **Alert LED (+)** | **GPIO 22** | Digital Out | 3.3V Logic, Active HIGH | *Provisional default* | In series with 220Ω resistor to GND |
| **L298N IN1** | **GPIO 18** | Digital Out | 3.3V Logic | *Provisional default* | Controls Left Motor Forward direction |
| **L298N IN2** | **GPIO 19** | Digital Out | 3.3V Logic | *Provisional default* | Controls Left Motor Reverse direction |
| **L298N IN3** | **GPIO 16** | Digital Out | 3.3V Logic (RX2 pin) | *Provisional default* | Controls Right Motor Forward direction |
| **L298N IN4** | **GPIO 17** | Digital Out | 3.3V Logic (TX2 pin) | *Provisional default* | Controls Right Motor Reverse direction |
| **L298N ENA (Left)** | *GPIO 25* | PWM (LEDC 0) | 3.3V PWM (1 kHz) | *Optional* | Remove jumper cap if using PWM speed |
| **L298N ENB (Right)** | *GPIO 26* | PWM (LEDC 1) | 3.3V PWM (1 kHz) | *Optional* | Remove jumper cap if using PWM speed |
| **SG90 Servo Signal** | **GPIO 13** | PWM (LEDC 2) | 50 Hz RC PWM | *Provisional default* | Servo VCC to 5V; GND to common GND |

> **Provisional Assignment Clarification:** As documented in Chapter 3 of the project report, the exact physical GPIO jumper pin choices vary based on the specific breadboard layout used during assembly. These assignments are safe, conflict-free defaults. If your physical prototype uses different pins, edit `config.h` to match your jumper wires.

---

### Table 2.2: AI-Thinker ESP32-CAM Configuration

The AI-Thinker ESP32-CAM (as photographed on the robot chest) has fewer exposed pins due to camera and SD card routing:

| Module / Component | ESP32-CAM Pin | Signal Type | Electrical Characteristics | Verification Status | Notes |
| :--- | :---: | :---: | :--- | :--- | :--- |
| **Active Buzzer** | **GPIO 12** | Digital Out | 3.3V Logic | *Provisional default* | Keep LOW at boot to avoid strapping issue |
| **Alert LED** | **GPIO 33** | Digital Out | Active LOW (Onboard red LED) | *Confirmed* | Small red LED on back of ESP32-CAM |
| **L298N IN1** | **GPIO 14** | Digital Out | 3.3V Logic | *Provisional default* | Left Motor Forward |
| **L298N IN2** | **GPIO 15** | Digital Out | 3.3V Logic | *Provisional default* | Left Motor Reverse |
| **L298N IN3** | **GPIO 13** | Digital Out | 3.3V Logic | *Provisional default* | Right Motor Forward |
| **L298N IN4** | **GPIO 16** | Digital Out | 3.3V Logic (U2RXD) | *Provisional default* | Right Motor Reverse |
| **SG90 Servo Signal** | **GPIO 2** | PWM (LEDC 2) | 50 Hz RC PWM | *Provisional default* | Pulled down at boot |

---

## 3. Step-by-Step Hardware Verification Procedure

Before connecting the 12V battery or powering the robot, complete these verification steps:

1. **Verify Common Ground with a Multimeter:**
   - Set multimeter to continuity / resistance mode.
   - Place one probe on the ESP32 `GND` pin and the other probe on the L298N `GND` screw terminal.
   - Ensure resistance is < 1Ω (continuous beep).
2. **Verify Motor Wiring Polarity:**
   - Elevate the robot chassis so the wheels do not touch the ground.
   - Send `'F'` (Forward) command over Serial Monitor.
   - Verify that both left and right wheels rotate forward.
   - If one wheel rotates backward, swap the two motor wires (`OUT1`/`OUT2` or `OUT3`/`OUT4`) on that motor's L298N terminal block.
3. **Verify Servo Movement and Center Position:**
   - Before attaching the mechanical head horn to the servo, send command `'2'` (Look Center, 90°).
   - Once the servo shaft has centered, press the 3D-printed head horn onto the shaft facing straight forward.
   - Send `'1'` (Look Left, 135°) and `'3'` (Look Right, 45°) to confirm smooth mechanical motion without hitting mechanical stops.
4. **Verify Buzzer and LED Alert Cadence:**
   - Send command `'A'` (Manual Alert).
   - Confirm that the buzzer sounds in 3 distinct beeps and the LED flashes in exact synchrony.
   - Send command `'K'` to confirm that the alert immediately silences and the dose is logged.

---

## 4. Common Wiring Traps & Pitfalls

| Symptom | Probable Cause | Corrective Action |
| :--- | :--- | :--- |
| **Motors do not spin at all** | Missing common GND, or ENA/ENB jumpers missing without PWM wired | Connect ESP32 GND to L298N GND; ensure 5V jumper caps are firmly seated on ENA and ENB headers |
| **ESP32 reboots when motors start** | Motor current spike causing 5V voltage drop | Never power ESP32 from the motor battery without a separate voltage regulator; power ESP32 via USB |
| **Servo shakes or vibrates** | Insufficient current on 5V rail or missing common ground | Power servo from external 5V rail, not ESP32 3.3V pin; ensure ground wire connects to ESP32 GND |
| **Buzzer tone is faint or distorted** | Passive buzzer used instead of active buzzer | Replace with active piezo buzzer, or use software `tone()` function |
| **Robot turns in opposite direction** | Inverted motor channel assignment in software or wiring | Swap IN1/IN2 with IN3/IN4 in `config.h`, or swap motor terminal leads on L298N |
