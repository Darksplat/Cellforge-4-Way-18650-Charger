# Copilot instructions for ASCD_Nano_PIO

Purpose: give AI agents the minimal, concrete knowledge to be immediately productive working on the ASCD Nano PlatformIO projects.

- **Big picture**: This repository contains a set of PlatformIO projects for the ASCD Nano charger/discharger firmware. Each project follows the PlatformIO layout: `platformio.ini`, `include/`, `lib/`, `src/` and `test/`. The main program for the ASCD Nano lives in `src/ASCD_Nano.ino` and is implemented across multiple `.ino` tabs (e.g. `Buzzer.ino`, `FanController.ino`, `Charge.ino`, `Discharge.ino`, `LCD_UI.ino`, `Button.ino`, `SerialComm.ino`, `Resistance.ino`, `Temperature.ino`, `IOUtils.ino`, `StateMachine.ino`). The main `.ino` file contains forward declarations and the global `CustomSettings` and `Modules` structs.

- **Major components & data flow**:
  - `src/ASCD_Nano.ino`: program entry (setup/loop), pin definitions, `CustomSettings`, `Modules[]` configuration, timers and main loop scheduling.
  - `.ino` tabs (per-feature files): contain implementation of hardware abstraction (shift register, MUX handling), UI (I2C LCD), temperature reading (OneWire + DallasTemperature), charging/discharging state machine, serial communication to ESP8266, and utility helpers. Functions are declared in `ASCD_Nano.ino` and defined in those tabs.
  - Hardware I/O: shift register (74HC595) controls output lines; a 4-to-1 analog multiplexer is used to sample batteries via `readMux(...)`; DS18B20 sensors use OneWire on `ONE_WIRE_BUS`.

- **Key files to inspect or modify**:
  - `src/ASCD_Nano.ino` — primary entry, hardware pin constants (e.g. `latchPin`, `clockPin`, `dataPin`, `S0..S3`, `SIG`, `BTN`, `FAN`, `BUZZ`), `CustomSettings` struct (defaults), `Modules` array (per-slot pin mappings and MOSFET indexes). Update here for global config changes.
  - `src/DebugConfig.h` — controls debugging macros such as `DBG_BEGIN(...)`. Use this when adding/controlling Serial debug output.
  - `src/Temp_Sensor_Serials.h` / temperature-related files — sensor indexing and how DS18B20 serials are used.
  - `.platformio.ini` (project root) — contains build environments and lib deps for PlatformIO. Use `pio` / `platformio` commands.

- **Project-specific conventions & patterns**:
  - Multiple `.ino` files are used like Arduino “tabs”. The project relies on forward declarations in `ASCD_Nano.ino`. Do not change function names or signatures unless you update all declarations/uses across tabs.
  - Types: the code uses `byte` extensively for small integers; preserve these types when editing to avoid subtle API mismatches.
  - Globals: lots of state is kept in global `module[]` array and `settings`. Prefer small, localized changes — updating those structs has global effects.
  - Timing: the loop is driven by non-blocking millis() timers (button poll every 2ms, buzzer 50ms, core cycle 1s, serial every 4s). Keep timers in mind when adding delays; avoid long blocking `delay()` calls in regular operation.

- **Build / flash / debug workflow** (PlatformIO)
  - Build: `pio run` (or `platformio run`).
  - Upload: `pio run -t upload` or `pio run -e <env> -t upload` if multiple environments are defined.
  - Monitor serial: `pio device monitor` (use the baud rate set in `DebugConfig.h` / the `DBG_BEGIN()` calls; the code uses 115200 for main serial and 57600 for the ESP8266 SoftwareSerial link).
  - When proposing code changes that affect wiring or timing, recommend the exact `platformio` command and the device monitor baud rate to the reviewer.

- **External dependencies & integration points**:
  - Libraries: `OneWire`, `DallasTemperature`, `LiquidCrystal_I2C`, `SoftwareSerial`. These are normally declared in `platformio.ini` (`lib_deps`). Ensure edits don't break library usage.
  - ESP8266: `SoftwareSerial ESP8266(3, 2);` is used for a serial link at 57600. Any changes to the serial protocol must be synchronized with the ESP8266 firmware or comms code in `SerialComm.ino`.
  - Hardware: the design uses a shift register (74HC595) and a mux to multiplex battery inputs; the `Modules` array defines per-slot pin patterns — changing them needs hardware verification.

- **Safe modification rules for AI agents** (what you can change and what to avoid):
  - Safe to change: non-global helper functions in feature `.ino` files (UI formatting, comments, small refactors), `CustomSettings` default values for experiments, localized bug fixes that don't change function signatures.
  - Avoid or flag for human review: renaming functions declared in `ASCD_Nano.ino`; changing pin mappings in `Modules[]` without hardware confirmation; changing serial baud rates used for ESP8266 unless coordinated; replacing timing strategy (switching away from millis timers) without tests.

- **Examples / Patterns** (concrete snippets to look for):
  - Forward declaration pattern (in `ASCD_Nano.ino`): `void buzzer();` — corresponding implementation lives in `Buzzer.ino`.
  - Module configuration example (four slots): `Modules module[4] = { {...}, {...}, {...}, {...} };` — update slot-specific behaviour here.
  - Timer loop pattern: `if (currentMillis - buzzerMillis >= 50) { buzzer(); buzzerMillis = currentMillis; }` — preserve this structure when adding periodic work.

- **Where to update documentation & tests**:
  - Add any new hardware notes to `include/README` or top-level README if you create one. PlatformIO projects often lack unit tests; if you add logic-heavy code, prefer adding small host-run unit tests under `test/`.

- **When you need help or approval**:
  - Any change that alters hardware pin mapping, MOSFET control logic, discharge/charge thresholds, or serial protocol with the ESP8266 should be marked with a comment and proposed in a PR for hardware/testing review.

If anything here is unclear or you want more detail on a particular area (build envs in `platformio.ini`, the ESP8266 comms protocol, or the temperature sensor indexing), tell me which topic and I will expand or add examples.
