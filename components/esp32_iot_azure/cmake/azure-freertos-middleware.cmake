### AZURE IOT MIDDLEWARE FREERTOS
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/azure-iot-middleware-freertos/CMakeLists.txt

message("Azure FreeRTOS: added SDK and IoT Hub")

idf_component_get_property(FREERTOS_DIR freertos COMPONENT_DIR)
idf_component_get_property(MBEDTLS_DIR mbedtls COMPONENT_DIR)

set(AZURE_IOT_MIDDLEWARE_FREERTOS_INCLUDE_DIRS
    ${MBEDTLS_DIR}/mbedtls/include
    ${FREERTOS_DIR}/include/freertos
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/include
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/interface
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreMQTT
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreHTTP
)

# SDK and IoT Hub

list(APPEND AZURE_IOT_MIDDLEWARE_FREERTOS_ROOT_SRCS
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot.c
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_hub_client.c
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_hub_client_properties.c
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_json_reader.c
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_json_writer.c
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_message.c
)

list(APPEND AZURE_IOT_MIDDLEWARE_FREERTOS_PORT_SRCS
     ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreMQTT/azure_iot_core_mqtt.c
)

# Device Provisioning Service

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED)
    message("Azure FreeRTOS: added Device Provisioning Service")

    list(APPEND AZURE_IOT_MIDDLEWARE_FREERTOS_ROOT_SRCS
        ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_provisioning_client.c
    )
endif()

# Device Update

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED)
    message("Azure FreeRTOS: added Device Update")

    list(APPEND AZURE_IOT_MIDDLEWARE_FREERTOS_ROOT_SRCS
         ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/azure_iot_adu_client.c
    )

    list(APPEND AZURE_IOT_MIDDLEWARE_FREERTOS_PORT_SRCS
         ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreHTTP/azure_iot_core_http.c
         ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/mbedTLS/azure_iot_jws_mbedtls.c
    )
endif()