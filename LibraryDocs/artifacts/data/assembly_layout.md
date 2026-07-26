---
title: KC868-A16 assembly layout
component: kc868-a16-application
level: project
status: verified
topics:
  - assembly
  - CIP
  - I/O
---

# KC868-A16 assembly layout

| Instance | Direction | Size | Contents |
|----------|-----------|------|----------|
| 151 | Config | 0 | Required path start; no payload |
| 150 | O→T (output) | 2 | Y01–Y08, Y09–Y16 relay bits |
| 100 | T→O (input) | 10 | X01–X16 + 4× uint16 ADC LE |

## Input assembly 100

| Offset | Size | Field |
|--------|------|-------|
| 0 | 1 | X01–X08 |
| 1 | 1 | X09–X16 |
| 2 | 2 | A1 / INA1 4–20 mA raw |
| 4 | 2 | A2 / INA2 0–5 V raw |
| 6 | 2 | A3 / INA3 0–5 V raw |
| 8 | 2 | A4 / INA4 4–20 mA raw |

## Active-low hardware

Firmware inverts PCF8574 bytes: PLC bit 1 = relay/input asserted.

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Instance numbers and sizes | `kc868_a16_application.c` L44–56 | E1 |
