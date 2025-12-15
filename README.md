# CellForge 4 Way 18650 Charger

Open-source 4-slot 18650 battery charger, discharger, and analyser with capacity, energy, and internal-resistance measurement.

## What this project is
CellForge is a hardware + firmware project for testing and grading individual 18650 lithium-ion cells. The goal is a practical, repeatable workflow for:
- Charging and discharging cells in controlled modes
- Measuring **capacity (mAh)** and **energy (Wh)**
- Recording **internal resistance (IR)** as a measured value
- Producing consistent results suitable for sorting and reuse (“second life” cells)

## Safety notice (read first)
Lithium-ion cells can be dangerous if mishandled. This project involves:
- High current paths
- Heat generation
- Risk of short-circuit, fire, or cell venting

You are responsible for building and using this project safely. Use appropriate fusing, insulation, airflow, temperature monitoring, and never leave cells unattended while testing.

## Repository structure (high level)
This repository will contain:
- **Firmware** (Arduino Nano baseline and future ESP32 variants)
- **Hardware** (schematics, PCB, gerbers, BOM)
- **Documentation** (build guides, calibration, operating modes)
- **Tools** (utilities, scripts, data formats)
- **Archive** (historical reference material)

## Current status
- In active development and consolidation.
- Legacy code and reference material are being organised into a stable structure.
- ESP32 migration is planned, with the Arduino Nano baseline retained for existing builds.

## Getting started
Until the full docs are published, the recommended starting point is:
1. Review the safety notes above.
2. Browse `firmware/` for the baseline build.
3. Browse `hardware/` for schematics/PCB and verify your board revision.
4. Use `docs/` for calibration and operating instructions as they are added.

## Contributing
Contributions are welcome:
- Bug fixes and refactors
- Documentation improvements
- Hardware revision notes
- Test data and repeatability validation

See `CONTRIBUTING.md` (to be added) once the repo structure is finalised.

## License
This project is licensed under the MIT License unless otherwise noted in specific subfolders (for example, third-party archive material).
See `LICENSE` for details.

## Credits and references
This project builds on prior community work and legacy material preserved for reference in the `archive/` folder. Where applicable, original attribution is retained alongside archived content.
