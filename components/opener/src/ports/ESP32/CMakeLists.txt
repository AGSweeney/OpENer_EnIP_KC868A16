idf_component_register(
    SRCS 
        "opener.c"
        "networkhandler.c"
        "networkconfig.c"
        "opener_error.c"
    INCLUDE_DIRS 
        "."
        "../.."
        "../../../cip"
        "../../../enet_encap"
        "../../../utils"
        "../../../ports"
        "../../../ports/nvdata"
    PRIV_INCLUDE_DIRS
        "kc868_a16_application"
    REQUIRES 
        lwip
        freertos
        i2c_manager
        pcf8574
    PRIV_REQUIRES
        lwip
        freertos
        i2c_manager
        pcf8574
        driver
)

target_compile_definitions(${COMPONENT_LIB} PRIVATE ESP32)

