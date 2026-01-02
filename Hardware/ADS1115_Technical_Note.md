ADS1115 Technical Note
Rationale, Configuration, and Integration in V2
This note explains why the ADS1115 was selected, how it is configured in the V2 schematic, and how it replaces the analogue multiplexer used in Brett’s original Arduino Nano design.
1. Why ADS1115 was chosen
The ADS1115 was selected to address the key limitations of the original analogue sensing approach:
Limitations in V1 (mux + MCU ADC)
10-bit ADC resolution (ATmega328P)
Shared SIG line susceptible to:
switching transients
settling-time errors
Sequential sampling via S0–S3
Firmware complexity around mux timing
Advantages of ADS1115
16-bit resolution (up to 32768 counts)
True differential-capable architecture (used single-ended here)
Per-channel input sampling without analogue switching
Stable, repeatable readings
Operates natively at 3.3 V
Widely supported, proven, inexpensive
The ADS1115 provides cleaner data with simpler firmware, without changing the charger’s electrical behaviour.
2. ADS1115 role in the V2 design
Two ADS1115 devices are used:
Device	I²C Address	Purpose
ADS1115 #1	0x48 (ADDR = GND)	Battery voltages BV1–BV4
ADS1115 #2	0x49 (ADDR = 3V3)	Load voltages RV1–RV4
Each ADS1115 uses:
Single-ended inputs
One channel per measurement
Shared I²C bus (SDA/SCL)
This mirrors the logical separation Brett already used (battery vs load sensing), just without the mux.
3. Electrical configuration (schematic-level)
Power
VDD = 3.3 V
GND = system ground
Local 100 nF decoupling capacitor per ADS1115
I²C
SDA / SCL pulled up to 3.3 V
No level shifting required
ALERT/RDY pin unused (left unconnected)
Address selection
ADDR tied to GND → address 0x48
ADDR tied to 3V3 → address 0x49
This avoids bus contention and allows both devices to coexist cleanly.
4. Firmware implications (high level)
What is removed
No S0–S3 control
No SIG analogue reads
No mux settling delays
What replaces it
Each voltage is read directly:
ads.readADC_SingleEnded(channel);
Where channel is 0–3 corresponding to BVx or RVx.
Scaling
The existing resistor divider ratios remain unchanged
Only the ADC conversion changes
Brett’s original scaling math is reused
This keeps calibration and interpretation consistent with the original design.
5. Performance and reliability benefits
Practical improvements
Less noise on voltage readings
No cross-channel coupling
Better low-current / near-end-of-charge resolution
More consistent logging results
System-level benefits
Simplified schematic
Fewer failure points
Easier debugging
Cleaner PCB routing
6. What ADS1115 does not change
To be explicit, ADS1115 does not:
Alter TP5100 charge behaviour
Change DFET/CFET control logic
Affect timing of charge/discharge cycles
Replace any safety or control logic
It is a measurement upgrade only.
7. Summary
The ADS1115 replaces the analogue multiplexer and MCU ADC with a dedicated, high-resolution measurement subsystem that:
Preserves Brett’s original design intent
Improves accuracy and reliability
Simplifies both schematic and firmware
Aligns naturally with a 3.3 V ESP32 platform
It is a modernisation, not a redesign.
