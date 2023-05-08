set(SDK_PATH ".sdk")

### AZURE IOT MIDDLEWARE FREERTOS
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/azure-iot-middleware-freertos/CMakeLists.txt

set(AZURE_IOT_MIDDLEWARE_FREERTOS ${SDK_PATH}/azure-iot-middleware-freertos)
set(AZURE_IOT_MIDDLEWARE_FREERTOS_CONFIGS ${SDK_PATH}/config)

file(GLOB_RECURSE AZURE_IOT_MIDDLEWARE_FREERTOS_ROOT_SRCS ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/*.c)
file(GLOB_RECURSE AZURE_IOT_MIDDLEWARE_FREERTOS_PORT_SRCS "src/*.c")

list(APPEND AZURE_IOT_MIDDLEWARE_FREERTOS_PORT_SRCS
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreMQTT/azure_iot_core_mqtt.c
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreHTTP/azure_iot_core_http.c
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/mbedTLS/azure_iot_jws_mbedtls.c
)

idf_component_get_property(MBEDTLS_DIR mbedtls COMPONENT_DIR)
idf_component_get_property(FREERTOS_DIR freertos COMPONENT_DIR)

set(AZURE_IOT_MIDDLEWARE_FREERTOS_INCLUDE_DIRS
    ${MBEDTLS_DIR}/mbedtls/include
    ${FREERTOS_DIR}/include/freertos
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/include
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/source/interface
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreMQTT
    ${AZURE_IOT_MIDDLEWARE_FREERTOS}/ports/coreHTTP
)

### AZURE SDK FOR C
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/azure-sdk-for-c/CMakeLists.txt

set(AZURE_SDK_FOR_C_PATH ${AZURE_IOT_MIDDLEWARE_FREERTOS}/libraries/azure-sdk-for-c/sdk)

file(GLOB_RECURSE AZURE_SDK_FOR_C_SOURCES ${AZURE_SDK_FOR_C_PATH}/src/azure/*.c)

list(FILTER AZURE_SDK_FOR_C_SOURCES EXCLUDE REGEX ".*(curl|win32).*")

set(AZURE_SDK_FOR_C_INCLUDE_DIRS ${AZURE_SDK_FOR_C_PATH}/inc)

### CORE MQTT
### Used by: Azure IoT Hub and Azure Device Provisioning Service
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/coreMQTT/CMakeLists.txt

set(CORE_MQTT_PATH ${AZURE_IOT_MIDDLEWARE_FREERTOS}/libraries/coreMQTT/source)

file(GLOB CORE_MQTT_SOURCES ${CORE_MQTT_PATH}/*.c)

set(CORE_MQTT_INCLUDE_DIRS
    ${CORE_MQTT_PATH}/include
    ${CORE_MQTT_PATH}/interface
)

### CORE HTTP
### Used by: Azure Device Update Service
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/coreHTTP/CMakeLists.txt

set(CORE_HTTP_PATH ${AZURE_IOT_MIDDLEWARE_FREERTOS}/libraries/coreHTTP/source)

file(GLOB CORE_HTTP_SOURCES
    ${CORE_HTTP_PATH}/*.c
    ${CORE_HTTP_PATH}/dependency/3rdparty/http_parser/http_parser.c
)

set(CORE_HTTP_INCLUDE_DIRS
    ${CORE_HTTP_PATH}/include
    ${CORE_HTTP_PATH}/interface
    ${CORE_HTTP_PATH}/dependency/3rdparty/http_parser
)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set_source_files_properties(
    ${CORE_HTTP_PATH}/core_http_client.c
        PROPERTIES COMPILE_FLAGS
        -Wno-stringop-truncation
    )
endif()

### ESP32_IOT_AZURE

file(GLOB srcsCOMP "src/*.c")

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