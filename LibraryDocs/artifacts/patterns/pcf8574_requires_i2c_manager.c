// EXCERPT — source: components/pcf8574/pcf8574.c
// EVIDENCE: E1 | symbol: pcf8574_init | lines: 38-66

esp_err_t pcf8574_init(const pcf8574_config_t *config, pcf8574_handle_t *handle) {
    if (config == NULL || handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (!i2c_manager_is_initialized()) {
        ESP_LOGE(TAG, "I2C manager not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    i2c_master_bus_handle_t bus_handle;
    esp_err_t ret = i2c_manager_get_bus(&bus_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = config->address,
        .scl_speed_hz = config->freq_hz,
    };
    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev->dev_handle);
    return ret;
}
