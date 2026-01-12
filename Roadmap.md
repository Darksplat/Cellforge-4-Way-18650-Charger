CellForge — Features & Roadmap
✅ Implemented Features (Current State)
These features are already implemented and working in CellForge.
🔋 Cell (Battery) Management
Create and manage individual cells
Barcode-based cell identification
Manufacturer, model, chemistry, and nominal capacity storage
Cell notes and metadata
Cell image support (photo URL)
Cell ownership per user
⚡ Charger Management
Multi-charger support per user
Hardware type tracking (Nano, ESP32, Mega, etc.)
Secure charger authentication via API token
Firmware version tracking
Charger slot identification via slot labels
Last check-in tracking
🔄 Run Lifecycle Management
Run start (RUN_START)
Run completion (RUN_COMPLETE)
Run error handling (RUN_ERROR)
Slot-aware run tracking
Status tracking (RUNNING, COMPLETE, ERROR)
Start and end timestamps
📏 Internal Resistance (IR) Testing
Internal resistance measurement (mΩ)
Test current tracking
Open-circuit voltage capture
Load voltage capture
Temperature capture at test time
Timestamped IR results per run
📊 Capacity & Discharge Results
Measured capacity (mAh)
Energy delivered (Wh)
Discharge current tracking
Cutoff voltage tracking
🧠 Data Model & Architecture
Modern relational schema (users, chargers, cells, runs)
Single-source-of-truth backend
Authenticated ingestion API
Firmware-compatible event model
Designed to scale to multiple chargers and users
🔐 Authentication
User accounts
Secure password hashing
Per-user data isolation
Charger-to-user association
🟡 Partially Implemented / UI Pending
These features are backend-ready, but need UI or presentation work.
🖥️ User Interface & UX
Cell list view (needs Brett-style layout parity)
Cell detail page (history, IR, capacity views)
Charger overview dashboard
Slot activity visualisation
Run history tables
Fault / error visual indicators
🚩 Cell Fault Handling
Backend supports error states via runs
UI flag for “cell marked faulty” still required
Visual indication in cell lists and details
📈 Visualisation
IR trend charts
Capacity vs cycle charts
Run timelines per cell
Charger slot utilisation graphs
🧩 Planned Features (Not Yet Implemented)
These are intentional future additions, not gaps.
🌐 Second Life Storage Integration
Scrape public Second Life Storage cell database
Autofill:
Manufacturer
Model
Chemistry
Nominal capacity
Import cell image
One-time import per cell (no auto-sync)
Manual override after import
🎨 UI & Branding
CellForge colour scheme applied site-wide
Visual alignment with Brett-era Battery Portal layout
Mobile-responsive UI improvements
Clear visual hierarchy for runs, IR, and results
🧪 Advanced Analytics (Future)
Cell health scoring
IR degradation tracking over cycles
Capacity fade tracking
Pass/fail thresholds
Sorting and filtering by health metrics
🧰 Admin & Quality-of-Life
Charger health/status page
Firmware rollout visibility
Export run data (CSV/JSON)
Audit / debug views for ingestion events
🧭 Explicitly Out of Scope (For Now)
Documenting this avoids future confusion.
Reverting to legacy (Brett-era) backend schema
Unauthenticated ingestion endpoints
Automatic syncing with external APIs
Cloud-only / SaaS hosting assumptions
Battery chemistry modelling beyond captured metrics
