# Smart Medicine Box with Robot Assistance

[![Hardware Prototype](https://img.shields.io/badge/Hardware-Prototype%20Assembled-success)](#)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32%20%2F%20ESP32--CAM-blue)](#)
[![Framework: Arduino](https://img.shields.io/badge/Framework-Arduino%20C%2B%2B-teal)](#)
[![License: Educational](https://img.shields.io/badge/Purpose-College%20Capstone%20Project-orange)](#)

> **Active scheduling alerts built on an embedded robotic foundation.**  
> An assistive hardware prototype combining a scheduled compartmentalised medicine alert engine with a mobile robotic chassis.

---

## 🏛️ Academic Institution & Project Credits

* **Institution:** [St. John College of Engineering and Management (SJCEM)](https://www.aldel.edu.in/), Palghar, Maharashtra, India
* **Department:** Department of Computer Science and Engineering (Data Science)
* **Academic Year:** 2026–2027
* **Project Guide:** Prof. Sachin Sase *(Assistant Professor, Dept. of CSE - Data Science)*
* **Head of Department:** Mrs. Joslyn Gracias
* **Principal:** Dr. Kamal Shah
* **Faculty Acknowledgements:** Prof. Megha Godboley, Prof. Navjyothi Katela, Prof. Kancharla Suresh

### 👥 The Architects Behind the Project

| Name | Roll Number | Student ID | Core Engineering Responsibility |
| :--- | :---: | :---: | :--- |
| **Om Kadam** | 18 | `EU1258047` | Overall Project Coordination, Requirements & Integration |
| **Rishabh Sonawane** | 52 | `EU1258001` | Mechanical Assembly & Hardware Interface Documentation |
| **Pravin Tiwari** | 57 | `EU1258039` | Embedded Controller, Motor Driver & Firmware Architecture |
| **Om Tripathi** | 59 | `EU1258052` | Testing Records, Schematic Diagrams & Quality Assurance |

---

## 📸 Prototype Gallery & Media Links

All original project documents, high-resolution photographs, and prototype demonstration videos are maintained in the repository:

| Asset Description | File Link | Details |
| :--- | :--- | :--- |
| **Final Project Report** | [Smart_Medicine_Box_Report_AF52_Final.pdf](Smart_Medicine_Box_Report_AF52_Final.pdf) | Complete 13-page academic report with architecture and references |
| **Presentation Slides** | [Smart_Robotic_Medicine_Box.pdf](Smart_Robotic_Medicine_Box.pdf) | 12 slide deck covering problem statement, teardown, and roadmap |
| **Robot Prototype Front View** | [WhatsApp Image 2026-10-23 at 17.28.49.jpeg](WhatsApp%20Image%202026-10-23%20at%2017.28.49.jpeg) | Shows WALL-E inspired eyes, ESP32-CAM module, and track wheels |
| **Team Demonstration Setup** | [WhatsApp Image 2026-10-23 at 17.28.49 (1).jpeg](WhatsApp%20Image%202026-10-23%20at%2017.28.49%20%281%29.jpeg) | Team with laptop presentation and robot hardware |
| **High-Resolution Presentation** | [WhatsApp Image 2026-10-23 at 16.15.00.jpeg](WhatsApp%20Image%202026-10-23%20at%2016.15.00.jpeg) | Close-up view of prototype chassis and project members |
| **Prototype Demonstration Video** | [WhatsApp Video 2026-10-23 at 16.14.49.mp4](WhatsApp%20Video%202026-10-23%20at%2016.14.49.mp4) | 10-second video clip demonstrating the assembled prototype |

---

## 🎯 Project Scope & Honest Engineering Boundaries

In accordance with our project declaration, we maintain strict clarity between implemented functions and future enhancements:

```
+--------------------------------------------+    +--------------------------------------------+
|        VERIFIED CURRENT SCOPE              |    |           FUTURE SCOPE ROADMAP             |
|             (Built Today)                  |    |              (Built Tomorrow)              |
+--------------------------------------------+    +--------------------------------------------+
| [x] Multi-compartment medicine storage     |    | [ ] Schedule-triggered autonomous travel   |
| [x] Active Buzzer & LED Compartment 1 alert|    | [ ] Computer-vision pill intake detection  |
| [x] Non-blocking alert state machine       |    | [ ] Mobile cloud notifications & app link  |
| [x] Dual 12V DC motor differential drive   |    | [ ] Obstacle avoidance & SLAM navigation   |
| [x] L298N H-bridge driver with failsafe    |    | [ ] Multi-compartment independent schedules|
| [x] SG90 servo head/neck pan actuation     |    | [ ] IoT WiFi NTP clock synchronisation     |
| [x] Wired USB interactive serial control   |    | [ ] Battery charging dock integration      |
+--------------------------------------------+    +--------------------------------------------+
```

---

## 🧩 Hardware Subsystems & Specifications

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

1. **Microcontroller:** Espressif ESP32 (supports both standard 30/38-pin DevKit V1 and AI-Thinker ESP32-CAM via `config.h`).
2. **Motor Driver:** STMicroelectronics L298N Dual H-Bridge module (high current isolation with common ground).
3. **Locomotion Actuators:** Two 12V 300 RPM DC gear motors driving 3D-printed wheels with rubber traction rings.
4. **Neck/Head Actuator:** TowerPro SG90 9g mini servo providing 30° to 150° horizontal pan motion.
5. **Audible Alert:** Active piezo buzzer with built-in multi-vibrator oscillator.
6. **Visual Alert:** 5mm high-brightness LED with 220Ω series current-limiting resistor.
7. **Body & Enclosure:** Laser-cut translucent acrylic chassis with yellow masking, 3D-printed face/eye brackets, and compartmentalised medicine box.

For comprehensive component data sheets, ratings, and pin tables, refer to:
* 📄 [Hardware Components Specification](Hardware-Documentation/components.md)
* 📄 [Pinout and Wiring Guide](Hardware-Documentation/pinout-and-wiring.md)

---

## 💻 Repository Structure & Source Code

```text
├── README.md                            # Main project overview and documentation index
├── Smart_Medicine_Box_Report_AF52_Final.pdf # Academic final project report
├── Smart_Robotic_Medicine_Box.pdf       # Project presentation slides
├── WhatsApp Image 2026-10-23...         # High-resolution prototype photos
├── WhatsApp Video 2026-10-23...         # Prototype demonstration video
│
├── Source-Code/
│   ├── Smart_Medicine_Box.ino           # Complete embedded firmware implementation
│   ├── config.h                         # Pinouts, board profiles, intervals, and safety limits
│   ├── README.md                        # Firmware architecture and serial command cheatsheet
│   └── Smart_Medicine_Box/              # Standard Arduino IDE sketch folder
│       ├── Smart_Medicine_Box.ino
│       └── config.h
│
├── Hardware-Documentation/
│   ├── components.md                    # Detailed Bill of Materials (BOM) & electrical specs
│   └── pinout-and-wiring.md             # GPIO wiring tables, schematics, and verification steps
│
└── Testing/
    └── test-checklist.md                # Cold checks, functional tests, viva script, sign-off sheet
```

---

## ⚡ Quick Start: Firmware Flashing & Serial Control

### 1. Arduino IDE Setup
1. Open `Source-Code/Smart_Medicine_Box/Smart_Medicine_Box.ino` in Arduino IDE (v1.8.x or v2.x).
2. Select your board from **Tools > Board > esp32**:
   * For standard ESP32: `ESP32 Dev Module`
   * For ESP32-CAM: `AI Thinker ESP32-CAM`
3. If using ESP32-CAM, open `config.h` and change:
   ```cpp
   #define TARGET_BOARD BOARD_ESP32_CAM
   ```
4. Connect the USB cable, select the correct COM / serial port, and click **Upload**.

### 2. Interactive Serial Command Reference
Open the Arduino Serial Monitor at **115200 baud** to operate the robot and reminder system:

| Key | Function | Description |
| :---: | :--- | :--- |
| **`F` / `f`** | Drive Forward | Both DC motors drive forward |
| **`B` / `b`** | Drive Reverse | Both DC motors drive backward |
| **`L` / `l`** | Steer Left | Right motor forward, left motor stop |
| **`R` / `r`** | Steer Right | Left motor forward, right motor stop |
| **`<` / `,`** | Pivot Left | Spins robot on its axis to the left |
| **`>` / `.`** | Pivot Right | Spins robot on its axis to the right |
| **`X` / `S`** | Emergency Stop | Instantly halts all motors |
| **`1`** | Look Left | Repositions head servo to 135° |
| **`2`** | Look Center | Centers head servo to 90° (forward) |
| **`3`** | Look Right | Repositions head servo to 45° |
| **`H<deg>`** | Set Head Angle | Positions servo to specified degree (30° to 150°) |
| **`A` / `a`** | Manual Alert | Immediately triggers Compartment 1 buzzer & LED cadence |
| **`K` / `k`** | Acknowledge Dose | Silences active alert and logs confirmed medicine intake |
| **`Z` / `z`** | Snooze Alert | Silences active alert for 5 minutes (300 seconds) |
| **`T<sec>`** | Set Interval | Configures reminder interval (e.g. `T30` for 30s demo) |
| **`I` / `i`** | System Telemetry| Prints system uptime, dose counters, and motor states |
| **`?` / `M`** | Help Menu | Displays interactive command cheatsheet |

---

## 🧪 Testing, Quality Assurance & Viva Preparation

* Refer to [Testing/test-checklist.md](Testing/test-checklist.md) for pre-power cold checks, operational validation steps, and our chronological **Viva Evaluation Demonstration Script**.
* **Motor Watchdog Protection:** Built-in 3000 ms auto-stop watchdog ensures motors halt automatically if communication is lost during movement.

---

## 🌍 Sustainable Development Goals (SDG) Alignment

* **SDG 3 (Good Health and Well-Being):** Promotes prescription adherence through prominent audible and visual scheduled alerts.
* **SDG 9 (Industry, Innovation and Infrastructure):** Demonstrates an accessible, affordable embedded platform for assistive personal robotics.
* **SDG 12 (Responsible Consumption and Production):** Built using modular, repairable off-the-shelf electronics and reusable acrylic/3D-printed elements.

---

## 📜 Academic Integrity & Honesty Declaration

This repository presents the physical prototype and firmware developed at St. John College of Engineering and Management. All documented features truthfully represent the physical prototype built for the demonstration. No unverified autonomous navigation, unmeasured reliability statistics, or unbuilt cloud features are claimed as completed work.
