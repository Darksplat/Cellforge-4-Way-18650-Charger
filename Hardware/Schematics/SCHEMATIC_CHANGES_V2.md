
Arduino 4× 18650 Smart Charger
V2 Schematic Change Summary (Brett-Compatible)
This document summarises the intentional schematic changes made in V2 relative to Brett Watt’s original Arduino Nano V3 (ATmega328P) design.
The goal of V2 is to modernise the controller and measurement architecture without changing the core charge/discharge behaviour or control topology.
1. Microcontroller platform change
Original (V1)
Arduino Nano V3 (ATmega328P)
5 V logic domain
Limited ADC resolution and channel count
V2
Arduino Nano ESP32 (ESP32-S3)
3.3 V logic domain
VIN range retained (6–21 V) for direct 12 V barrel-jack supply
USB-C used for programming and development
Rationale
Modern MCU with Wi-Fi/BLE capability
Significantly higher processing headroom
Native USB-C
Long-term availability
The ESP32 Nano is treated as a drop-in controller, not a functional redesign.
2. Analogue measurement architecture (major change)
Original (V1)
CD74HC4067 analogue multiplexer
Shared SIG line into ATmega ADC
Select lines S0–S3
Sequential sampling of:
Battery voltages (BVx)
Load voltages (RVx)
TP5100 status via mux
V2
Analogue multiplexer completely removed
Replaced with two ADS1115 16-bit I²C ADCs
ADS1115	Address	Function
U14	0x48 (ADDR = GND)	Battery voltages BV1–BV4
U15	0x49 (ADDR = 3V3)	Load voltages RV1–RV4
All ADC inputs are single-ended
I²C bus pulled up to 3.3 V
Local 100 nF decoupling per ADC
Rationale
Removes mux timing complexity
Higher resolution and repeatability
Parallel access to all channels
Cleaner firmware model (no SIG or select logic)
Firmware preserves Brett’s state machine but replaces analogRead(SIG) with ADS1115 channel reads.
3. TP5100 charge status sensing
Original (V1)
CD1–CD4 routed through the analogue mux
Read indirectly via SIG
V2
CD1–CD4 are direct GPIO inputs
Each CD line:
Has its own 10 kΩ pull-up to 3.3 V
Connects directly to the ESP32
Charger	ESP32 pin (Brett-aligned)
CD1	D12
CD2	D11
CD3	D10
CD4	D9
Rationale
TP5100 outputs are digital status signals
ADC/mux path was unnecessary
Improves reliability and simplifies logic
4. Shift register control (unchanged in concept)
Original (V1)
74HC595 powered from 5 V
Driven by ATmega GPIO
Controls DFET/CFET lines
V2
74HC595 retained
Now powered from 3.3 V
Control signals unchanged at the net level:
DS
SH/CP
ST/CP
OE tied to GND
MR tied to 3.3 V
Rationale
Maintains identical charge/discharge control behaviour
3.3 V logic is fully sufficient for downstream transistor stages
Eliminates mixed-voltage logic risks
5. Pin mapping philosophy
A key design goal in V2 was to keep pin names and usage identical to Brett’s schematic wherever possible.
Kept identical
DS, SH/CP, ST/CP
TMP (DS18B20 bus) on D4
FAN on D5
BTN, BUZZ
I²C on A4/A5
VIN on VIN
Reused pins
Former mux select pins (S0–S3) are now used for CD1–CD4
No new GPIOs introduced unless strictly required
Result
Firmware diffs are small and readable
PCB routing changes are minimal
Original mental model remains valid
6. Temperature sensing
Original (V1)
DS18B20 sensors
5 V pull-up on OneWire bus
V2
DS18B20 sensors retained
Pull-up moved to 3.3 V
ESP32-safe logic levels
7. Fan control
Fan driver topology unchanged
Control pin unchanged
Verified to operate correctly from 3.3 V drive
8. Power domain changes
Logic power
Entire logic domain is now 3.3 V
No 5 V logic remains
VIN
12 V barrel jack feeds VIN directly
ESP32 onboard regulator generates 3.3 V
USB-C may be connected simultaneously for programming
Important
VBUS (USB 5 V) is not used as a system rail
9. Removed elements
The following are intentionally removed in V2:
CD74HC4067 analogue multiplexer
SIG analogue bus
S0–S3 select logic
5 V pull-ups on logic signals
Any 74HCT logic
10. What has NOT changed
TP5100 charge/discharge topology
Load resistor arrangement
DFET/CFET transistor stages
Overall charger behaviour and sequencing logic
Mechanical layout concept (4 independent bays)
Summary
V2 is a modernised controller and sensing architecture built on top of Brett’s proven charger design.
Control logic philosophy is unchanged
Measurement is cleaner, higher resolution, and simpler
Logic voltage is unified at 3.3 V
ESP32 provides a future-proof platform
If you understand Brett’s original schematic, V2 should feel immediately familiar — just without the mux and with much better ADCs.
