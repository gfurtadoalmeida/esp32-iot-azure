file(GLOB_RECURSE MAIN_EXAMPLES "*.c")

idf_component_register(
    SRCS
        "main.c"
        ${MAIN_EXAMPLES}
    INCLUDE_DIRS
        "."
    REQUIRES
        freertos
        esp32_iot_azure
        nvs_flash
)