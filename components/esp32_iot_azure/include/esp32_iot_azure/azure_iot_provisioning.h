#ifndef __ESP32_IOT_AZURE_PROVISIONING_H__
#define __ESP32_IOT_AZURE_PROVISIONING_H__

#include <stdint.h>
#include "esp32_iot_azure/azure_iot_common.h"
#include "azure_iot_provisioning_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef azure_dps_context_t
     * @brief Azure IoT Provisioning Client context.
     */
    typedef struct azure_dps_context_t azure_dps_context_t;

    /**
     * @brief Create an Azure IoT Provisioning Client context.
     * @note The context must be released by @ref azure_dps_free.
     * @param[in] mqtt_buffer Buffer for MQTT operations. Minimum of 2048 bytes are needed.
     * @return @ref azure_dps_context_t on success or null on failure.
     */
    azure_dps_context_t *azure_dps_create(buffer_t *mqtt_buffer);

    /**
     * @brief Initialize the \p context internal Azure IoT Provisioning Options.
     * @note The following fields will be filled with values from configuration: pucUserAgent, ulUserAgentLength.
     * @param[in] context DPS context.
     * @param[out] client_options Pointer to a @ref AzureIoTProvisioningClientOptions_t pointer used by the \p context.
     * Can be null if there is no need to change its fields.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_options_init(azure_dps_context_t *context, AzureIoTProvisioningClientOptions_t **client_options);

    /**
     * @brief Initialize the Azure IoT Provisioning Client with a
     * given registration id.
     * @param[in] context DPS context.
     * @param[in] hostname Hostname.
     * @param[in] hostname_length Hostname length.
     * @param[in] scope_id Scope id.
     * @param[in] scope_id_length Scope id length.
     * @param[in] registration_id Device registration id.
     * @param[in] registration_id_length Device registration id length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_init(azure_dps_context_t *context,
                                    const uint8_t *hostname,
                                    uint32_t hostname_length,
                                    const uint8_t *scope_id,
                                    uint32_t scope_id_length,
                                    const uint8_t *registration_id,
                                    uint32_t registration_id_length);

    /**
     * @brief Set the device derived or individual symmetric key to use for authentication.
     * @note Derived Key: https://learn.microsoft.com/en-us/azure/iot-dps/concepts-symmetric-key-attestation?tabs=windows#group-enrollments
     * @param[in] context DPS context.
     * @param[in] symmetric_key Device symmetric key.
     * @param[in] symmetric_key_length Device symmetric key, length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_auth_set_symmetric_key(azure_dps_context_t *context,
                                                      const uint8_t *symmetric_key,
                                                      uint32_t symmetric_key_length);

    /**
     * @brief Set the client certificate to use for authentication.
     * @note Certificate Attestation: https://learn.microsoft.com/en-us/azure/iot-dps/concepts-x509-attestation
     * @param[in] context DPS context.
     * @param[in] certificate Device client certificate.
     */
    void azure_dps_auth_set_client_certificate(azure_dps_context_t *context,
                                               const client_certificate_t *certificate);

    /**
     * @brief Set registration payload. Used by custom allocation policies, IoT Plug and Play and IoT Central devices.
     * @note Must be called before @ref azure_dps_register.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-dps/how-to-send-additional-data#when-to-use-it
     * @param[in] context DPS context.
     * @param[in] payload User defined JSON registration payload.
     * @param[in] payload_length Payload length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_set_registration_payload(azure_dps_context_t *context,
                                                        const uint8_t *payload,
                                                        uint32_t payload_length);

    /**
     * @brief Begin the provisioning process and wait until the device is registered or fail.
     * @note This method only returns after a device is registered of fails; there is
     * no need to check for @ref eAzureIoTErrorPending.
     * @note IoT Plug and Play (PnP) devices may use the payload to send their model
     * ID when they register with DPS.
     * @note IoT Central devices that connect through DPS should follow IoT Plug
     * and Play conventions and send their model ID when they register.
     * @param[in] context DPS context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_register(azure_dps_context_t *context);

    /**
     * @brief Get the IoT Hub hostname and device Id, after a
     * registration has been completed.
     * @param[in] context DPS context.
     * @param[out] hostname IoT Hub hostname buffer with capacity for @ref AZURE_CONST_HOSTNAME_MAX_LENGTH chars.
     * @param[in,out] hostname_length Hostname buffer length. Will be updated with the retrieved hostname length.
     * @param[out] device_id Device Id buffer with capacity for @ref AZURE_CONST_DEVICE_ID_MAX_LENGTH chars.
     * @param[in,out] device_id_length Device Id buffer length. Will be updated with the retrieved device id length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_get_device_and_hub(azure_dps_context_t *context,
                                                  uint8_t *hostname,
                                                  uint32_t *hostname_length,
                                                  uint8_t *device_id,
                                                  uint32_t *device_id_length);

    /**
     * @brief Get extended code for Provisioning failure.
     * @note Extended code is 6 digit error code last returned via the Provisioning service, when registration is done.
     * @param[in] context DPS context.
     * @param[out] extended_code Pointer to the @ref uint32_t which will be populated with the extended code.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_dps_get_extended_code(azure_dps_context_t *context, uint32_t *extended_code);

    /**
     * @brief Deinitialize the Azure IoT Provisioning Client.
     * @param[in] context DPS context.
     */
    void azure_dps_deinit(azure_dps_context_t *context);

    /**
     * @brief Cleanup and free the context.
     * @param[in] context DPS context.
     */
    void azure_dps_free(azure_dps_context_t *context);

#ifdef __cplusplus
}
#endif
#endif