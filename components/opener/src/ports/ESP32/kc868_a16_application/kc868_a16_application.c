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
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

struct netif;

#define DEMO_APP_INPUT_ASSEMBLY_NUM                100
#define DEMO_APP_OUTPUT_ASSEMBLY_NUM               150

#define OUTPUT_ASSEMBLY_SIZE                      2
#define INPUT_ASSEMBLY_SIZE                       2

#define I2C_PORT                I2C_NUM_0
#define I2C_SDA_GPIO            4
#define I2C_SCL_GPIO            5
#define I2C_FREQ_HZ             400000
#define I2C_TIMEOUT_MS          100

#define PCF8574_ADDR_INPUTS_1_8  0x22
#define PCF8574_ADDR_INPUTS_9_16 0x21
#define PCF8574_ADDR_OUTPUTS_1_8 0x24
#define PCF8574_ADDR_OUTPUTS_9_16 0x25

static EipUint8 s_input_assembly_data[INPUT_ASSEMBLY_SIZE];
static EipUint8 s_output_assembly_data[OUTPUT_ASSEMBLY_SIZE];
static bool s_i2c_initialized = false;

static esp_err_t Pcf8574Write(uint8_t addr, uint8_t value) {
  return i2c_master_write_to_device(I2C_PORT, addr, &value, 1,
                                    pdMS_TO_TICKS(I2C_TIMEOUT_MS));
}

static esp_err_t Pcf8574Read(uint8_t addr, uint8_t *value) {
  return i2c_master_read_from_device(I2C_PORT, addr, value, 1,
                                     pdMS_TO_TICKS(I2C_TIMEOUT_MS));
}

static void InitializeI2C(void) {
  if (s_i2c_initialized) {
    return;
  }

  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_SDA_GPIO,
    .scl_io_num = I2C_SCL_GPIO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_FREQ_HZ,
  };
  if (i2c_param_config(I2C_PORT, &conf) != ESP_OK) {
    return;
  }
  if (i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0) != ESP_OK) {
    return;
  }

  s_i2c_initialized = true;

  Pcf8574Write(PCF8574_ADDR_INPUTS_1_8, 0xFF);
  Pcf8574Write(PCF8574_ADDR_INPUTS_9_16, 0xFF);
  Pcf8574Write(PCF8574_ADDR_OUTPUTS_1_8, 0xFF);
  Pcf8574Write(PCF8574_ADDR_OUTPUTS_9_16, 0xFF);
}

static void UpdateInputs(void) {
  if (!s_i2c_initialized) {
    s_input_assembly_data[0] = 0;
    s_input_assembly_data[1] = 0;
    return;
  }

  uint8_t input_1_8 = 0xFF;
  if (Pcf8574Read(PCF8574_ADDR_INPUTS_1_8, &input_1_8) == ESP_OK) {
    s_input_assembly_data[0] = (uint8_t)~input_1_8;
  } else {
    s_input_assembly_data[0] = 0;
  }

  uint8_t input_9_16 = 0xFF;
  if (Pcf8574Read(PCF8574_ADDR_INPUTS_9_16, &input_9_16) == ESP_OK) {
    s_input_assembly_data[1] = (uint8_t)~input_9_16;
  } else {
    s_input_assembly_data[1] = 0;
  }
}

static void UpdateOutputs(void) {
  if (!s_i2c_initialized) {
    return;
  }

  uint8_t outputs_1_8 = (uint8_t)~s_output_assembly_data[0];
  Pcf8574Write(PCF8574_ADDR_OUTPUTS_1_8, outputs_1_8);

  uint8_t outputs_9_16 = (uint8_t)~s_output_assembly_data[1];
  Pcf8574Write(PCF8574_ADDR_OUTPUTS_9_16, outputs_9_16);
}

EipStatus ApplicationInitialization(void) {
  InitializeI2C();

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

