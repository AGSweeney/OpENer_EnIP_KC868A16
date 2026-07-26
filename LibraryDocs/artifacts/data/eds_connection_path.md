---
title: EDS connection paths
component: device-identity-eds
level: platform
status: verified
topics:
  - EDS
  - connection-path
  - config-assembly
---

# EDS connection paths

From `eds/KC868A16.eds` (must match firmware assemblies).

| Connection | Path | Meaning |
|------------|------|---------|
| Exclusive Owner | `20 04 24 97 2C 96 2C 64` | Class 4 → Inst 151 → CP 150 → CP 100 |
| Input Only | `20 04 24 97 2C 64` | Class 4 → Inst 151 → CP 100 |
| Listen Only | `20 04 24 97 2C 64` | Class 4 → Inst 151 → CP 100 |

| Hex | Decimal | Role |
|-----|---------|------|
| 64 | 100 | Input assembly |
| 96 | 150 | Output assembly |
| 97 | 151 | Config assembly (size 0) |

## Identity (must match `devicedata.h`)

| Field | Value |
|-------|-------|
| VendCode | 55512 |
| ProdType | 7 |
| ProdCode | 1 |
| MajRev / MinRev | 1 / 1 |
| ProdName | KC868-A16 |

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Connection paths include Assem151 | `eds/KC868A16.eds` Connection1–3 Path fields | E1 |
