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

#include "webui_api.h"
#include "ciptcpipinterface.h"
#include "nvtcpip.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "lwip/inet.h"
#include <string.h>

static const char *TAG = "webui_api";

// Mutex for protecting g_tcpip structure access (shared between OpENer task and API handlers)
static SemaphoreHandle_t s_tcpip_mutex = NULL;
static SemaphoreHandle_t s_tcpip_mutex_creation_mutex = NULL;

static SemaphoreHandle_t get_tcpip_mutex(void) {
  if (s_tcpip_mutex != NULL) {
    return s_tcpip_mutex;
  }
  
  if (s_tcpip_mutex_creation_mutex == NULL) {
    s_tcpip_mutex_creation_mutex = xSemaphoreCreateMutex();
    if (s_tcpip_mutex_creation_mutex == NULL) {
      return NULL;
    }
  }
  
  if (xSemaphoreTake(s_tcpip_mutex_creation_mutex, portMAX_DELAY) == pdTRUE) {
    if (s_tcpip_mutex == NULL) {
      s_tcpip_mutex = xSemaphoreCreateMutex();
    }
    xSemaphoreGive(s_tcpip_mutex_creation_mutex);
  }
  
  return s_tcpip_mutex;
}

// Helper function to send JSON response
static esp_err_t send_json_response(httpd_req_t *req, cJSON *json, esp_err_t status_code)
{
    char *json_str = cJSON_Print(json);
    if (json_str == NULL) {
        cJSON_Delete(json);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_status(req, status_code == ESP_OK ? "200 OK" : "400 Bad Request");
    httpd_resp_send(req, json_str, strlen(json_str));
    
    free(json_str);
    cJSON_Delete(json);
    return ESP_OK;
}

// Helper function to send JSON error response
static esp_err_t send_json_error(httpd_req_t *req, const char *message, int http_status)
{
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "status", "error");
    cJSON_AddStringToObject(json, "message", message);
    
    char *json_str = cJSON_Print(json);
    if (json_str == NULL) {
        cJSON_Delete(json);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    
    httpd_resp_set_type(req, "application/json");
    if (http_status == 400) {
        httpd_resp_set_status(req, "400 Bad Request");
    } else if (http_status == 500) {
        httpd_resp_set_status(req, "500 Internal Server Error");
    } else {
        httpd_resp_set_status(req, "400 Bad Request");
    }
    httpd_resp_send(req, json_str, strlen(json_str));
    
    free(json_str);
    cJSON_Delete(json);
    return ESP_OK;
}

// Helper function to convert IP string to uint32_t (network byte order)
static uint32_t ip_string_to_uint32(const char *ip_str)
{
    if (ip_str == NULL || strlen(ip_str) == 0) {
        return 0;
    }
    struct in_addr addr;
    if (inet_aton(ip_str, &addr) == 0) {
        return 0;
    }
    return addr.s_addr;
}

// Helper function to convert uint32_t (network byte order) to IP string
static void ip_uint32_to_string(uint32_t ip, char *buf, size_t buf_size)
{
    struct in_addr addr;
    addr.s_addr = ip;
    const char *ip_str = inet_ntoa(addr);
    if (ip_str != NULL) {
        strncpy(buf, ip_str, buf_size - 1);
        buf[buf_size - 1] = '\0';
    } else {
        buf[0] = '\0';
    }
}

// GET /api/ipconfig - Get IP configuration
static esp_err_t api_get_ipconfig_handler(httpd_req_t *req)
{
    s_tcpip_mutex = get_tcpip_mutex();
    if (s_tcpip_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create TCP/IP mutex");
        return send_json_error(req, "Internal error: mutex creation failed", 500);
    }
    
    // Always read from OpENer's g_tcpip (single source of truth)
    // Protect with mutex to prevent race conditions with OpENer task
    if (xSemaphoreTake(s_tcpip_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGW(TAG, "Timeout waiting for TCP/IP mutex");
        return send_json_error(req, "Timeout accessing IP configuration", 500);
    }
    
    bool use_dhcp = (g_tcpip.config_control & kTcpipCfgCtrlMethodMask) == kTcpipCfgCtrlDhcp;
    
    // Copy values to local variables while holding mutex
    uint32_t ip_address = g_tcpip.interface_configuration.ip_address;
    uint32_t network_mask = g_tcpip.interface_configuration.network_mask;
    uint32_t gateway = g_tcpip.interface_configuration.gateway;
    uint32_t name_server = g_tcpip.interface_configuration.name_server;
    uint32_t name_server_2 = g_tcpip.interface_configuration.name_server_2;
    
    xSemaphoreGive(s_tcpip_mutex);
    
    // Build JSON response outside of mutex (safer, no blocking)
    cJSON *json = cJSON_CreateObject();
    cJSON_AddBoolToObject(json, "use_dhcp", use_dhcp);
    
    char ip_str[16];
    ip_uint32_to_string(ip_address, ip_str, sizeof(ip_str));
    cJSON_AddStringToObject(json, "ip_address", ip_str);
    
    ip_uint32_to_string(network_mask, ip_str, sizeof(ip_str));
    cJSON_AddStringToObject(json, "netmask", ip_str);
    
    ip_uint32_to_string(gateway, ip_str, sizeof(ip_str));
    cJSON_AddStringToObject(json, "gateway", ip_str);
    
    ip_uint32_to_string(name_server, ip_str, sizeof(ip_str));
    cJSON_AddStringToObject(json, "dns1", ip_str);
    
    ip_uint32_to_string(name_server_2, ip_str, sizeof(ip_str));
    cJSON_AddStringToObject(json, "dns2", ip_str);
    
    return send_json_response(req, json, ESP_OK);
}

// POST /api/ipconfig - Set IP configuration
static esp_err_t api_post_ipconfig_handler(httpd_req_t *req)
{
    char content[512];
    int ret = httpd_req_recv(req, content, sizeof(content) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    content[ret] = '\0';
    
    cJSON *json = cJSON_Parse(content);
    if (json == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }
    
    // Parse JSON first (before taking mutex)
    cJSON *item = cJSON_GetObjectItem(json, "use_dhcp");
    bool use_dhcp_requested = false;
    bool use_dhcp_set = false;
    if (item != NULL && cJSON_IsBool(item)) {
        use_dhcp_requested = cJSON_IsTrue(item);
        use_dhcp_set = true;
    }
    
    // Parse IP configuration values
    uint32_t ip_address_new = 0;
    uint32_t network_mask_new = 0;
    uint32_t gateway_new = 0;
    uint32_t name_server_new = 0;
    uint32_t name_server_2_new = 0;
    bool ip_address_set = false;
    bool network_mask_set = false;
    bool gateway_set = false;
    bool name_server_set = false;
    bool name_server_2_set = false;
    
    // Read current config_control to determine if we should parse IP settings
    bool is_static_ip = false;
    if (s_tcpip_mutex != NULL && xSemaphoreTake(s_tcpip_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        is_static_ip = ((g_tcpip.config_control & kTcpipCfgCtrlMethodMask) == kTcpipCfgCtrlStaticIp);
        xSemaphoreGive(s_tcpip_mutex);
    }
    
    if (is_static_ip || !use_dhcp_requested) {
        item = cJSON_GetObjectItem(json, "ip_address");
        if (item != NULL && cJSON_IsString(item)) {
            ip_address_new = ip_string_to_uint32(cJSON_GetStringValue(item));
            ip_address_set = true;
        }
        
        item = cJSON_GetObjectItem(json, "netmask");
        if (item != NULL && cJSON_IsString(item)) {
            network_mask_new = ip_string_to_uint32(cJSON_GetStringValue(item));
            network_mask_set = true;
        }
        
        item = cJSON_GetObjectItem(json, "gateway");
        if (item != NULL && cJSON_IsString(item)) {
            gateway_new = ip_string_to_uint32(cJSON_GetStringValue(item));
            gateway_set = true;
        }
    }
    
    item = cJSON_GetObjectItem(json, "dns1");
    if (item != NULL && cJSON_IsString(item)) {
        name_server_new = ip_string_to_uint32(cJSON_GetStringValue(item));
        name_server_set = true;
    }
    
    item = cJSON_GetObjectItem(json, "dns2");
    if (item != NULL && cJSON_IsString(item)) {
        name_server_2_new = ip_string_to_uint32(cJSON_GetStringValue(item));
        name_server_2_set = true;
    }
    
    cJSON_Delete(json);
    
    s_tcpip_mutex = get_tcpip_mutex();
    if (s_tcpip_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create TCP/IP mutex");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Internal error: mutex creation failed");
        return ESP_FAIL;
    }
    
    // Update g_tcpip with mutex protection
    if (xSemaphoreTake(s_tcpip_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        ESP_LOGW(TAG, "Timeout waiting for TCP/IP mutex");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Timeout accessing IP configuration");
        return ESP_FAIL;
    }
    
    // Update configuration control
    if (use_dhcp_set) {
        if (use_dhcp_requested) {
            g_tcpip.config_control &= ~kTcpipCfgCtrlMethodMask;
            g_tcpip.config_control |= kTcpipCfgCtrlDhcp;
            g_tcpip.interface_configuration.ip_address = 0;
            g_tcpip.interface_configuration.network_mask = 0;
            g_tcpip.interface_configuration.gateway = 0;
        } else {
            g_tcpip.config_control &= ~kTcpipCfgCtrlMethodMask;
            g_tcpip.config_control |= kTcpipCfgCtrlStaticIp;
        }
    }
    
    // Update IP settings if provided
    if (ip_address_set) {
        g_tcpip.interface_configuration.ip_address = ip_address_new;
    }
    if (network_mask_set) {
        g_tcpip.interface_configuration.network_mask = network_mask_new;
    }
    if (gateway_set) {
        g_tcpip.interface_configuration.gateway = gateway_new;
    }
    if (name_server_set) {
        g_tcpip.interface_configuration.name_server = name_server_new;
    }
    if (name_server_2_set) {
        g_tcpip.interface_configuration.name_server_2 = name_server_2_new;
    }
    
    // Save to NVS while holding mutex
    EipStatus nvs_status = NvTcpipStore(&g_tcpip);
    xSemaphoreGive(s_tcpip_mutex);
    
    if (nvs_status != kEipStatusOk) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to save IP configuration");
        return ESP_FAIL;
    }
    
    cJSON *response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "status", "ok");
    cJSON_AddStringToObject(response, "message", "IP configuration saved successfully. Reboot required to apply changes.");
    
    return send_json_response(req, response, ESP_OK);
}

void webui_register_api_handlers(httpd_handle_t server)
{
    if (server == NULL) {
        ESP_LOGE(TAG, "Cannot register API handlers: server handle is NULL!");
        return;
    }
    
    ESP_LOGI(TAG, "Registering API handlers...");
    
    // GET /api/ipconfig
    httpd_uri_t get_ipconfig_uri = {
        .uri       = "/api/ipconfig",
        .method    = HTTP_GET,
        .handler   = api_get_ipconfig_handler,
        .user_ctx  = NULL
    };
    esp_err_t ret = httpd_register_uri_handler(server, &get_ipconfig_uri);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register GET /api/ipconfig: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered GET /api/ipconfig handler");
    }
    
    // POST /api/ipconfig
    httpd_uri_t post_ipconfig_uri = {
        .uri       = "/api/ipconfig",
        .method    = HTTP_POST,
        .handler   = api_post_ipconfig_handler,
        .user_ctx  = NULL
    };
    ret = httpd_register_uri_handler(server, &post_ipconfig_uri);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register POST /api/ipconfig: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Registered POST /api/ipconfig handler");
    }
    
    ESP_LOGI(TAG, "API handler registration complete");
}
