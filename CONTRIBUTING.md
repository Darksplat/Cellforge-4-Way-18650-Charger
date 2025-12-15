# Contributing to CellForge

Thank you for your interest in contributing to the CellForge 4 Way 18650 Charger project.

This repository contains firmware, hardware design files, documentation, and historical reference material. Please read the guidelines below before contributing.

---

## Code of conduct
Be respectful and constructive. This project is open to contributors of all experience levels, but unsafe or misleading technical advice will not be accepted.

---

## What you can contribute
We welcome contributions in the following areas:

- Firmware improvements and refactors
- Bug fixes and safety improvements
- Documentation and build guides
- Hardware design revisions (schematics, PCB, enclosure)
- Test results and validation data

---

## Repository rules (important)

### 1. Archive is read-only
The `archive/` directory contains historical and reference material.

- Do **not** modify files in `archive/`
- Do **not** refactor or rename archived content
- Add notes or clarifications in documentation instead

---

### 2. Firmware contributions
- Keep Arduino Nano code compatible with the Arduino IDE
- Maintain clear separation between legacy and refactored code
- Comment safety-critical logic clearly
- Avoid introducing breaking changes without discussion

---

### 3. Hardware contributions
- Do not overwrite existing revisions
- Create a new revision folder for each hardware change
- Include:
  - Schematic
  - PCB files
  - BOM
  - Revision notes

---

### 4. Documentation
- Use clear, practical language
- Include photos or diagrams where helpful
- Keep safety information explicit and prominent

---

## Pull requests
When submitting a pull request:
- Clearly describe what changed and why
- Reference related issues if applicable
- Include screenshots, photos, or test data where relevant
- Keep changes focused and reviewable

---

## Issues
When reporting an issue, please include:
- Board revision
- Firmware version
- Power supply used
- Cell type and condition
- Steps to reproduce the issue

Incomplete issues may be closed without action.

---

## Licensing
By contributing, you agree that your contributions will be licensed under the same license as the project unless otherwise stated.

---

## Safety disclaimer
This project involves lithium-ion batteries and high current circuits. Contributors must not provide advice or changes that encourage unsafe operation.

---

Thank you for helping improve CellForge.
