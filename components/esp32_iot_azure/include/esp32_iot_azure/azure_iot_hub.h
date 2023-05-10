#ifndef __ESP32_IOT_AZURE_IOT_HUB_H__
#define __ESP32_IOT_AZURE_IOT_HUB_H__

#include <stdint.h>
#include "esp32_iot_azure/azure_iot_common.h"
#include "azure_iot_hub_client.h"
#include "azure_iot_hub_client_properties.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef azure_iot_hub_context_t
     * @brief Azure IoT Hub Client context.
     */
    typedef struct azure_iot_hub_context_t azure_iot_hub_context_t;

    /**
     * @brief Create an Azure IoT Hub Client context.
     * @note The context must be released by @ref azure_iot_hub_free.
     * @return @ref azure_iot_hub_context_t on success or null on failure.
     */
    azure_iot_hub_context_t *azure_iot_hub_create();

    /**
     * @brief Get the @ref AzureIoTHubClient_t used by the \p context
     * @param[in] context IoT context.
     * @return Pointer to @ref AzureIoTHubClient_t.
     */
    AzureIoTHubClient_t *azure_iot_hub_get_context_client(azure_iot_hub_context_t *context);

    /**
     * @brief Initialize the \p context internal Azure IoT Hub Options.
     * @note The following fields will be filled with values from configuration: pucUserAgent, ulUserAgentLength, pucModelID, ulModelIDLength.
     * @param[in] context IoT context.
     * @param[out] client_options Pointer to a @ref AzureIoTHubClientOptions_t pointer used by the \p context.
     * Can be null if there is no need to change its fields.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_options_init(azure_iot_hub_context_t *context,
                                                AzureIoTHubClientOptions_t **client_options);

    /**
     * @brief Initialize the Azure IoT Hub Client with a given device id on a
     * given hostname.
     * @param[in] context IoT context.
     * @param[in] hostname IoTHub hostname.
     * @param[in] hostname_length IoTHub hostname length.
     * @param[in] device_id Device id.
     * @param[in] device_id_length Device id length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_init(azure_iot_hub_context_t *context,
                                        const uint8_t *hostname,
                                        uint16_t hostname_length,
                                        const uint8_t *device_id,
                                        uint16_t device_id_length);

    /**
     * @brief Set the symmetric key to use for authentication.
     * @param[in] context IoT context.
     * @param[in] symmetric_key Device symmetric key.
     * @param[in] symmetric_key_length Device symmetric key length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_auth_set_symmetric_key(azure_iot_hub_context_t *context,
                                                          const uint8_t *symmetric_key,
                                                          uint32_t symmetric_key_length);

    /**
     * @brief Set the client certificate to use for authentication.
     * @param[in] context DPS context.
     * @param[in] certificate Device client certificate.
     */
    void azure_iot_hub_auth_set_client_certificate(azure_iot_hub_context_t *context,
                                                   const client_certificate_t *certificate);

    /**
     * @brief Connect via MQTT to the IoT Hub endpoint.
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_connect(azure_iot_hub_context_t *context);

    /**
     * @brief Disconnect from the IoT Hub endpoint.
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_disconnect(azure_iot_hub_context_t *context);

    /**
     * @brief Subscribe to cloud to device messages.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-messages-c2d
     * @note Contract: https://learn.microsoft.com/en-us/azure/iot-central/core/concepts-telemetry-properties-commands#properties
     * @note Mqtt: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#receiving-cloud-to-device-messages
     * @param[in] context IoT context.
     * @param[in] callback Callback to invoke when CloudToDevice messages arrive.
     * @param[in] callback_context context to pass to the callback
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_subscribe_cloud_to_device_message(azure_iot_hub_context_t *context,
                                                                     AzureIoTHubClientCloudToDeviceMessageCallback_t callback,
                                                                     void *callback_context);

    /**
     * @brief Subscribe to Azure IoT Hub Direct Methods (commands) messages.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods
     * @note Contract: https://learn.microsoft.com/en-us/azure/iot-central/core/concepts-telemetry-properties-commands#commands
     * @note Mqtt: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-direct-methods#method-invocation-1
     * @param[in] context IoT context.
     * @param[in] callback Callback to invoke when Command messages arrive.
     * @param[in] callback_context context to pass to the callback
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_subscribe_command(azure_iot_hub_context_t *context,
                                                     AzureIoTHubClientCommandCallback_t callback,
                                                     void *callback_context);

    /**
     * @brief Subscribe to device twin's properties messages.
     * @note Contract: https://learn.microsoft.com/en-us/azure/iot-central/core/concepts-telemetry-properties-commands#properties
     * @note Mqtt: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#retrieving-a-device-twins-properties
     * @details
     *  - eAzureIoTHubPropertiesRequestedMessage: azure_iot_hub_request_properties_async response = property document (desired + reported).
     *  - eAzureIoTHubPropertiesReportedResponseMessage: azure_iot_hub_send_properties_reported response = properties value stored at the server.
     *  - eAzureIoTHubPropertiesWritablePropertyMessage: server want to change a device property.
     * @param[in] context IoT context.
     * @param[in] callback Callback to invoke when DeviceProperty messages arrive.
     * @param[in] callback_context context to pass to the callback
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_subscribe_properties(azure_iot_hub_context_t *context,
                                                        AzureIoTHubClientPropertiesCallback_t callback,
                                                        void *callback_context);

    /**
     * @brief Unsubscribe from cloud to device messages.
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_unsubscribe_cloud_to_device_message(azure_iot_hub_context_t *context);

    /**
     * @brief Unsubscribe Azure IoT Hub Direct Methods (commands) messages.
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_unsubscribe_command(azure_iot_hub_context_t *context);

    /**
     * @brief Unsubscribe from device twin's properties messages.
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_unsubscribe_properties(azure_iot_hub_context_t *context);

    /**
     * @brief Send a response to a received Direct Method (command) message.
     * @note Do not attempt to read \p command_request values after sending the response.
     * @note Mqtt: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#respond-to-a-direct-method
     * @param[in] context IoT context.
     * @param[in] command_request Command request to which a response is being sent.
     * @param[in] payload User defined response payload.
     * @param[in] payload_length Payload length.
     * @param[in] status_code User code that indicates the result of the command.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_send_command_response(azure_iot_hub_context_t *context,
                                                         const AzureIoTHubClientCommandRequest_t *command_request,
                                                         const uint8_t *payload,
                                                         uint32_t payload_length,
                                                         uint32_t status_code);

    /**
     * @brief Request to get the device property document, containing desired and reported properties.
     * @note @ref azure_iot_hub_subscribe_properties must be called before calling this function.
     * @note The response to the request will be returned via the @ref AzureIoTHubClientPropertiesCallback_t which was passed in the @ref azure_iot_hub_subscribe_properties call.
     * @note The type of message will be @ref eAzureIoTHubPropertiesRequestedMessage and the payload (on success) will be the property document.
     * @note Mqtt: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#retrieving-a-device-twins-properties
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_request_properties_async(azure_iot_hub_context_t *context);

    /**
     * @brief Send reported device properties to Azure IoT Hub.
     * @note Update device twin's reported properties.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-devguide-device-twins
     * @note Mqtt: https://learn.microsoft.com/en-us/azure/iot-hub/iot-hub-mqtt-support#update-device-twins-reported-properties
     * @note The response to the request will be returned via the @ref AzureIoTHubClientPropertiesCallback_t which was passed in the @ref azure_iot_hub_subscribe_properties call.
     * @note The type of message will be @ref eAzureIoTHubPropertiesReportedResponseMessage.
     * @param[in] context IoT context.
     * @param[in] payload Properly formatted, reported properties.
     * @param[in] payload_length Payload length.
     * @param[out] request_id Request ID used to send the reported property, generated by the IoT context. Can be `NULL`.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_send_properties_reported(azure_iot_hub_context_t *context,
                                                            const uint8_t *payload,
                                                            uint32_t payload_length,
                                                            uint32_t *request_id);

    /**
     * @brief Send telemetry data to IoT Hub.
     * @note Contract: https://learn.microsoft.com/en-us/azure/iot-central/core/concepts-telemetry-properties-commands#telemetry
     * @param[in] context IoT context.
     * @param[in] payload User defined telemetry payload.
     * @param[in] payload_length Payload length.
     * @param[in] properties Properties to send with the telemetry.
     * @param[in] qos The QOS to use for the telemetry
     * @param[out] packet_id Packet id for the sent telemetry, generated by the IoT context.
     * Can be notified of PUBACK for QOS 1 using the @ref AzureIoTHubClientOptions_t `xTelemetryCallback` option.
     * If \p qos is @ref eAzureIoTHubMessageQoS0 this value will not be sent on return. Can be `NULL`.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_send_telemetry(azure_iot_hub_context_t *context,
                                                  const uint8_t *payload,
                                                  uint32_t payload_length,
                                                  AzureIoTMessageProperties_t *properties,
                                                  AzureIoTHubMessageQoS_t qos,
                                                  uint16_t *packet_id);

    /**
     * @brief Receive any incoming MQTT messages from and manage the MQTT connection to IoT Hub.
     * @note This API will receive any messages sent to the device and manage the connection such as sending `PING` messages.
     * @param[in] context IoT context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_hub_process_loop(azure_iot_hub_context_t *context);

    /**
     * @brief Deinitialize the Azure IoT Hub Client.
     * @param[in] context IoT context.
     */
    void azure_iot_hub_deinit(azure_iot_hub_context_t *context);

    /**
     * @brief Cleanup and free the context.
     * @param[in] context DPS context.
     */
    void azure_iot_hub_free(azure_iot_hub_context_t *context);

#ifdef __cplusplus
}
#endif
#endif