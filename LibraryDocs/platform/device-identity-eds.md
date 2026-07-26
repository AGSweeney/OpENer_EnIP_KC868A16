---
title: Device identity and EDS
component: device-identity-eds
level: platform
platforms:
  - ESP32-KC868-A16
topics:
  - eds
  - identity
  - vendor-id
  - connection-path
source_paths:
  - components/opener/src/ports/ESP32/devicedata.h
  - eds/KC868A16.eds
status: verified
---

# Device identity and EDS

## Identity (firmware)

From `devicedata.h`:

| Attribute | Value |
|-----------|-------|
| Vendor ID | 55512 |
| Device Type | 7 |
| Product Code | 1 |
| Major / Minor | 1 / 1 |
| Product Name | KC868-A16 |

EDS `[Device]` section must match these fields for electronic keying.

## Assemblies / paths

Canonical path notes: [A-PL03-data](../artifacts/data/eds_connection_path.md).

Config assembly **151** (size 0) must exist in firmware and appear first in EDS connection paths.

## Generic Ethernet Module (no EDS)

| Parameter | Value |
|-----------|-------|
| Input | Inst 100, 10 bytes |
| Output | Inst 150, 2 bytes |
| Config | Inst 151, 0 bytes |

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Identity macros | `devicedata.h` L4–9 | E1 |
| EDS VendCode 55512 | `eds/KC868A16.eds` L13 | E1 |
| Exclusive Owner path includes 151 | `eds/KC868A16.eds` Connection1 path | E1 |
