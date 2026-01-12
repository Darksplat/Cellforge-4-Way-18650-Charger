# Firmware Flashing and Recovery Guide
## Arduino Nano ESP32 (ESP32-S3)

This document describes how to build, flash, and recover CellForge firmware on the Arduino Nano ESP32 (ESP32-S3).
It also documents common errors and their resolutions.

---

## Supported Hardware

- Arduino Nano ESP32 (ESP32-S3)
- USB-C connection (data-capable cable required)

---

## Build Environment

Recommended:
- PlatformIO (VS Code)

Supported:
- Arduino IDE (limited debugging support)

---

## Firmware Location

Firmware
- ASCD_Nano_PIO
  - ASCD_Nano_Cellforge
    - src
      - ASCD_Nano.ino
      - Pins.h
      - Secrets.h
      - WifiManager.h
      - StatusLED.h

---

## Configuration (Secrets.h)

Before flashing, ensure `Secrets.h` is configured.

Example:

```cpp
#pragma once

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define CELLFORGE_API_TOKEN "your_api_token_here"
