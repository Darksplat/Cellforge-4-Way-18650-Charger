# CellForge Roadmap

This document outlines the current implementation status and planned development
for the CellForge project. It is intended to prevent duplication of work and to
provide clear direction for future development.

---

## Implemented Features

### Cell Management
- Create and manage individual cells
- Barcode-based cell identification
- Manufacturer, model, chemistry, and nominal capacity storage
- Cell notes and metadata
- Cell image support (photo URL)
- Per-user cell ownership

### Charger Management
- Multi-charger support per user
- Hardware type tracking (Nano, ESP32, Mega, etc.)
- Secure charger authentication via API token
- Firmware version tracking
- Charger slot identification via slot labels
- Last check-in tracking

### Run Lifecycle
- Run start handling (RUN_START)
- Run completion handling (RUN_COMPLETE)
- Run error handling (RUN_ERROR)
- Slot-aware run tracking
- Run status tracking (RUNNING, COMPLETE, ERROR)
- Start and end timestamps

### Internal Resistance Testing
- Internal resistance measurement (milliohms)
- Test current tracking
- Open-circuit voltage capture
- Load voltage capture
- Temperature capture at test time
- Timestamped IR results per run

### Capacity and Discharge Results
- Measured capacity (mAh)
- Energy delivered (Wh)
- Discharge current tracking
- Cutoff voltage tracking

### Authentication and Architecture
- User authentication and account management
- Secure password hashing
- Per-user data isolation
- Authenticated ingestion API
- Firmware-compatible event model
- Relational database schema for users, chargers, cells, and runs

---

## UI and Presentation Work Pending

### User Interface Parity
- Cell list view aligned with legacy Battery Portal workflows
- Cell detail pages with history, IR, and capacity views
- Charger overview dashboard
- Slot activity and utilisation views
- Run history tables

### Visualisation
- Internal resistance trend charts
- Capacity versus cycle charts
- Run timelines per cell
- Charger slot utilisation graphs

### Cell Fault Indicators
- User interface control to mark cells as faulty
- Visual indicators for faulty cells in lists and detail views

---

## Planned Features

### Second Life Storage Integration
- Scrape public Second Life Storage cell database
- Autofill manufacturer, model, chemistry, and nominal capacity
- Import and associate cell images
- One-time import per cell with no automatic updates
- Manual override of imported data

### UI and Branding
- Apply CellForge colour scheme site-wide
- Align layout and navigation with Brett-era Battery Portal
- Improve mobile responsiveness
- Improve visual hierarchy for runs, IR results, and discharge data

### Analytics and Health Metrics
- Cell health scoring
- Internal resistance degradation tracking
- Capacity fade tracking over multiple cycles
- Pass/fail thresholds for cells
- Sorting and filtering by health metrics

### Administration and Data Export
- Charger health and status overview
- Firmware rollout visibility
- Export run and cell data (CSV and JSON)
- Ingestion and event audit views

---

## Out of Scope (Current)

The following items are explicitly out of scope for the current roadmap:

- Reverting to legacy backend schemas
- Unauthenticated ingestion endpoints
- Automatic synchronisation with external APIs
- Cloud-only or SaaS-specific assumptions
- Advanced battery chemistry modelling beyond captured metrics
