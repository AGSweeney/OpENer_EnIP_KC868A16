idf_component_register(
    SRCS
        "src/webui.c"
        "src/webui_api.c"
        "src/webui_html.c"
    INCLUDE_DIRS
        "include"
    REQUIRES
        esp_http_server
        nvs_flash
        json
        lwip
        opener
)

