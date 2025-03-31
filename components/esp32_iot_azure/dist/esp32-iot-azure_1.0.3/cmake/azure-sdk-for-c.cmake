### AZURE SDK FOR C
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/azure-sdk-for-c/CMakeLists.txt

message("Azure SDK C: added SDK and IoT Hub")

set(AZURE_SDK_FOR_C_PATH ${AZURE_IOT_MIDDLEWARE_FREERTOS}/libraries/azure-sdk-for-c/sdk)
set(AZURE_SDK_FOR_C_INCLUDE_DIRS ${AZURE_SDK_FOR_C_PATH}/inc)

# SDK and IoT Hub

file(GLOB_RECURSE AZURE_SDK_FOR_C_CORE_SOURCES ${AZURE_SDK_FOR_C_PATH}/src/azure/core/*.c)
file(GLOB_RECURSE AZURE_SDK_FOR_C_IOT_SOURCES ${AZURE_SDK_FOR_C_PATH}/src/azure/iot/az_iot_hub_client*.c)
file(GLOB_RECURSE AZURE_SDK_FOR_C_PLATFORM_SOURCES ${AZURE_SDK_FOR_C_PATH}/src/azure/platform/*.c)

list(APPEND AZURE_SDK_FOR_C_IOT_SOURCES
     ${AZURE_SDK_FOR_C_PATH}/src/azure/iot/az_iot_common.c
)

# Device Provisioning Service

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED)
    message("Azure SDK C: added Device Provisioning Service")

    list(APPEND AZURE_SDK_FOR_C_IOT_SOURCES
         ${AZURE_SDK_FOR_C_PATH}/src/azure/iot/az_iot_provisioning_client.c
         ${AZURE_SDK_FOR_C_PATH}/src/azure/iot/az_iot_provisioning_client_sas.c
    )
endif()

# Device Update

if(CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED)
    message("Azure SDK C: added Device Update")

    list(APPEND AZURE_SDK_FOR_C_IOT_SOURCES
         ${AZURE_SDK_FOR_C_PATH}/src/azure/iot/az_iot_adu_client.c
    )
endif()

# Finalization

list(APPEND AZURE_SDK_FOR_C_SOURCES
     ${AZURE_SDK_FOR_C_CORE_SOURCES}
     ${AZURE_SDK_FOR_C_IOT_SOURCES}
     ${AZURE_SDK_FOR_C_PLATFORM_SOURCES}
)

list(FILTER AZURE_SDK_FOR_C_SOURCES EXCLUDE REGEX ".*(curl|win32).*")