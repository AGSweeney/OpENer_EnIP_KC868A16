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

#include <stdbool.h>

#include "opener_api.h"
#include "appcontype.h"
#include "trace.h"
#include "cipidentity.h"
#include "ciptcpipinterface.h"
#include "cipqos.h"
#include "cipstring.h"
#include "ciptypes.h"
#include "typedefs.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "i2c_manager.h"
#include "pcf8574.h"

struct netif;

#define DEMO_APP_INPUT_ASSEMBLY_NUM                100
#define DEMO_APP_OUTPUT_ASSEMBLY_NUM               150

#define OUTPUT_ASSEMBLY_SIZE                      2
#define DIGITAL_INPUT_BYTES                       2
#define ANALOG_INPUT_COUNT                        4
#define ANALOG_INPUT_BYTES_PER_CHANNEL            2
#define INPUT_ANALOG_START_OFFSET                 DIGITAL_INPUT_BYTES
#define INPUT_ASSEMBLY_SIZE                       (DIGITAL_INPUT_BYTES + \
                                                   (ANALOG_INPUT_COUNT * \
                                                    ANALOG_INPUT_BYTES_PER_CHANNEL))

#define I2C_SDA_GPIO            4
#define I2C_SCL_GPIO            5
#define I2C_FREQ_HZ             400000

#define PCF8574_ADDR_INPUTS_1_8  0x22
#define PCF8574_ADDR_INPUTS_9_16 0x21
#define PCF8574_ADDR_OUTPUTS_1_8 0x24
#define PCF8574_ADDR_OUTPUTS_9_16 0x25

#define ANALOG_A1  36  /* Physical terminal A1 - INA1 (4-20mA) */
#define ANALOG_A2  34  /* Physical terminal A2 - INA2 (0-5V) */
#define ANALOG_A3  35  /* Physical terminal A3 - INA3 (0-5V) */
#define ANALOG_A4  39  /* Physical terminal A4 - INA4 (4-20mA) */

static const adc_channel_t kAnalogChannels[ANALOG_INPUT_COUNT] = {
  ADC_CHANNEL_0,  /* GPIO36 - A1/INA1 (4-20mA) */
  ADC_CHANNEL_6,  /* GPIO34 - A2/INA2 (0-5V) */
  ADC_CHANNEL_7,  /* GPIO35 - A3/INA3 (0-5V) */
  ADC_CHANNEL_3,  /* GPIO39 - A4/INA4 (4-20mA) */
};

static const char *TAG_IO = "kc868_io";

static EipUint8 s_input_assembly_data[INPUT_ASSEMBLY_SIZE];
static EipUint8 s_output_assembly_data[OUTPUT_ASSEMBLY_SIZE];
static bool s_pcf8574_initialized = false;
static bool s_adc_initialized = false;
static adc_oneshot_unit_handle_t s_adc_handle = NULL;
static pcf8574_handle_t s_pcf8574_inputs_1_8 = NULL;
static pcf8574_handle_t s_pcf8574_inputs_9_16 = NULL;
static pcf8574_handle_t s_pcf8574_outputs_1_8 = NULL;
static pcf8574_handle_t s_pcf8574_outputs_9_16 = NULL;

static void InitializeI2C(void) {
  if (s_pcf8574_initialized) {
    return;
  }

  // Initialize I2C manager
  esp_err_t ret = i2c_manager_init(I2C_SDA_GPIO, I2C_SCL_GPIO, I2C_FREQ_HZ);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize I2C manager: %s", esp_err_to_name(ret));
    return;
  }

  // Scan for PCF8574 devices
  const uint8_t expected_addresses[] = {
    PCF8574_ADDR_INPUTS_1_8,
    PCF8574_ADDR_INPUTS_9_16,
    PCF8574_ADDR_OUTPUTS_1_8,
    PCF8574_ADDR_OUTPUTS_9_16,
  };
  const char *device_names[] = {
    "Inputs X01-X08",
    "Inputs X09-X16",
    "Outputs Y01-Y08",
    "Outputs Y09-Y16",
  };

  ESP_LOGI(TAG_IO, "Checking PCF8574 device presence...");
  size_t num_found = 0;
  uint8_t found_addresses[4] = {0};
  ret = pcf8574_scan(expected_addresses, sizeof(expected_addresses), found_addresses, &num_found);
  if (ret == ESP_OK) {
    for (size_t i = 0; i < sizeof(expected_addresses); i++) {
      bool found = false;
      for (size_t j = 0; j < num_found; j++) {
        if (found_addresses[j] == expected_addresses[i]) {
          ESP_LOGI(TAG_IO, "  [OK] PCF8574 at 0x%02X - %s", expected_addresses[i], device_names[i]);
          found = true;
          break;
        }
      }
      if (!found) {
        ESP_LOGW(TAG_IO, "  [FAIL] PCF8574 at 0x%02X - %s not found", expected_addresses[i], device_names[i]);
      }
    }
    ESP_LOGI(TAG_IO, "PCF8574 scan complete: %zu/%zu devices found", num_found, sizeof(expected_addresses));
  }

  // Initialize PCF8574 devices
  pcf8574_config_t config = {
    .freq_hz = I2C_FREQ_HZ,
  };

  config.address = PCF8574_ADDR_INPUTS_1_8;
  ret = pcf8574_init(&config, &s_pcf8574_inputs_1_8);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize PCF8574 inputs_1_8 (0x%02X): %s", 
             PCF8574_ADDR_INPUTS_1_8, esp_err_to_name(ret));
    return;
  }

  config.address = PCF8574_ADDR_INPUTS_9_16;
  ret = pcf8574_init(&config, &s_pcf8574_inputs_9_16);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize PCF8574 inputs_9_16 (0x%02X): %s", 
             PCF8574_ADDR_INPUTS_9_16, esp_err_to_name(ret));
    return;
  }

  config.address = PCF8574_ADDR_OUTPUTS_1_8;
  ret = pcf8574_init(&config, &s_pcf8574_outputs_1_8);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize PCF8574 outputs_1_8 (0x%02X): %s", 
             PCF8574_ADDR_OUTPUTS_1_8, esp_err_to_name(ret));
    return;
  }

  config.address = PCF8574_ADDR_OUTPUTS_9_16;
  ret = pcf8574_init(&config, &s_pcf8574_outputs_9_16);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize PCF8574 outputs_9_16 (0x%02X): %s", 
             PCF8574_ADDR_OUTPUTS_9_16, esp_err_to_name(ret));
    return;
  }

  s_pcf8574_initialized = true;
  ESP_LOGI(TAG_IO, "PCF8574 devices initialized successfully");

  // Initialize all PCF8574 outputs to 0xFF (all relays OFF - active low)
  uint8_t init_value = 0xFF;
  pcf8574_write(s_pcf8574_inputs_1_8, init_value);
  pcf8574_write(s_pcf8574_inputs_9_16, init_value);
  ret = pcf8574_write(s_pcf8574_outputs_1_8, init_value);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize outputs_1_8: %s", esp_err_to_name(ret));
  }
  ret = pcf8574_write(s_pcf8574_outputs_9_16, init_value);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to initialize outputs_9_16: %s", esp_err_to_name(ret));
  }
}

static void InitializeAdc(void) {
  if (s_adc_initialized) {
    return;
  }

  adc_oneshot_unit_init_cfg_t unit_cfg = {
    .unit_id = ADC_UNIT_1,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  if (adc_oneshot_new_unit(&unit_cfg, &s_adc_handle) != ESP_OK) {
    s_adc_handle = NULL;
    return;
  }

  adc_oneshot_chan_cfg_t chan_cfg = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_12,
  };
  
  for (size_t channel_index = 0; channel_index < ANALOG_INPUT_COUNT;
       ++channel_index) {
    if (adc_oneshot_config_channel(s_adc_handle,
                                   kAnalogChannels[channel_index],
                                   &chan_cfg) != ESP_OK) {
      ESP_LOGE(TAG_IO, "Failed to configure ADC channel %zu", channel_index);
      adc_oneshot_del_unit(s_adc_handle);
      s_adc_handle = NULL;
      return;
    }
  }

  s_adc_initialized = true;
}

static void StoreAnalogValue(size_t channel_index, uint16_t value) {
  size_t offset = INPUT_ANALOG_START_OFFSET +
                  (channel_index * ANALOG_INPUT_BYTES_PER_CHANNEL);
  s_input_assembly_data[offset] = (uint8_t)(value & 0xFF);
  s_input_assembly_data[offset + 1] = (uint8_t)(value >> 8);
}

static void UpdateInputs(void) {
  if (!s_pcf8574_initialized) {
    s_input_assembly_data[0] = 0;
    s_input_assembly_data[1] = 0;
  } else {
    uint8_t input_1_8 = 0xFF;
    if (pcf8574_read(s_pcf8574_inputs_1_8, &input_1_8) == ESP_OK) {
      s_input_assembly_data[0] = (uint8_t)~input_1_8;
    } else {
      s_input_assembly_data[0] = 0;
    }

    uint8_t input_9_16 = 0xFF;
    if (pcf8574_read(s_pcf8574_inputs_9_16, &input_9_16) == ESP_OK) {
      s_input_assembly_data[1] = (uint8_t)~input_9_16;
    } else {
      s_input_assembly_data[1] = 0;
    }
  }

  if (!s_adc_initialized) {
    for (size_t channel_index = 0; channel_index < ANALOG_INPUT_COUNT;
         ++channel_index) {
      StoreAnalogValue(channel_index, 0);
    }
    return;
  }

  for (size_t channel_index = 0; channel_index < ANALOG_INPUT_COUNT;
       ++channel_index) {
    int raw_value = 0;
    if (adc_oneshot_read(s_adc_handle, kAnalogChannels[channel_index],
                         &raw_value) != ESP_OK) {
      raw_value = 0;
    }
    if (raw_value < 0) {
      raw_value = 0;
    }
    StoreAnalogValue(channel_index, (uint16_t)raw_value);
  }
}

static void UpdateOutputs(void) {
  if (!s_pcf8574_initialized) {
    ESP_LOGW(TAG_IO, "UpdateOutputs called but PCF8574 not initialized");
    return;
  }

  uint8_t outputs_1_8 = (uint8_t)~s_output_assembly_data[0];
  esp_err_t ret = pcf8574_write(s_pcf8574_outputs_1_8, outputs_1_8);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to write outputs_1_8 (0x%02X): %s", outputs_1_8, esp_err_to_name(ret));
  }

  uint8_t outputs_9_16 = (uint8_t)~s_output_assembly_data[1];
  ret = pcf8574_write(s_pcf8574_outputs_9_16, outputs_9_16);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG_IO, "Failed to write outputs_9_16 (0x%02X): %s", outputs_9_16, esp_err_to_name(ret));
  }
}

EipStatus ApplicationInitialization(void) {
  InitializeI2C();
  InitializeAdc();

  CreateAssemblyObject(DEMO_APP_OUTPUT_ASSEMBLY_NUM, s_output_assembly_data,
                       OUTPUT_ASSEMBLY_SIZE);

  CreateAssemblyObject(DEMO_APP_INPUT_ASSEMBLY_NUM, s_input_assembly_data,
                       INPUT_ASSEMBLY_SIZE);

  ConfigureExclusiveOwnerConnectionPoint(0, DEMO_APP_OUTPUT_ASSEMBLY_NUM,
                                        DEMO_APP_INPUT_ASSEMBLY_NUM, 0);
  ConfigureInputOnlyConnectionPoint(0, DEMO_APP_OUTPUT_ASSEMBLY_NUM,
                                    DEMO_APP_INPUT_ASSEMBLY_NUM, 0);
  ConfigureListenOnlyConnectionPoint(0, DEMO_APP_OUTPUT_ASSEMBLY_NUM,
                                     DEMO_APP_INPUT_ASSEMBLY_NUM, 0);
  CipRunIdleHeaderSetO2T(false);
  CipRunIdleHeaderSetT2O(false);

  return kEipStatusOk;
}

void HandleApplication(void) {
}

void CheckIoConnectionEvent(unsigned int output_assembly_id,
                            unsigned int input_assembly_id,
                            IoConnectionEvent io_connection_event) {
  (void) output_assembly_id;
  (void) input_assembly_id;
  (void) io_connection_event;
}

EipStatus AfterAssemblyDataReceived(CipInstance *instance) {
  if (instance->instance_number == DEMO_APP_OUTPUT_ASSEMBLY_NUM) {
    UpdateOutputs();
    UpdateInputs();
  }
  return kEipStatusOk;
}

EipBool8 BeforeAssemblyDataSend(CipInstance *instance) {
  if (instance->instance_number == DEMO_APP_INPUT_ASSEMBLY_NUM) {
    UpdateInputs();
  }
  return true;
}

EipStatus ResetDevice(void) {
  CloseAllConnections();
  CipQosUpdateUsedSetQosValues();
  return kEipStatusOk;
}

EipStatus ResetDeviceToInitialConfiguration(void) {
  g_tcpip.encapsulation_inactivity_timeout = 120;
  CipQosResetAttributesToDefaultValues();
  CloseAllConnections();
  return kEipStatusOk;
}

void* CipCalloc(size_t number_of_elements, size_t size_of_element) {
  return calloc(number_of_elements, size_of_element);
}

void CipFree(void *data) {
  free(data);
}

void RunIdleChanged(EipUint32 run_idle_value) {
  (void) run_idle_value;
}

void KC868_A16_ApplicationNotifyLinkUp(void) {
}

void KC868_A16_ApplicationNotifyLinkDown(void) {
}

void KC868_A16_ApplicationSetActiveNetif(struct netif *netif) {
  (void) netif;
}

#if defined(OPENER_ETHLINK_CNTRS_ENABLE) && 0 != OPENER_ETHLINK_CNTRS_ENABLE
EipStatus EthLnkPreGetCallback(CipInstance *instance,
                               CipAttributeStruct *attribute,
                               CipByte service) {
  (void) instance;
  (void) attribute;
  (void) service;
  return kEipStatusOk;
}

EipStatus EthLnkPostGetCallback(CipInstance *instance,
                              CipAttributeStruct *attribute,
                              CipByte service) {
  (void) instance;
  (void) attribute;
  (void) service;
  return kEipStatusOk;
}
#else
EipStatus EthLnkPreGetCallback(CipInstance *instance,
                               CipAttributeStruct *attribute,
                               CipByte service) {
  (void) instance;
  (void) attribute;
  (void) service;
  return kEipStatusOk;
}

EipStatus EthLnkPostGetCallback(CipInstance *instance,
                                CipAttributeStruct *attribute,
                                CipByte service) {
  (void) instance;
  (void) attribute;
  (void) service;
  return kEipStatusOk;
}
#endif

