# Hardware and Software Testing Checklist: Smart Medicine Box with Robot Assistance

**Project:** Smart Medicine Box with Robot Assistance  
**Institution:** St. John College of Engineering & Management  
**Department:** Computer Science and Engineering (Data Science)  
**Academic Year:** 2026–2027  
**Team Members:** Om Kadam, Rishabh Sonawane, Pravin Tiwari, Om Tripathi  

---

## 1. Overview & Testing Philosophy

This testing document provides an objective, repeatable quality-assurance checklist to validate both the **Medicine Reminder Subsystem** and the **Robotic Platform** before project evaluation and viva demonstration.

In compliance with honest engineering standards:
- **Simulated & Syntax Validated:** Code syntax, function logic, and multi-board configurations have been validated via compilation suites.
- **Physical Hardware Tests:** The tests outlined below require physical execution with the assembled hardware prototype. Record actual observations in the provided tables.

---

## 2. Pre-Power Safety Verification (Cold Checks)

*Perform these checks BEFORE connecting any battery or USB cable.*

- [ ] **1. Continuity & Common Ground Check:**  
  Multimeter in continuity mode: verify that ESP32 `GND`, L298N `GND`, and Battery `(-)` are all electrically tied together.  
  *Expected:* Continuous beep / resistance < 1.0 Ω.
- [ ] **2. Power Rail Isolation:**  
  Verify no short circuit between 12V motor supply rail and 3.3V/5V microcontroller rail.  
  *Expected:* Open circuit / Megaohm resistance.
- [ ] **3. Mechanical Clearance Check:**  
  Manually rotate 3D-printed wheels. Ensure no rubbing or binding against the acrylic chassis.  
  *Expected:* Smooth free rotation.
- [ ] **4. Head Servo Horn Freeplay Check:**  
  Ensure the mini servo arm is not forced past mechanical endstops.

---

## 3. Power-On Baseline Checks

- [ ] **5. USB Serial Console Initialization:**  
  Connect USB cable to ESP32. Open Serial Monitor at **115200 baud**.  
  *Expected:* ASCII banner prints cleanly showing project title, college name, team member names, and hardware profile.
- [ ] **6. Default State Motor Safety:**  
  Observe DC motors during initial microcontroller power-up.  
  *Expected:* Motors remain completely stationary. No momentary twitch or spin.
- [ ] **7. Default State Alert Inactive:**  
  Observe Buzzer and Alert LED upon boot.  
  *Expected:* Buzzer is silent, LED is OFF (or steady for active-low indicator).

---

## 4. Medicine Reminder Engine Validation

| Test ID | Test Description | Input / Action | Expected Result | Actual Observation | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **REM-01** | Scheduled Alert Trigger | Wait for configured demonstration interval (e.g. 30s) | Buzzer emits 3 beeps; LED flashes synchronously with buzzer; console logs alert | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **REM-02** | Manual Alert Trigger | Send serial command `'A'` | Immediate activation of buzzer and LED alert cadence | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **REM-03** | Dose Acknowledge | Send serial command `'K'` during active alert | Alert stops immediately; dose count increments; timer resets | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **REM-04** | Alert Snooze | Send serial command `'Z'` during active alert | Alert silences; system logs snooze status; alert re-arms after snooze duration | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **REM-05** | Safety Auto-Timeout | Allow alert to sound without user interaction for 60s | Alert automatically stops after 60s to prevent battery drain | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **REM-06** | Interval Reconfiguration | Send serial command `'T45'` | Console confirms: reminder interval updated to 45 seconds | *Record during hardware test* | [ ] Pass / [ ] Fail |

---

## 5. Robotic Platform & Motor Control Validation

*Elevate the robot on a stand so wheels do not touch the desk during initial tests.*

| Test ID | Test Description | Input / Action | Expected Result | Actual Observation | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **MOT-01** | Forward Drive | Send command `'F'` | Both left and right wheels rotate forward | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-02** | Reverse Drive | Send command `'B'` | Both left and right wheels rotate backward | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-03** | Steer Left | Send command `'L'` | Left wheel stops, right wheel rotates forward | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-04** | Steer Right | Send command `'R'` | Left wheel rotates forward, right wheel stops | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-05** | Pivot Left | Send command `'<'` | Left wheel reverse, right wheel forward (spin on spot) | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-06** | Pivot Right | Send command `'>'` | Left wheel forward, right wheel reverse (spin on spot) | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-07** | Emergency Stop | Send command `'X'` while motors are running | All motors stop instantly | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **MOT-08** | Watchdog Auto-Stop | Send `'F'` and do not send any subsequent commands | Motors run for 3 seconds, then stop automatically | *Record during hardware test* | [ ] Pass / [ ] Fail |

---

## 6. Head/Neck Servo Actuation Validation

| Test ID | Test Description | Input / Action | Expected Result | Actual Observation | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **SRV-01** | Look Center | Send command `'2'` | Head servo moves smoothly to 90° (facing forward) | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **SRV-02** | Look Left | Send command `'1'` | Head servo moves to 135° without mechanical binding | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **SRV-03** | Look Right | Send command `'3'` | Head servo moves to 45° without mechanical binding | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **SRV-04** | Custom Angle Command | Send command `'H60'` | Head servo repositions accurately to 60° | *Record during hardware test* | [ ] Pass / [ ] Fail |
| **SRV-05** | Safety Boundary Clamp | Send command `'H10'` and `'H170'` | Angle clamped to safe limits (30° and 150°) | *Record during hardware test* | [ ] Pass / [ ] Fail |

---

## 7. Viva & Practical Demonstration Script

Use this exact sequence to demonstrate the project smoothly to professors and evaluators:

1. **Step 1: Introduction & Hardware Profile**  
   - Connect USB cable; launch Serial Monitor at 115200 baud.
   - Show the initial banner and run status command `'I'` to show uptime and reminder parameters.
2. **Step 2: Demonstrate Medicine Reminder Functionality**  
   - Point out Compartment 1 on the medicine box.
   - Wait for the scheduled alert timer (or send `'A'` for immediate trigger).
   - Point out the synchronous audible beeping (buzzer) and visual flashing (LED).
   - Send `'K'` (Acknowledge) to simulate the patient taking their medication. Highlight the dose confirmation log.
3. **Step 3: Demonstrate Robot Head Inspection**  
   - Send commands `'1'` (Look Left), `'2'` (Look Center), and `'3'` (Look Right).
   - Demonstrate the robot's expressive WALL-E head motion.
4. **Step 4: Demonstrate Locomotion & Safety Watchdog**  
   - Place robot on floor. Send `'F'` (Forward) -> robot drives forward.
   - Send `'<'` (Pivot Left) -> robot spins smoothly.
   - Send `'F'` and don't press anything -> demonstrate that the 3-second safety watchdog automatically stops the robot to prevent collisions.
5. **Step 5: Conclude with Future Scope**  
   - Explain the staged engineering roadmap: from current wired USB verification to future wireless control and schedule-triggered autonomous travel.

---

## 8. Test Sign-Off & Verification Record

| Evaluator / Role | Name | Signature | Date |
| :--- | :--- | :---: | :---: |
| **Project Coordinator** | Om Kadam | ____________________ | ____________ |
| **Assembly & Hardware Lead** | Rishabh Sonawane | ____________________ | ____________ |
| **Firmware & Control Lead** | Pravin Tiwari | ____________________ | ____________ |
| **Testing & Quality Assurance** | Om Tripathi | ____________________ | ____________ |
| **Project Guide** | Prof. Sachin Sase | ____________________ | ____________ |
