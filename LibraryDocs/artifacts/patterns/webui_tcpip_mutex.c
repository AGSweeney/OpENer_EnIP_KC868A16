// EXCERPT — source: components/webui/src/webui_api.c
// EVIDENCE: E1 | symbol: api_get_ipconfig_handler | lines: 36-150

/* g_tcpip is shared with OpENer task — always take mutex before read/write */
static SemaphoreHandle_t s_tcpip_mutex = NULL;

static esp_err_t api_get_ipconfig_handler(httpd_req_t *req)
{
    s_tcpip_mutex = get_tcpip_mutex();
    if (s_tcpip_mutex == NULL) {
        return send_json_error(req, "Internal error: mutex creation failed", 500);
    }

    if (xSemaphoreTake(s_tcpip_mutex, pdMS_TO_TICKS(1000)) != pdTRUE) {
        return send_json_error(req, "Timeout accessing IP configuration", 500);
    }

    /* Read from OpENer's g_tcpip (single source of truth) */
    bool is_dhcp = ((g_tcpip.config_control & kTcpipCfgCtrlMethodMask)
                    == kTcpipCfgCtrlDhcp);
    /* ... build JSON from g_tcpip fields ... */
    xSemaphoreGive(s_tcpip_mutex);
    return send_json_response(req, json, ESP_OK);
}
