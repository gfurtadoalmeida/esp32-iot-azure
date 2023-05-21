set(SDK_PATH ".sdk")

set(AZURE_IOT_MIDDLEWARE_FREERTOS ${SDK_PATH}/azure-iot-middleware-freertos)
set(AZURE_IOT_MIDDLEWARE_FREERTOS_CONFIGS ${SDK_PATH}/config)

include(${COMPONENT_DIR}/cmake/azure-freertos-middleware.cmake)
include(${COMPONENT_DIR}/cmake/azure-sdk-for-c.cmake)
include(${COMPONENT_DIR}/cmake/core-mqtt.cmake)
include(${COMPONENT_DIR}/cmake/core-http.cmake)

# SDK and IoT Hub
message("ESP32 IoT Azure: added SDK and IoT Hub")

list(APPEND srcsCOMP
     "src/azure_iot_sdk.c"
     "src/azure_iot_hub.c"
     "src/extension/azure_iot_hub_extension.c"
     "src/extension/azure_iot_json_reader_extension.c"
     "src/extension/azure_iot_message_extension.c"
     "src/infrastructure/azure_iot_certificate.c"
     "src/infrastructure/azure_transport_interface.c"
     "src/infrastructure/backoff_algorithm.c"
     "src/infrastructure/crypto.c"
     "src/infrastructure/static_memory.c"
     "src/infrastructure/time.c"
     "src/infrastructure/transport.c"
)

# Device Provisioning Service

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED)
    message("ESP32 IoT Azure: added Device Provisioning Service")

    list(APPEND srcsCOMP
         "src/azure_iot_provisioning.c"
         "src/extension/azure_iot_provisioning_extension.c")
endif()

# Device Update

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED)
    message("ESP32 IoT Azure: added Device Update")

    list(APPEND srcsCOMP
         "src/azure_iot_adu.c"
         "src/azure_iot_adu_workflow.c"
         "src/azure_iot_http_client.c"
         "src/extension/azure_iot_adu_extension.c"
         "src/extension/azure_iot_http_client_extension.c"
         "src/infrastructure/azure_adu_root_key.c"
         "src/port/azure_iot_flash_platform_port.c"
    )
endif()

idf_component_register(
    SRCS
        ${srcsCOMP}
        ${AZURE_IOT_MIDDLEWARE_FREERTOS_ROOT_SRCS}
        ${AZURE_IOT_MIDDLEWARE_FREERTOS_PORT_SRCS}
        ${AZURE_SDK_FOR_C_SOURCES}
        ${CORE_MQTT_SOURCES}
        ${CORE_HTTP_SOURCES}
    INCLUDE_DIRS
        "include"
        "include/esp32_iot_azure/port"
        ${AZURE_IOT_MIDDLEWARE_FREERTOS_INCLUDE_DIRS}
        ${AZURE_IOT_MIDDLEWARE_FREERTOS_CONFIGS}
        ${AZURE_SDK_FOR_C_INCLUDE_DIRS}
        ${CORE_MQTT_INCLUDE_DIRS}
        ${CORE_HTTP_INCLUDE_DIRS}
    PRIV_INCLUDE_DIRS
        "private_include"
    REQUIRES
        freertos
        mbedtls
        tcp_transport
        app_update
)