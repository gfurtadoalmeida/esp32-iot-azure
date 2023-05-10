#ifndef __ESP32_IOT_AZURE_ADU_H__
#define __ESP32_IOT_AZURE_ADU_H__

#include <stdint.h>
#include "esp32_iot_azure/azure_iot_hub.h"
#include "azure_iot_adu_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef azure_adu_context_t
     * @brief Azure IoT Device Update Client context.
     */
    typedef struct azure_adu_context_t azure_adu_context_t;

    /**
     * @brief Create an Azure IoT Device Update Client context.
     * @note The context must be released by @ref azure_adu_free.
     * @param[in] iot_context IoT Hub Context to which this ADU will be linked.
     * @return @ref azure_dps_context_t on success or null on failure.
     */
    azure_adu_context_t *azure_adu_create(azure_iot_hub_context_t *iot_context);

    /**
     * @brief Initialize the \p context internal Azure IoT Device Update Options.
     * @param[in] context ADU context.
     * @param[out] client_options Pointer to a @ref AzureIoTADUClientOptions_t pointer used by the \p context.
     * Can be `NULL` if there is no need to change its fields.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_options_init(azure_adu_context_t *context, AzureIoTADUClientOptions_t **client_options);

    /**
     * @brief Initialize the Azure IoT Device Update Client.
     * @param[in] context ADU context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_init(azure_adu_context_t *context);

    /**
     * @brief Initialize the Azure IoT Device Properties with default values.
     * @note The following fields will be filled with values from configuration:
     * ucManufacturer, ulManufacturerLength, ucModel, ulModelLength, ucCurrentUpdateId, ucCurrentUpdateLength.
     * @note About: https://learn.microsoft.com/en-us/azure/iot-hub-device-update/import-concepts#update-identity
     * @note Contract: https://learn.microsoft.com/en-us/azure/iot-hub-device-update/import-schema#updateid-object
     * @param[out] device_properties The @ref AzureIoTADUClientDeviceProperties_t instance to set with default values.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_device_properties_init(AzureIoTADUClientDeviceProperties_t *device_properties);

    /**
     * @brief Returns whether the component is the ADU component.
     * @note If it is, user should follow by parsing the component with the
     *       @ref AzureIoTHubClient_ADUProcessComponent() call. The properties will be
     *       processed into the AzureIoTADUClient.
     * @param[in] context ADU context.
     * @param[in] component_name Name of writable property component to be checked.
     * @param[in] component_name_length Name of writable property component length.
     * @return A boolean value indicating if the writable properties component
     *         is from ADU service.
     */
    bool azure_adu_is_adu_component(azure_adu_context_t *context,
                                    const uint8_t *component_name,
                                    uint32_t component_name_length);

    /**
     * @brief Parse the ADU update request into the requisite structure.
     * @note The JSON reader returned to the caller from @ref AzureIoTHubClientProperties_GetNextComponentProperty()
     * should be passed to this API.
     * @param[in] context ADU context.
     * @param[in,out] json_reader The initialized JSON reader positioned at the beginning of the ADU subcomponent property.
     * @param[out] update_request The @ref AzureIoTADUUpdateRequest_t into which the properties will be parsed.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_parse_request(azure_adu_context_t *context,
                                             AzureIoTJSONReader_t *json_reader,
                                             AzureIoTADUUpdateRequest_t *update_request);

    /**
     * @brief Updates the ADU Agent Client with ADU service device update properties.
     * @remark It must be called whenever writable properties are received containing
     *         ADU service properties (verified with @ref azure_adu_is_adu_component()).
     *         It effectively parses the properties (aka, the device update request)
     *         from ADU and sets the state machine to perform the update process if the
     *         the update request is applicable (e.g., if the version is not already
     *         installed).
     *         This function also provides the payload to acknowledge the ADU service
     *         Azure Plug-and-Play writable properties.
     * @param[in] adu_context ADU context.
     * @param[in] request_decision The @ref #AzureIoTADURequestDecision_t for this response.
     * @param[in] property_version Version of the writable properties.
     * @param[in] response_buffer A pointer to the memory buffer where to write
     * the resulting Azure Plug-and-Play properties acknowledgement payload.
     * @param[in] response_buffer_length Response buffer length.
     * @param[in] request_id Pointer to request id to use for the operation.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_send_response(azure_adu_context_t *adu_context,
                                             AzureIoTADURequestDecision_t request_decision,
                                             uint32_t property_version,
                                             uint8_t *response_buffer,
                                             uint32_t response_buffer_length,
                                             uint32_t *request_id);

    /**
     * @brief Sends the current state of the Azure IoT ADU agent.
     * @param[in] adu_context ADU context.
     * @param[in] device_properties The device information which will be used to generate the payload.
     * @param[in] update_request The current @ref AzureIoTADUUpdateRequest_t. This can be `NULL` if there isn't currently
     * an update request.
     * @param[in] agent_state The current @ref AzureIoTADUAgentState_t.
     * @param[in] update_results The current @ref AzureIoTADUClientInstallResult_t. This can be `NULL` if there aren't any
     * results from an update.
     * @param[out] buffer The buffer into which the generated payload will be placed.
     * @param[out] buffer_length Buffer length.
     * @param[in] request_id Pointer to request id to use for the operation.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_send_agent_state(azure_adu_context_t *adu_context,
                                                AzureIoTADUClientDeviceProperties_t *device_properties,
                                                AzureIoTADUUpdateRequest_t *update_request,
                                                AzureIoTADUAgentState_t agent_state,
                                                AzureIoTADUClientInstallResult_t *update_results,
                                                uint8_t *buffer,
                                                uint32_t buffer_length,
                                                uint32_t *request_id);

    /**
     * @brief Cleanup and free the context.
     * @note Will not free the @ref azure_iot_hub_context_t passed on @ref azure_adu_create().
     * @param[in] context ADU context.
     */
    void azure_adu_free(azure_adu_context_t *context);

#ifdef __cplusplus
}
#endif
#endif