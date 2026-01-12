# CellForge Roadmap

This roadmap describes the intended development direction of the CellForge project.
It is not a promise of timelines; items may move or change as the project evolves.

The focus is on reliability, safety, and real-world usability.

---

## Firmware

### Completed
- ESP32 Nano (ESP32-S3) platform migration
- Wi-Fi connectivity
- Server heartbeat (PING) protocol
- Connection state reporting (CONNECTED / CONNECTING)
- RSSI (Wi-Fi signal strength) reporting
- ADS1115 external ADC integration
- PWM fan control using ESP32 `ledc`
- Multi-sensor temperature support
- Legacy compatibility for original state machine logic

### Planned
- Firmware build/version reporting to server
- Charger-side error codes surfaced in UI
- Optional heartbeat interval configuration
- OTA update strategy (future, not immediate)

---

## Hardware

### Completed
- ESP32-safe 3.3 V logic domain
- Dual ADS1115 voltage measurement architecture
- Removal of analogue multiplexer
- TP5100 charge status GPIO integration
- Shift-register-based MOSFET control
- Electrical rule check (ERC) validation

### Planned
- PCB V2 layout validation
- Thermal optimisation review
- Enclosure finalisation
- Connector and strain relief improvements

---

## Website / Backend

### Completed
- User authentication (register / login / logout)
- Charger registration via API token
- Charger claiming workflow
- Live connection status indicators
- RSSI signal strength visualisation
- Local-time rendering for timestamps
- Cron-based offline detection
- Runs ingest pipeline (start, IR, complete, error)
- Cell inventory management

### Planned
- Per-charger detail pages
- Charger telemetry history (RSSI, uptime)
- Run history visualisation (charts)
- Per-cell performance trends
- Alerts (offline, weak signal, failed runs)
- Data export (CSV / JSON)

---

## Documentation

### Completed
- Changelog
- Project README
- Roadmap

### Planned
- Firmware flashing and recovery guide
- Hardware build and wiring guide
- API reference documentation
- Safety and operating guidelines

---

## Data & Integrations

### Completed
- Core schema for users, chargers, cells, and runs

### Planned
- Second Life Storage data scraping (cell metadata, images)
- Cell model normalisation
- Historical trend analysis

---

## Guiding Principles

- Server determines truth for connectivity and state
- Firmware reports measurements and facts only
- UI reflects backend state, not inference
- Safety and electrical correctness over speed
- Backward compatibility with Brett Watt’s design where practical
