// EXCERPT — source: main/main.c
// EVIDENCE: E1 | symbol: got_ip_event_handler | lines: 79-105

static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    if (s_eth_netif != NULL) {
        struct netif *lwip_netif = esp_netif_get_netif_impl(s_eth_netif);
        if (lwip_netif != NULL) {
            ESP_LOGI(TAG, "Initializing OpENer EtherNet/IP stack...");
            opener_init(lwip_netif);

            ESP_LOGI(TAG, "Initializing Web UI...");
            if (!webui_init()) {
                ESP_LOGW(TAG, "Failed to initialize Web UI");
            }
        }
    }
}
