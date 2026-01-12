# Changelog

All notable changes to this project are documented in this file.

The format broadly follows *Keep a Changelog*.
This project uses pragmatic semantic versioning where appropriate.

---

## [0.4.0] – Live connectivity, telemetry & web platform integration

### Added
- Server-authoritative charger heartbeat (`PING`) protocol
- ESP32 firmware Wi-Fi heartbeat with state reporting
- RSSI (Wi-Fi signal strength) telemetry from firmware
- RSSI storage (`wifi_rssi`) in chargers table
- Traffic-light connection status (CONNECTED / CONNECTING / DISCONNECTED)
- Automatic charger offline detection via cron job
- Charger claiming workflow (unclaimed → user-owned)
- Browser-local time rendering for charger last-seen timestamps
- Signal strength visualisation (bars) in Chargers UI

### Changed
- Connection status is now backend-driven (no browser inference)
- Charger presence determined by server + cron, not firmware assumptions
- Time display moved from server formatting to browser-local rendering

### Fixed
- Timezone offset errors caused by mixed UTC / local comparisons
- Chargers remaining falsely “connected” after power loss
- Missing heartbeat handling in API ingest endpoint

### Status
- End-to-end connectivity verified with real ESP32 hardware
- Heartbeat, RSSI, cron enforcement, and UI fully coherent

---

## [0.3.0] – ESP32 Nano hardware refactor & ADC architecture finalised

### Added
- Finalised migration to **Arduino Nano ESP32 (ESP32-S3)** as drop-in controller
- Dual **ADS1115 (16-bit I²C ADC)** architecture replacing analogue multiplexer:
  - ADS1115 @ `0x48` for battery voltages (BV1–BV4)
  - ADS1115 @ `0x49` for load voltages (RV1–RV4)
- All ADS1115 devices powered from 3.3 V with local 100 nF decoupling
- I²C bus standardised to 3.3 V pull-ups (ESP32-safe)
- TP5100 charge-status lines (CD1–CD4) converted to direct GPIO inputs with
  per-module 10 kΩ pull-ups to 3.3 V
- 74HC595 shift register retained for DFET / CFET control, now:
  - Powered from 3.3 V
  - OE tied to GND
  - MR tied to 3.3 V
- Power strategy finalised:
  - VIN (6–21 V) fed directly from 12 V barrel jack
  - USB-C used for programming and testing
  - Logic rail entirely 3.3 V
- ERC completed with only expected warnings (no electrical errors)

### Changed
- Removed analogue multiplexer (CD74HC4067) and all associated select logic
- Replaced all analogue reads with ADS1115 channel reads while preserving
  original state-machine flow
- Pin mapping deliberately aligned to Brett Watt’s original Arduino Nano V3
  schematic to minimise firmware and PCB changes
- Shift-register control pins (DS, SH/CP, ST/CP) kept identical at net level
- Temperature bus (DS18B20) retained on original pin, explicitly pulled up to 3.3 V
- Fan control retained on original pin and validated for 3.3 V drive
- All logic-side pull-ups migrated from 5 V → 3.3 V
- +5 V rail removed from logic domain (retained only where isolated probes require)

### Fixed
- Eliminated mixed-voltage logic issues inherited from ATmega328P design
- Removed all dependencies on 5 V logic thresholds
- Resolved ERC issues related to:
  - Floating ADC address pins
  - Power-flag ambiguity on VIN
- Prevented ESP32 GPIO over-voltage risks on TMP, I²C, and CD lines

### Status
- Schematic electrically complete and ESP32-safe
- Pin mapping locked and Brett-compatible
- Ready for firmware ADS1115 integration, bench validation, and PCB layout

---

## [0.2.0] – ESP32 Nano port & PlatformIO stabilisation

### Added
- Initial migration from Arduino Nano (ATmega328P) to Arduino Nano ESP32 (ESP32-S3)
- PlatformIO environment configured for `arduino_nano_esp32`
- Modular firmware structure validated (`.ino` / `.h` separation)
- Initial ADS1115 support introduced (external ADC)
- ESP32-native PWM fan control using `ledc`
- Debug configuration system (`DebugConfig.h`) with compile-time enable/disable
- Temperature sensor enumeration updated for 6× DS18B20 sensors
- Legacy compatibility wrapper for original `readMux()` logic

### Changed
- Removed AVR-specific assumptions and ESP8266 remnants
- Refactored pin definitions into `Pins.h`
- Began transition from mux-based ADC reads to ADS1115

### Fixed
- Compile-blocking syntax and macro conflicts
- Header include ordering for ESP32 toolchain
- Missing Python dependency (`intelhex`) in ESP32 build chain

---

## [0.1.0] – Repository restructure & archive recovery

### Added
- Initial repository creation
- Legacy Arduino Nano firmware archived
- Hardware files, documentation, and historical reference material organised
