# SWOT-Bot  
**Smartphone–Web-based Outdoor Teleoperation Robot**

👉 **[Web BLE Robot Controller (Live Demo)](https://menginventor.github.io/SWOT-BOT/)**

---

## Overview

**SWOT-Bot** (Smartphone–Web-based Outdoor Teleoperation) is a research and educational robotics project that explores **web-native robot control using standard web browsers and smartphones**.

This repository contains **Phase-1** of the SWOT-Bot project, which focuses on building a **mobile robot platform controllable directly from a web page**, without requiring native mobile applications or heavy software frameworks.

Unlike traditional robot control apps, **SWOT-Bot uses web technologies as the primary interface**, allowing users to design, modify, and deploy their own control UI easily and accessibly.

---

## Motivation

Building robot systems using Single-Board Computers (SBCs) often introduces:
- High cost
- Complex peripheral integration
- Power and reliability challenges
- Steep learning curves for beginners

**SWOT-Bot proposes an alternative approach**:
> Use a **smartphone as the robot’s brain** and the **web browser as the control interface**.

This significantly lowers the barrier for:
- Students
- Researchers
- DIY robotics enthusiasts

---

## Phase-1: Proof of Concept

Phase-1 demonstrates **manual robot teleoperation via a static web page**.

### Key Characteristics
- Manual robot control using a **web-based UI**
- Works on **smartphones, tablets, and PCs**
- No native mobile app required
- Static web deployment (e.g., GitHub Pages)

---

## Core Technologies

### Web Bluetooth (Phase-1)
- Uses **Web Bluetooth API**
- Browser communicates directly with the robot via **Bluetooth Low Energy (BLE)**
- **Classic Bluetooth is not used**
- ESP32 operates in BLE mode and receives control commands from the web UI

### Hardware
- **ESP32** as motor controller and BLE device
- Smartphone acts as:
  - User interface
  - Sensor hub
  - Future vision system

---

## Customization & Deployment

In Phase-1:
- Users can **customize the control UI**
- Fork this repository
- Modify HTML / CSS / JavaScript
- Deploy the web interface as a **static website** (e.g., GitHub Pages)
- Instantly control the robot from a smartphone browser

This design emphasizes **accessibility, flexibility, and rapid experimentation**.

---

## Planned Phase-2: Outdoor Teleoperation

Phase-2 will extend the framework to **true outdoor teleoperation** using **WebRTC**.

### Planned Features
- **Peer-to-peer video streaming**
  - Smartphone mounted on robot → mission control browser
- **Bidirectional control and telemetry**
- Works over:
  - 4G / 5G cellular networks
- No centralized media server required

---

## Browser-Based Intelligence

Modern web browsers can also handle:
- Image processing
- Computer vision (e.g., MediaPipe)
- Sensor fusion using:
  - IMU
  - GPS
  - Compass

This enables advanced robotic capabilities **without dedicated onboard computers**.

---

## Research & Educational Value

SWOT-Bot is not designed to replace specialized industrial or military platforms.  
Instead, it aims to:
- Democratize robotic experimentation
- Enable rapid prototyping
- Support education and research in:
  - Web robotics
  - Human–robot interaction
  - Decentralized control
  - Low-cost field robotics

---

## Project Status

- ✅ Phase-1: Web Bluetooth manual teleoperation  
- 🚧 Phase-2: WebRTC-based outdoor teleoperation (in progress)

---

## License
MIT License

---

## Acknowledgment

This project is developed as part of ongoing research and experimentation in **accessible, web-based robotic systems**.
