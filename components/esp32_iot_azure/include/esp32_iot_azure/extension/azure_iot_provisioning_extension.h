#ifndef __ESP32_IOT_AZURE_PROVISIONING_EXT_H__
#define __ESP32_IOT_AZURE_PROVISIONING_EXT_H__

#include "esp32_iot_azure/azure_iot_common.h"
#include "esp32_iot_azure/azure_iot_provisioning.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"
#include "azure_iot_result.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Space needed for a Azure IoT Plug and Play registration payload
 * containg just the configured "modelId" property.
 * @details Number of bytes, excluding the `NULL` terminator.
 * @return Space need in bytes.
 */
#define AZURE_DPS_SIZEOF_PNP_REGISTRATION_PAYLOAD (AZURE_JSON_SIZEOF_OBJECT_EMPTY + AZURE_JSON_SIZEOF_OBJECT_PROPERTY("modelId", sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID) - 1))

/**
 * @brief Create an @ref buffer_t with enough space for an
 * Azure IoT Plug and Play registration payload containg just the
 * configured "modelId" property.
 * @note Helper for @ref azure_dps_create_pnp_registration_payload.
 */
#define BUFFER_FOR_DPS_PNP_REGISTRATION_PAYLOAD() BUFFER_WITH_FIXED_LENGTH(AZURE_DPS_SIZEOF_PNP_REGISTRATION_PAYLOAD)

    /**
     * @brief Initialize the Azure IoT Hub Client with a given device id, on the server
     * configured via menuconfig.
     * @note The hostname MUST BE configured in @ref CONFIG_ESP32_IOT_AZURE_DPS_SERVER_HOSTNAME.
     * @note The scope id MUST BE configured in @ref CONFIG_ESP32_IOT_AZURE_DPS_SCOPE_ID.
     * @param[in] context DPS context.
     * @param[in] registration_id Device registration id.
     * @param[in] registration_id_length Device registration id length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_init_default(azure_dps_context_t *context,
                                            const uint8_t *registration_id,
                                            uint32_t registration_id_length);

    /**
     * @brief Create an Azure IoT Plug and Play registration payload, with just the device model id, that can be passed
     * to @ref azure_dps_set_registration_payload function.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-develop/concepts-developer-guide-device?pivots=programming-language-ansi-c#dps-payload
     * @param[in,out] registration_payload Registration payload with a buffer with at least AZURE_DPS_SIZEOF_PNP_REGISTRATION_PAYLOAD.
     * @param[in,out] registration_payload_length Registration payload length. Will be updated with the created registration payload length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_create_pnp_registration_payload(uint8_t *registration_payload,
                                                               uint32_t *registration_payload_length);

#ifdef __cplusplus
}
#endif
#endif