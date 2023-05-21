### CORE MQTT
### Used by: Azure IoT Hub and Azure Device Provisioning Service
### https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/coreMQTT/CMakeLists.txt

message("Core MQTT: added")

set(CORE_MQTT_PATH ${AZURE_IOT_MIDDLEWARE_FREERTOS}/libraries/coreMQTT/source)

file(GLOB CORE_MQTT_SOURCES ${CORE_MQTT_PATH}/*.c)

set(CORE_MQTT_INCLUDE_DIRS
    ${CORE_MQTT_PATH}/include
    ${CORE_MQTT_PATH}/interface
)