# CellForge – 4 Way 18650 Charger & Analytics Platform

CellForge is an open hardware and software project for testing, analysing, and cataloguing 18650 lithium-ion cells.

It is inspired by Brett Watt’s original ASCD Nano charger and extends the concept with modern hardware, Wi-Fi connectivity, and a web-based analytics platform.

This repository contains **firmware, hardware, documentation, and web platform components** required to build and operate a connected 4-bay charger.

---

## Project Components

### Hardware
- 4-slot 18650 charger / discharger
- TP5100 charge controllers per slot
- Dual ADS1115 (16-bit I²C ADCs)
  - Battery voltage sensing
  - Load / discharge voltage sensing
- DS18B20 temperature sensors
- Active cooling with PWM fan control
- 3.3 V logic throughout (ESP32-safe)

### Firmware
- Target: **Arduino Nano ESP32 (ESP32-S3)**
- Wi-Fi connectivity
- Server heartbeat (PING) protocol
- RSSI (signal strength) reporting
- Internal resistance measurement
- Capacity and energy calculation
- Modular PlatformIO-based build

### Web Platform
- PHP + MySQL backend
- User authentication
- Charger registration and claiming
- Live charger connectivity status
- Signal strength visualisation
- Cell inventory
- Run ingestion (start, IR, complete, error)
- Cron-based offline detection

### Data
- Cell metadata (barcode-centric)
- Internal resistance results
- Capacity (mAh) and energy (Wh)
- Charger telemetry (presence, RSSI)

---

## Current Status

- Firmware: **Stable, Wi-Fi enabled**
- Connectivity: **Server-authoritative heartbeat**
- Telemetry: **RSSI reporting live**
- Offline detection: **Cron-enforced**
- UI: **Live status, signal bars, local time rendering**

---

## Repository Structure (high-level)


---

## Design Principles

- Server decides truth (presence, offline state)
- Firmware reports facts, not assumptions
- Browser renders user-local time
- Hardware remains Brett-compatible where practical
- Electrical safety first (3.3 V logic, no mixed-voltage traps)

---

## Licensing

Original ASCD Nano work:
- © Brett Watt
- Creative Commons BY-NC-SA 3.0

CellForge modifications:
- © Jeremy Younger
- Released under the same license unless otherwise stated

---

## Disclaimer

This project involves lithium-ion cells.
Improper use can result in fire, injury, or property damage.

You are responsible for ensuring safe construction, operation, and supervision.
