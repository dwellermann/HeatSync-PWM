# HeatSync PWM Translator for Viessmann EM-SM1

<p align="center">
  <img src="docs/schematics/logo.svg" alt="HeatSync PWM Translator Logo" width="160"/>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Status-Phase%202%3A%20Prototyping-yellow" alt="Project Status"/>
  <img src="https://img.shields.io/badge/Viessmann-EM--SM1-orange" alt="Viessmann EM-SM1"/>
  <img src="https://img.shields.io/badge/OEG-Pump%20Compatible-blue" alt="OEG Pump Compatible"/>
  <img src="https://img.shields.io/badge/Opto--Isolated-red" alt="Opto-Isolated"/>
</p>

A specific solution to fix the PWM signal incompatibility between the **Viessmann Solar Controller EM-SM1** and modern **OEG circulation pumps**. This adapter acts as an intelligent signal translator, making the two devices compatible.

## The Specific Problem

The Viessmann EM-SM1 solar controller uses an inverted PWM logic to regulate pump speed (e.g., 90% duty cycle means minimum power). Modern, efficient pumps from manufacturers like OEG expect a standard PWM signal (90% duty cycle means maximum power).

This adapter was built to solve this exact problem, acting as a smart "man-in-the-middle" to translate the control signals correctly.

## How It Works

1.  **Read:** It safely measures the duty cycle of the EM-SM1's PWM signal using an opto-isolated input to protect the controller's electronics.
2.  **Translate/Map:** It processes the measured value through a flexible mapping function. This function is pre-configured for the Viessmann/OEG combination but can be easily adjusted.
3.  **Write:** It generates a new, clean 500 Hz PWM signal that the OEG pump understands perfectly.

## Key Feature: The Flexible Translation Logic

This is not a simple inverter. The core of the solution is a programmable mapping logic within the Arduino code. This allows for precise adaptation to the specific characteristics of the heater and pump.

This approach enables:
*   **Simple Logic Inversion:** The primary function of reversing the signal.
*   **Setting Safety Limits:** Defining a safe operating window for the pump (e.g., min 10%, max 98%) to prevent damage or inefficiency.
*   **Compensating for "Dead Zones":** Ensuring the pump always receives a signal strong enough to run reliably.

The customization is done directly in the well-commented source code, keeping this documentation clean and focused.

## Project Status & Roadmap

This project is currently in the early prototyping stage.

![Status: Phase 2: Prototyping](https://img.shields.io/badge/Status-Phase%202%3A%20Prototyping-yellow)

Here is the planned development path:

- [x]   **Phase 1: Problem Definition & Solution Evaluation**
  -   [x] Problem understood and clearly defined.
  -   [x] Solution approach with a programmable microcontroller and galvanic isolation evaluated as ideal.

- [X]   **Phase 2: Arduino Output Test (Current Phase)**
  -   [X] Generation of a stable 500 Hz PWM signal with the Arduino.
  -   [X] Test of direct control of the OEG pump with the generated signal to verify basic compatibility.

- [ ]   **Phase 3: Optocoupler Input Test**
  -   [ ] Setup of the input circuit with the ILD213T optocoupler on an Arduino Uno for testing purposes.
  -   [ ] Verification that the external PWM signal can be read safely and correctly.

- [ ]   **Phase 4: Integration Test**
  -   [ ] Merging of input and output logic into a single sketch.
  -   [ ] Test of the complete "translation process" on the lab bench (test signal -> adapter -> measuring device).

- [ ]   **Phase 5: Prototyping with Arduino Nano & Enclosure**
  -   [ ] Transfer of the working circuit to a more compact perfboard with the Arduino Nano.
  -   [ ] Design and 3D printing of a suitable, functional enclosure.

- [ ]   **Phase 6: Finalization & Field Test**
  -   [ ] Installation of the final prototype in the enclosure.
  -   [ ] Long-term test of the adapter in the real heating system.
  -   [ ] Cleanup of documentation and code.

## Hardware & Circuit

The required hardware (Arduino Nano, ILD213T, etc.) and the circuit are simple and focused on safety and reliability. A detailed schematic can be found in `/docs/schematics/`.

## Adaptability

While this project specifically targets the **Viessmann EM-SM1** and **OEG pumps**, the core concept can be adapted to solve other PWM incompatibility issues by adjusting the mapping logic in the source code.

## ⚠️ Safety Disclaimer ⚠️

This project interfaces with a heating system. While it is designed for maximum safety via galvanic isolation, you are responsible for ensuring all connections are correct. The use of an external, certified USB power supply is a key safety feature of this design. Do not attempt to power this device differently without expert knowledge. Installation and use are at your own risk.

## License

This project is licensed under the [MIT License](./LICENSE).