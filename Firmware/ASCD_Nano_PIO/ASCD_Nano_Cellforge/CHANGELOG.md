# Changelog

[0.3.0] – ESP32 Nano hardware refactor & ADC architecture finalised
Added
Finalised migration to Arduino Nano ESP32 (ESP32-S3) as a drop-in controller for the 4-bay charger.
Dual ADS1115 (16-bit I²C ADC) architecture implemented to replace the analogue multiplexer:
ADS1115 @ 0x48 for battery voltages (BV1–BV4)
ADS1115 @ 0x49 for load voltages (RV1–RV4)
All ADS1115 devices powered from 3V3 with local 100 nF decoupling capacitors.
I²C bus standardised to 3.3 V pull-ups (ESP32-safe).
TP5100 charge-status lines (CD1–CD4) converted to direct GPIO inputs with per-module 10 kΩ pull-ups to 3V3.
74HC595 shift register retained for DFET/CFET control, now:
Powered from 3V3
OE tied to GND
MR tied to 3V3
Power strategy finalised:
VIN (6–21 V) fed directly from 12 V barrel jack
USB-C used for programming and testing
Logic rail entirely 3.3 V
ERC completed with only expected warnings (no electrical errors).
Changed
Removed analogue multiplexer (CD74HC4067) and all associated select logic (S0–S3, SIG).
Replaced all analogue reads with ADS1115 channel reads, preserving original state-machine flow.
Pin mapping deliberately aligned to Brett’s original Arduino Nano V3 schematic wherever possible to minimise:
Firmware changes
PCB routing changes
Shift-register control pins kept identical to original design:
DS, SH/CP, ST/CP unchanged at the net level.
Temperature bus (TMP / DS18B20) retained on D4, now explicitly pulled up to 3V3.
Fan control retained on original pin and verified for 3.3 V drive.
All logic-side pull-ups migrated from 5 V → 3.3 V.
+5 V rail removed from logic domain (retained only as isolated probe where applicable).
Fixed
Eliminated mixed-voltage logic issues inherited from the ATmega328P design.
Removed all dependencies on 5 V logic thresholds (HCT parts, 5 V pull-ups).
Resolved ERC issues related to:
Floating ADC address pins
Power-flag ambiguity on VIN
Prevented ESP32 GPIO over-voltage risks on TMP, I²C, and CD lines.
Status
Schematic electrically complete and ESP32-safe
Pin mapping locked and Brett-compatible
Ready for:
Firmware ADS1115 integration
Breadboard / dev-board ADC validation
PCB layout and ordering

[0.2.0] – ESP32 Nano port & PlatformIO stabilisation
Added
Initial migration from Arduino Nano (ATmega328P) to Arduino Nano ESP32 (ESP32-S3).
PlatformIO environment configured for arduino_nano_esp32.
Modular source structure validated (.ino, .h separation).
ADS1115 support introduced (I²C, external ADC).
ESP32-native PWM fan control using ledc.
Debug configuration system (DebugConfig.h) with compile-time enable/disable.
Temperature sensor enumeration updated for 6× DS18B20 sensors.
Compatibility wrapper for legacy readMux() logic.
Changed
Removed AVR-specific assumptions and ESP8266 remnants.
Refactored pin definitions into Pins.h.
Began transition from mux-based ADC reads to ADS1115.
Fixed
Compile-blocking syntax and macro conflicts.
Header include ordering for ESP32 toolchain.
Missing Python dependency (intelhex) in ESP32 build chain.

[0.1.0] – Repository restructure
Initial repository created.
Legacy Arduino Nano firmware archived.
Hardware files, documentation, and reference material organised.
