---
title: kc868-a16-application
component: kc868-a16-application
level: project
platforms:
  - ESP32-KC868-A16
topics:
  - assembly
  - relays
  - adc
  - connection-points
source_paths:
  - components/opener/src/ports/ESP32/kc868_a16_application/kc868_a16_application.c
  - components/opener/src/ports/ESP32/devicedata.h
status: verified
retrieval:
  questions:
    - What assembly instances and sizes does the KC868-A16 expose?
    - When are outputs written to the PCF8574?
    - Why is configuration assembly 151 required?
  related:
    - ../ethernet-bringup/README.md
    - ../../architecture/system-overview.md
    - ../../../libraries/opener-esp32/README.md
---

# kc868-a16-application

Product application callbacks that map CIP assemblies to Kincony KC868-A16 I/O.

## Purpose

Create assemblies 100/150/151, configure Exclusive Owner / Input Only / Listen Only paths, and bridge to PCF8574 + ADC.

## Runtime lifecycle

- `ApplicationInitialization` — I2C, ADC, assemblies, connection points ([A-P01-pat](../../../artifacts/patterns/assembly_init.c)).
- `AfterAssemblyDataReceived` — on output 150: `UpdateOutputs` then `UpdateInputs`.
- `BeforeAssemblyDataSend` — on input 100: refresh inputs/analogs.

## Data layout

Canonical map: [A-P01-data](../../../artifacts/data/assembly_layout.md).

| Instance | Size | Role |
|----------|------|------|
| 151 | 0 | Config (path start) |
| 150 | 2 | Relays Y01–Y16 |
| 100 | 10 | Digital + analog inputs |

## Failure modes

Missing config assembly → scanner errors 16#0315 / 16#0204. Active-low hardware requires firmware invert (implemented in Update*).

## Related components

Depends on L01, L02, L03. Identity in PL03 / `devicedata.h`.

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Assemblies 100/150/151 | `kc868_a16_application.c` L44–49, L299–316 | E1 |
| Input size 10 bytes | L54–56 | E1 |
| Output invert on write | L282–289 | E1 |
