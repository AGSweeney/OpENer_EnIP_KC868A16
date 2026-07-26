---
title: i2c-manager
component: i2c-manager
level: library
reuse: high
platforms:
  - ESP32-KC868-A16
topics:
  - i2c
  - bus
  - gpio
  - esp-idf
source_paths:
  - components/i2c_manager/include/i2c_manager.h
  - components/i2c_manager/i2c_manager.c
status: verified
retrieval:
  questions:
    - How do I initialize the I2C bus?
    - Which GPIO pins does the KC868-A16 use for I2C?
    - What happens if pcf8574_init runs before i2c_manager_init?
  related:
    - ../pcf8574/README.md
    - ../../project/subsystems/kc868-a16-application/README.md
---

# i2c-manager

Singleton ESP-IDF I2C master bus wrapper used by PCF8574 expanders. **Reuse: high** for any ESP32 board needing one shared bus.

## Purpose

Own `I2C_NUM_0` creation, teardown, and handle publication for device drivers.

## Reuse classification

High — no board-specific pin defaults inside the library; caller passes GPIO/frequency.

## Public API

See artifact [A-L01-if](../../artifacts/interfaces/i2c_manager.h): `i2c_manager_init`, `deinit`, `get_bus`, `is_initialized`, `get_freq`.

## Dependencies

- ESP-IDF `driver/i2c_master.h`
- FreeRTOS (indirect via ESP-IDF)

## Ownership / concurrency

Single global bus (`s_bus_handle`). Not re-entrant for concurrent init; intended one-time setup from application init path.

## Runtime lifecycle

1. `i2c_manager_init(sda, scl, freq_hz)` creates master bus with internal pull-ups.
2. Device drivers call `i2c_manager_get_bus`.
3. Optional `i2c_manager_deinit` deletes the bus.

## Configuration

| Parameter | KC868-A16 value | Set by |
|-----------|-----------------|--------|
| SDA | GPIO 4 | P01 application |
| SCL | GPIO 5 | P01 application |
| Frequency | 400000 Hz | P01 application |

## Initialization

Idempotent: second `init` logs warning and returns `ESP_OK` if already initialized.

## Error handling

Returns `esp_err_t`. Failures from `i2c_new_master_bus` are logged and returned.

## Thread safety

Not documented as multi-thread safe for init/deinit. Device TX/RX uses ESP-IDF I2C master APIs.

## Memory / resources

One `i2c_master_bus_handle_t`; no dynamic buffers in the manager itself.

## Limits

One default bus (`I2C_NUM_0` only).

## Failure modes

| Symptom | Likely cause |
|---------|--------------|
| `ESP_ERR_INVALID_STATE` from pcf8574 | Manager not initialized |
| Bus create fails | Pin conflict / wrong port |

## Data formats

Not applicable — bus handle only.

## Integration points

Required before [pcf8574](../pcf8574/README.md). Called from `InitializeI2C()` in P01.

## Logging / diagnostics

Tag `i2c_manager`; logs GPIO and frequency on success.

## Portability

ESP-IDF I2C master API (IDF 5.x). Not portable to bare-metal without driver rewrite.

## Security considerations

Not applicable (local bus, no network exposure).

## Related components

- [pcf8574](../pcf8574/README.md)
- [kc868-a16-application](../../project/subsystems/kc868-a16-application/README.md)

## Source evidence

| Claim | Evidence | Level |
|-------|----------|-------|
| Init API signature | `components/i2c_manager/include/i2c_manager.h` L40–62 | E1 |
| Uses I2C_NUM_0 + internal pull-ups | `i2c_manager.c` L39–46 | E1 |
| Idempotent re-init returns ESP_OK | `i2c_manager.c` L34–37 | E1 |
