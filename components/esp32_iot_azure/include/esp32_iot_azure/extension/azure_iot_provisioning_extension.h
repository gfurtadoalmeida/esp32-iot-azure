#ifndef __ESP32_IOT_AZURE_PROVISIONING_EXT_H__
#define __ESP32_IOT_AZURE_PROVISIONING_EXT_H__

#include "esp32_iot_azure/azure_iot_common.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"
#include "azure_iot_result.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Space in bytes needed to create a basic registration payload.
 * @details Number of bytes, excluding the `NULL` terminator, for: {"modelId":"CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID"}
 */
#define AZURE_DPS_BASIC_REGISTRATION_PAYLOAD_NEEDED_BYTES (AZURE_JSON_OBJECT_MIN_NEEDED_BYTES + AZURE_JSON_CALC_PROPERTY_NEEDED_BYTES("modelId", sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID) - 1))

/**
 * @brief Create an @ref utf8_string_t with a buffer enough for a DPS
 * basic registration payload.
 */
#define UTF8_STRING_FOR_DPS_BASIC_PAYLOAD() UTF8_STRING_WITH_FIXED_LENGTH((AZURE_DPS_BASIC_REGISTRATION_PAYLOAD_NEEDED_BYTES))

    /**
     * @brief Create an Azure IoT Plug and Play registration payload, with just the device model id, that can be passed
     * to @ref azure_dps_set_registration_payload function.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-develop/concepts-developer-guide-device?pivots=programming-language-ansi-c#dps-payload
     * @param[in,out] registration_payload Registration payload with a buffer with at least AZURE_DPS_BASIC_REGISTRATION_PAYLOAD_NEEDED_BYTES.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_create_pnp_registration_payload(utf8_string_t *registration_payload);

#ifdef __cplusplus
}
#endif
#endif