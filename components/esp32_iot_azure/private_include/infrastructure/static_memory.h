#ifndef __ESP32_IOT_AZURE_INFRA_STATIC_MEMORY_H__
#define __ESP32_IOT_AZURE_INFRA_STATIC_MEMORY_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Azure IoT Hub MQTT buffer.
     * @note Size: @ref CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE_IOT_HUB.
     */
    extern uint8_t *STATIC_MEMORY_IOT_HUB;

    /**
     * @brief Azure DPS MQTT buffer.
     * @note Size: @ref CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE_DPS.
     */
    extern uint8_t *STATIC_MEMORY_DPS;

    /**
     * @brief Azure Device Update scratch buffer.
     * @note Size: @ref CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH.
     */
    extern uint8_t *STATIC_MEMORY_ADU_SCRATCH_BUFFER;

    /**
     * @brief Azure Device Update download.
     * @note Size: @ref CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_HTTP_DOWNLOAD.
     */
    extern uint8_t *STATIC_MEMORY_ADU_DOWNLOAD;

#endif
#ifdef __cplusplus
}
#endif