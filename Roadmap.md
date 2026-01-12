# CellForge Roadmap

This document outlines the current implementation status and planned development
for the CellForge project. It is structured by discipline to avoid duplication of
work and to clearly communicate responsibility boundaries.

CellForge is the system of record. Legacy Battery Portal functionality is treated
as a UX and feature reference only.

---

## Hardware

### Implemented
- 4-slot cell charger/discharger hardware design
- Support for Arduino Nano, ESP32, and other hardware variants
- Slot-based cell handling
- Temperature sensing support
- Hardware abstraction sufficient for multiple charger units

### Planned
- Hardware revision documentation (schematics and BOM cleanup)
- Clarification of supported hardware variants
- Optional enclosure and mechanical documentation
- Hardware self-test and diagnostics support (future)

---

## Firmware

### Implemented
- Event-driven firmware model
- Secure charger authentication using API tokens
- Firmware version reporting to backend
- Run lifecycle events:
  - RUN_START
  - IR_RESULT
  - RUN_COMPLETE
  - RUN_ERROR
- Internal resistance measurement reporting
- Voltage, current, temperature telemetry
- Capacity and energy reporting
- Slot label support

### Planned
- Firmware configuration documentation
- Improved error classification and reporting
- Optional firmware self-diagnostics
- Firmware update/version rollout visibility
- Extended telemetry support (where hardware allows)

---

## Website Development

### Implemented
#### Core Functionality
- User authentication and account management
- Secure password handling
- Per-user data isolation
- Charger management and association
- Cell (battery) management
- Run lifecycle tracking
- Internal resistance data capture
- Capacity and discharge result storage

#### Data Model
- Relational schema for users, chargers, cells, and runs
- Authenticated ingestion API
- Firmware-compatible event handling
- Scalable backend architecture

### UI and UX Work Pending
- Cell list view aligned with legacy Battery Portal workflows
- Cell detail pages with:
  - Run history
  - Internal resistance results
  - Capacity and energy results
- Charger overview dashboard
- Slot activity and utilisation views
- Run history tables

### Visualisation
- Internal resistance trend charts
- Capacity versus cycle charts
- Run timelines per cell
- Charger slot utilisation graphs

### Planned Features
- Second Life Storage scraping and import
  - Manufacturer, model, chemistry, capacity
  - Cell image import
  - One-time import with manual override
- Cell fault marking UI and indicators
- Data export (CSV, JSON)
- Administrative charger health views

---

## Documentation

### Implemented
- Basic project structure
- Existing inline code documentation
- Firmware and backend comments

### Planned
- Hardware documentation (schematics, BOM, wiring)
- Firmware setup and configuration guide
- API and event model documentation
- Database schema documentation
- Contributor onboarding documentation
- User guide for the CellForge website
- Migration notes from legacy Battery Portal concepts

---

## Out of Scope (Current)

The following items are explicitly out of scope for the current roadmap:

- Reverting to legacy backend schemas
- Maintaining multiple parallel ingestion APIs
- Unauthenticated charger ingestion
- Automatic synchronisation with external APIs
- Cloud-only or SaaS-specific assumptions
- Advanced battery chemistry modelling beyond captured metrics
