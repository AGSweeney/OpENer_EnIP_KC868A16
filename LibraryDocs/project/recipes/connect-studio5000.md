---
title: Connect from Studio 5000
component: connect-studio5000
level: project
topics:
  - studio-5000
  - eds
  - generic-ethernet-module
status: verified
---

# Connect from Studio 5000

## Using EDS

1. Install `eds/KC868A16.eds` via EDS Hardware Installation Tool.
2. Add module "KC868-A16" (AGSweeney Automation).
3. Ensure firmware includes config assembly 151.
4. See [device-identity-eds.md](../../platform/device-identity-eds.md).

## Using Generic Ethernet Module

| Field | Value |
|-------|-------|
| Input instance / size | 100 / 10 |
| Output instance / size | 150 / 2 |
| Config instance / size | 151 / 0 |

## Related

- [kc868-a16-application](../subsystems/kc868-a16-application/README.md)
- [assembly_layout](../../artifacts/data/assembly_layout.md)
