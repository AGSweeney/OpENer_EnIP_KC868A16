// EXCERPT — source: components/pcf8574/include/pcf8574.h
// EVIDENCE: E1 | symbol: pcf8574_init | lines: 32-87

#pragma once
#include "esp_err.h"

typedef struct pcf8574_handle *pcf8574_handle_t;

typedef struct {
    uint8_t address;   ///< 7-bit I2C address
    uint32_t freq_hz;  ///< 0 = bus default
} pcf8574_config_t;

esp_err_t pcf8574_init(const pcf8574_config_t *config, pcf8574_handle_t *handle);
esp_err_t pcf8574_deinit(pcf8574_handle_t handle);
esp_err_t pcf8574_read(pcf8574_handle_t handle, uint8_t *value);
esp_err_t pcf8574_write(pcf8574_handle_t handle, uint8_t value);
esp_err_t pcf8574_scan(const uint8_t *expected_addresses, size_t num_addresses,
                       uint8_t *found_addresses, size_t *num_found);
