// EXCERPT — source: components/i2c_manager/include/i2c_manager.h
// EVIDENCE: E1 | symbol: i2c_manager_init | lines: 40-62

#pragma once
#include "esp_err.h"
#include "driver/i2c_master.h"

esp_err_t i2c_manager_init(int sda_gpio, int scl_gpio, uint32_t freq_hz);
esp_err_t i2c_manager_deinit(void);
esp_err_t i2c_manager_get_bus(i2c_master_bus_handle_t *bus_handle);
bool i2c_manager_is_initialized(void);
esp_err_t i2c_manager_get_freq(uint32_t *freq_hz);
