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

/**
 * @brief Initialize the I2C manager and create the default I2C bus
 *
 * @param sda_gpio SDA GPIO pin number
 * @param scl_gpio SCL GPIO pin number
 * @param freq_hz I2C bus frequency in Hz
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t i2c_manager_init(int sda_gpio, int scl_gpio, uint32_t freq_hz);

/**
 * @brief Deinitialize the I2C manager
 *
 * @return ESP_OK on success
 */
esp_err_t i2c_manager_deinit(void);

/**
 * @brief Get the default I2C bus handle
 *
 * @param bus_handle Pointer to store the bus handle
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if not initialized
 */
esp_err_t i2c_manager_get_bus(i2c_master_bus_handle_t *bus_handle);

/**
 * @brief Check if I2C manager is initialized
 *
 * @return true if initialized, false otherwise
 */
bool i2c_manager_is_initialized(void);

/**
 * @brief Get the I2C bus frequency that was configured
 *
 * @param freq_hz Pointer to store the frequency in Hz
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if not initialized
 */
esp_err_t i2c_manager_get_freq(uint32_t *freq_hz);

#ifdef __cplusplus
}
#endif
