---
title: pcf8574
component: pcf8574
level: library
reuse: high
platforms:
  - ESP32-KC868-A16
topics:
  - pcf8574
  - i2c
  - gpio-expander
  - relay
source_paths:
  - components/pcf8574/include/pcf8574.h
  - components/pcf8574/pcf8574.c
status: verified
retrieval:
  questions:
    - How do I read or write an 8-bit PCF8574 port?
    - Why does pcf8574_init return ESP_ERR_INVALID_STATE?
    - What I2C addresses does the KC868-A16 use?
  related:
    - ../i2c-manager/README.md
    - ../../project/subsystems/kc868-a16-application/README.md
    - ../../artifacts/patterns/pcf8574_requires_i2c_manager.c
---

# pcf8574

Opaque-handle driver for NXP PCF8574 quasi-bidirectional I/O expanders. **Reuse: high**.

## Purpose

Add devices on the shared I2C bus and exchange full 8-bit port values.

## Reuse classification

High — address and frequency passed by caller; no KC868 hardcoding in the driver.

## Public API

Artifact [A-L02-if](../../artifacts/interfaces/pcf8574.h): `pcf8574_init/deinit/read/write/scan`.

## Dependencies

Requires [i2c-manager](../i2c-manager/README.md) already initialized. Pattern: [A-L02-pat](../../artifacts/patterns/pcf8574_requires_i2c_manager.c).

## Ownership / concurrency

Each handle owns one `i2c_master_dev_handle_t`. Callers serialize access if sharing a handle across tasks.

## Runtime lifecycle

1. `i2c_manager_init`
2. `pcf8574_init` → `i2c_master_bus_add_device`
3. `pcf8574_read` / `pcf8574_write`
4. `pcf8574_deinit` removes device and frees handle

## Configuration

| Field | Meaning |
|-------|---------|
| `address` | 7-bit I2C address |
| `freq_hz` | SCL speed (0 = bus default) |

KC868-A16 addresses (set by P01, not this library): inputs `0x22`/`0x21`, outputs `0x24`/`0x25`.

## Initialization

Fails with `ESP_ERR_INVALID_STATE` if I2C manager not ready; `ESP_ERR_NO_MEM` if malloc fails.

## Error handling

Propagates ESP-IDF I2C errors; 100 ms transaction timeout (`PCF8574_TIMEOUT_MS`).

## Thread safety

No internal locking. Safe if one task owns a handle or external mutex used.

## Memory / resources

`malloc` of `struct pcf8574_handle` per device.

## Limits

8 bits per device; scan helper checks caller-provided expected address list.

## Failure modes

| Symptom | Likely cause |
|---------|--------------|
| Init INVALID_STATE | Forgot `i2c_manager_init` |
| Read/write timeout | Wrong address / missing pull-ups / wiring |

## Data formats

Port byte: bit 0 = P0 … bit 7 = P7. Hardware active-low polarity is applied by the application (P01), not this driver.

## Integration points

Used by `UpdateInputs` / `UpdateOutputs` in P01.

## Logging / diagnostics

Tag `pcf8574`.

## Portability

ESP-IDF I2C master device API.

## Security considerations

Not applicable.

## Related components

- [i2c-manager](../i2c-manager/README.md)
- [kc868-a16-application](../../project/subsystems/kc868-a16-application/README.md)

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Requires i2c_manager | `pcf8574.c` L43–46 | E1 |
| Public API | `pcf8574.h` L32–87 | E1 |
| 100 ms timeout define | `pcf8574.c` L31 | E1 |
