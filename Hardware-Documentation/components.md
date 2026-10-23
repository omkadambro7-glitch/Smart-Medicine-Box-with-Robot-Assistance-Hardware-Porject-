# Hardware Components Specification: Smart Medicine Box with Robot Assistance

**Project:** Smart Medicine Box with Robot Assistance  
**Institution:** St. John College of Engineering & Management  
**Department:** Computer Science and Engineering (Data Science)  
**Academic Year:** 2026–2027  
**Team Members:** Om Kadam, Rishabh Sonawane, Pravin Tiwari, Om Tripathi  

---

## 1. Overview & Verification Status

This document catalogues the confirmed hardware bill of materials (BOM), electrical specifications, functional roles, and safety considerations derived from physical prototype inspection, project photographs, demonstration media, and the academic engineering report (`Smart_Medicine_Box_Report_AF52_Final.pdf`).

```
                              +--------------------------+
                              |   12V DC Power Source    |
                              +-------------+------------+
                                            | 12V
                                            v
+-------------------------+  5V logic  +----+---------------------+   12V   +-------------------+
|  5V USB / Power Rail    +----------->|   L298N Motor Driver     +-------->| 2x 12V DC Motors  |
+------------+------------+            +----+---------------------+         +-------------------+
             |                              ^
             | 5V/3.3V                      | GPIO Control
             v                              |
+------------+------------------------------+------------+
|             ESP32 Microcontroller Core                 |
|             (DevKit V1 / AI-Thinker CAM)               |
+------------+---------------+--------------+------------+
             |               |              |
             | GPIO          | GPIO         | PWM (50Hz)
             v               v              v
      +------+-----+   +-----+------+   +---+--------+
      | Active     |   | Alert LED  |   | SG90 Mini  |
      | Buzzer     |   | Indicator  |   | Servo      |
      +------------+   +------------+   +------------+
```

---

## 2. Component Bill of Materials (BOM)

| Component | Specification / Rating | Quantity | Physical Verification Status | Role in Prototype |
| :--- | :--- | :---: | :--- | :--- |
| **ESP32 Controller** | Dual-core Tensilica Xtensa 32-bit LX6, 240 MHz, 3.3V logic | 1 | **Confirmed** (ESP32-CAM mounted on robot front; DevKit supported) | Master embedded controller running alert state machine & motor driver |
| **L298N Motor Driver** | Dual full-bridge H-bridge driver, 2A peak per bridge | 1 | **Confirmed** (Red module with black heatsink mounted in chassis) | Interfaces 3.3V logic signals with high-current 12V DC motors |
| **DC Gear Motors** | 12V DC, 300 RPM geared motors | 2 | **Confirmed** (Mounted to lower chassis driving side wheels) | Differential drive locomotion for the robot platform |
| **Mini Servo Motor** | TowerPro SG90 (9g micro servo), 4.8V–6.0V, 50 Hz PWM | 1 | **Confirmed** (Mounted at head/neck joint) | Horizontal pan actuation for robot head/eyes |
| **Active Piezo Buzzer** | 3.3V–5V active acoustic buzzer (built-in oscillator) | 1 | **Confirmed** in report & demo flow | Audible medicine alert for Compartment 1 |
| **Alert LED Indicator** | 5mm High-brightness LED with 220Ω / 330Ω resistor | 1 | **Confirmed** in report & demo flow | Visual synchronous blinking alert for Compartment 1 |
| **Acrylic Enclosure** | Yellow tinted transparent acrylic sheet assembly | 1 | **Confirmed** (Photo Fig A.1 & A.2) | Main structural robot body and compartment housing |
| **3D-Printed Parts** | PLA / PETG custom wheels, tracks, and face assembly | Multiple | **Confirmed** (Grey wheels & white eye goggles) | Custom mechanical traction and WALL-E inspired face styling |
| **Medicine Storage** | Multi-compartment medicine organizer box | 1 | **Confirmed** in project scope | Physical storage for compartmentalised prescriptions |
| **Power Supply System** | 12V DC battery pack / supply + 5V USB line | Dual rail | **Confirmed** (Separate motor and logic power lines) | Powers motors (12V) and microcontroller/sensors (5V/3.3V) |

---

## 3. Detailed Component Technical Profiles

### 3.1. ESP32 Microcontroller
- **Architecture:** Tensilica Xtensa 32-bit LX6 Dual-Core up to 240 MHz.
- **Memory:** 520 KB SRAM, 4 MB external flash memory.
- **Physical Variants Supported:**
  1. *ESP32 DevKit V1 (30/38 pin):* Standard prototyping board with CP2102/CH340 USB-UART bridge.
  2. *AI-Thinker ESP32-CAM:* Compact board equipped with OV2640 camera interface and micro-SD socket. Mounted directly on the front panel of the physical chassis.
- **Electrical Warning:** All ESP32 GPIO pins operate strictly at **3.3V logic levels**. Never connect 5V or 12V directly to any ESP32 input pin.

### 3.2. L298N Dual H-Bridge Motor Driver
- **Driver Chip:** STMicroelectronics L298N.
- **Operating Modes:**
  - *Direction Control:* Driven by 4 inputs (`IN1`, `IN2`, `IN3`, `IN4`).
  - *Speed Control:* Controlled by `ENA` and `ENB`. In default student prototypes, onboard jumper caps tie `ENA` and `ENB` to 5V, providing 100% duty cycle. The firmware supports both jumpered mode and optional PWM speed modulation.
- **Onboard 5V Regulator (78M05):** When the 12V jumper is in place, the driver regulates 12V down to 5V. However, to prevent electrical ground noise and voltage dips during motor startup, the ESP32 should always be powered from a clean USB source with common grounds connected.

### 3.3. 12V 300 RPM DC Geared Motors
- **Rated Operating Voltage:** 12V DC (nominal operational range: 6V–12V).
- **Rated No-Load Speed:** 300 RPM.
- **Torque:** High-torque reduction gearbox suitable for driving 3D-printed wheels with rubber traction rings.
- **Drive Configuration:** 2-wheel differential steering with passive rear/front stability balance.

### 3.4. TowerPro SG90 Mini Servo
- **Weight:** 9 grams.
- **Operating Voltage:** 4.8V to 6.0V DC.
- **Stall Torque:** 1.8 kg·cm at 4.8V.
- **Control Signal:** Standard RC PWM at 50 Hz (20 ms period).
  - 0.5 ms pulse = 0°
  - 1.5 ms pulse = 90° (Center position)
  - 2.5 ms pulse = 180°
- **Software Clamping:** Firmware enforces a safe operating window of **30° to 150°** to prevent over-extension and mechanical binding.

### 3.5. Active Buzzer & Alert LED
- **Audible Transducer:** Active piezo buzzer with an internal multi-vibrator circuit. Setting the GPIO `HIGH` produces a clear 2.3 kHz alert tone without requiring high-frequency software frequency synthesis.
- **Visual Alert:** High-efficiency LED driven in parallel cadence with the buzzer. A 220Ω series resistor limits current to ~10 mA, protecting the microcontroller output pin.

---

## 4. Hardware Safety & Best Practices

1. **Common Ground Connection (Mandatory):**  
   The `GND` pin of the ESP32 **must** be connected to the `GND` terminal of the L298N motor driver. Without a common ground reference, control signals float, causing unpredictable motor actuation or microcontroller crashes.
2. **Never Power Motors Directly from ESP32 GPIOs:**  
   The ESP32 pins are rated for a maximum of 12 mA per pin (40 mA absolute maximum across all pins). DC motors draw up to 1.5A under stall or acceleration, which will instantly destroy the microcontroller if wired directly.
3. **Separate Power Rails:**  
   Motors must be powered from the dedicated 12V rail. The microcontroller must be powered from clean 5V USB power.
4. **Failsafe Motor Boot:**  
   During ESP32 boot, floating GPIO pins can trigger momentary motor pulses. The firmware initializes all motor direction pins to `LOW` immediately in `initPins()`.
