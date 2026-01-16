/*
 * Copyright (c) 2025, Adam G. Sweeney <agsweeney@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pcf8574_handle *pcf8574_handle_t;

/**
 * @brief Configuration structure for PCF8574
 */
typedef struct {
    uint8_t address;           ///< I2C address (7-bit)
    uint32_t freq_hz;          ///< I2C frequency in Hz (0 = use bus default)
} pcf8574_config_t;

/**
 * @brief Initialize a PCF8574 device
 *
 * @param config Configuration structure
 * @param handle Pointer to store the device handle
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t pcf8574_init(const pcf8574_config_t *config, pcf8574_handle_t *handle);

/**
 * @brief Deinitialize a PCF8574 device
 *
 * @param handle Device handle
 * @return ESP_OK on success
 */
esp_err_t pcf8574_deinit(pcf8574_handle_t handle);

/**
 * @brief Read all 8 I/O pins from PCF8574
 *
 * @param handle Device handle
 * @param value Pointer to store the read value (bit 0 = P0, bit 7 = P7)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t pcf8574_read(pcf8574_handle_t handle, uint8_t *value);

/**
 * @brief Write all 8 I/O pins to PCF8574
 *
 * @param handle Device handle
 * @param value Value to write (bit 0 = P0, bit 7 = P7)
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t pcf8574_write(pcf8574_handle_t handle, uint8_t value);

/**
 * @brief Scan for PCF8574 devices on the I2C bus
 *
 * @param expected_addresses Array of expected I2C addresses to check
 * @param num_addresses Number of addresses in the array
 * @param found_addresses Output array of found addresses (can be NULL)
 * @param num_found Pointer to store number of found devices (can be NULL)
 * @return ESP_OK on success
 */
esp_err_t pcf8574_scan(const uint8_t *expected_addresses, size_t num_addresses,
                       uint8_t *found_addresses, size_t *num_found);

#ifdef __cplusplus
}
#endif
