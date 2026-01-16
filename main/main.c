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

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_net_stack.h"
#include "esp_event.h"
#include "esp_eth.h"
#include "esp_eth_mac.h"
#include "esp_eth_phy.h"
#include "esp_eth_com.h"
#include "esp_eth_mac_esp.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "opener.h"
#include "webui.h"
#include "ciptcpipinterface.h"
#include "nvtcpip.h"

static const char *TAG = "main";
static esp_netif_t *s_eth_netif = NULL;

#define ETH_PHY_ADDR         1
#define ETH_PHY_MDC_PIN       23
#define ETH_PHY_MDIO_PIN      18

static void eth_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "Ethernet Link Down");
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
        ESP_LOGI(TAG, "Ethernet Stopped");
        break;
    default:
        break;
    }
}

static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");

    if (s_eth_netif != NULL) {
        struct netif *lwip_netif = esp_netif_get_netif_impl(s_eth_netif);
        if (lwip_netif != NULL) {
            ESP_LOGI(TAG, "Initializing OpENer EtherNet/IP stack...");
            opener_init(lwip_netif);
            
            ESP_LOGI(TAG, "Initializing Web UI...");
            if (!webui_init()) {
                ESP_LOGW(TAG, "Failed to initialize Web UI");
            }
        } else {
            ESP_LOGE(TAG, "Failed to get lwIP netif from esp_netif");
        }
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Hello World!");
    
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_ETH();
    s_eth_netif = esp_netif_new(&cfg);
    
    bool use_dhcp = true;
    esp_netif_ip_info_t static_ip_info = {0};
    
    EipStatus nv_status = NvTcpipLoad(&g_tcpip);
    if (kEipStatusOk == nv_status) {
        bool is_dhcp = ((g_tcpip.config_control & kTcpipCfgCtrlMethodMask) == kTcpipCfgCtrlDhcp);
        use_dhcp = is_dhcp;
        
        if (!is_dhcp) {
            static_ip_info.ip.addr = g_tcpip.interface_configuration.ip_address;
            static_ip_info.netmask.addr = g_tcpip.interface_configuration.network_mask;
            static_ip_info.gw.addr = g_tcpip.interface_configuration.gateway;
            ESP_LOGI(TAG, "Loaded static IP config from NVS: " IPSTR "/" IPSTR " gw:" IPSTR,
                     IP2STR(&static_ip_info.ip),
                     IP2STR(&static_ip_info.netmask),
                     IP2STR(&static_ip_info.gw));
        } else {
            ESP_LOGI(TAG, "Loaded DHCP config from NVS");
        }
    } else {
        ESP_LOGI(TAG, "No saved IP config in NVS, using DHCP by default");
    }

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
    phy_config.phy_addr = ETH_PHY_ADDR;
    phy_config.reset_gpio_num = -1;

    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp32_emac_config.smi_gpio.mdc_num = ETH_PHY_MDC_PIN;
    esp32_emac_config.smi_gpio.mdio_num = ETH_PHY_MDIO_PIN;
    esp32_emac_config.clock_config.rmii.clock_mode = EMAC_CLK_OUT;
    esp32_emac_config.clock_config.rmii.clock_gpio = 17;

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);

    esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
    esp_eth_handle_t eth_handle = NULL;
    ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));

    ESP_ERROR_CHECK(esp_netif_attach(s_eth_netif, esp_eth_new_netif_glue(eth_handle)));

    if (nv_status == kEipStatusOk && g_tcpip.hostname.length > 0 && g_tcpip.hostname.string != NULL) {
        ESP_ERROR_CHECK(esp_netif_set_hostname(s_eth_netif, (const char *)g_tcpip.hostname.string));
        ESP_LOGI(TAG, "Set hostname from NVS: %s", g_tcpip.hostname.string);
    } else {
        ESP_ERROR_CHECK(esp_netif_set_hostname(s_eth_netif, "KC868-A16-EnIP"));
    }

    if (!use_dhcp) {
        ESP_LOGI(TAG, "Configuring static IP address...");
        ESP_ERROR_CHECK(esp_netif_dhcpc_stop(s_eth_netif));
        ESP_ERROR_CHECK(esp_netif_set_ip_info(s_eth_netif, &static_ip_info));
        
        if (g_tcpip.interface_configuration.name_server != 0) {
            esp_netif_dns_info_t dns_info;
            dns_info.ip.u_addr.ip4.addr = g_tcpip.interface_configuration.name_server;
            dns_info.ip.type = IPADDR_TYPE_V4;
            ESP_ERROR_CHECK(esp_netif_set_dns_info(s_eth_netif, ESP_NETIF_DNS_MAIN, &dns_info));
        }
        if (g_tcpip.interface_configuration.name_server_2 != 0) {
            esp_netif_dns_info_t dns_info;
            dns_info.ip.u_addr.ip4.addr = g_tcpip.interface_configuration.name_server_2;
            dns_info.ip.type = IPADDR_TYPE_V4;
            ESP_ERROR_CHECK(esp_netif_set_dns_info(s_eth_netif, ESP_NETIF_DNS_BACKUP, &dns_info));
        }
    } else {
        ESP_LOGI(TAG, "Using DHCP for IP configuration");
        ESP_ERROR_CHECK(esp_netif_dhcpc_start(s_eth_netif));
    }

    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL));
    
    ESP_ERROR_CHECK(esp_eth_start(eth_handle));
    
    while (1) {
    vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
