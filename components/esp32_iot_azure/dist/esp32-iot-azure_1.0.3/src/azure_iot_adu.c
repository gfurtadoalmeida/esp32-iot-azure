#include <string.h>
#include "esp32_iot_azure/azure_iot_adu.h"
#include "config.h"

#define UPDATE_ID "{\"provider\":\"" CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_PROVIDER "\",\"name\":\"" CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME "\",\"version\":\"" CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION "\"}"

struct azure_adu_context_t
{
    AzureIoTADUClient_t adu_client;
    AzureIoTADUClientOptions_t adu_client_options;
    azure_iot_hub_context_t *iot_context;
};

azure_adu_context_t *azure_adu_create(azure_iot_hub_context_t *iot_context)
{
    azure_adu_context_t *context = (azure_adu_context_t *)malloc(sizeof(azure_adu_context_t));

    memset(context, 0, sizeof(azure_adu_context_t));

    context->iot_context = iot_context;

    return context;
}

azure_iot_hub_context_t *azure_adu_get_iot_hub_context(azure_adu_context_t *context)
{
    return context->iot_context;
}

AzureIoTResult_t azure_adu_options_init(azure_adu_context_t *context, AzureIoTADUClientOptions_t **client_options)
{
    AzureIoTResult_t result = AzureIoTADUClient_OptionsInit(&context->adu_client_options);

    if (client_options != NULL)
    {
        *client_options = &context->adu_client_options;
    }

    return result;
}

AzureIoTResult_t azure_adu_init(azure_adu_context_t *context)
{
    return AzureIoTADUClient_Init(&context->adu_client, &context->adu_client_options);
}

AzureIoTResult_t azure_adu_device_properties_init(AzureIoTADUClientDeviceProperties_t *device_properties)
{
    AzureIoTResult_t result = AzureIoTADUClient_DevicePropertiesInit(device_properties);

    device_properties->ucManufacturer = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER;
    device_properties->ulManufacturerLength = sizeof(CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER) - 1;
    device_properties->ucModel = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL;
    device_properties->ulModelLength = sizeof(CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL) - 1;
    device_properties->ucCurrentUpdateId = (const uint8_t *)UPDATE_ID;
    device_properties->ulCurrentUpdateIdLength = sizeof(UPDATE_ID) - 1;

    return result;
}

bool azure_adu_is_adu_component(azure_adu_context_t *context,
                                const uint8_t *component_name,
                                uint32_t component_name_length)
{
    return AzureIoTADUClient_IsADUComponent(&context->adu_client,
                                            component_name,
                                            component_name_length);
}

AzureIoTResult_t azure_adu_parse_request(azure_adu_context_t *context,
                                         AzureIoTJSONReader_t *json_reader,
                                         AzureIoTADUUpdateRequest_t *update_request)
{
    return AzureIoTADUClient_ParseRequest(&context->adu_client,
                                          json_reader,
                                          update_request);
}

AzureIoTResult_t azure_adu_send_response(azure_adu_context_t *adu_context,
                                         AzureIoTADURequestDecision_t request_decision,
                                         uint32_t property_version,
                                         uint8_t *response_buffer,
                                         uint32_t response_buffer_length,
                                         uint32_t *request_id)
{
    return AzureIoTADUClient_SendResponse(&adu_context->adu_client,
                                          azure_iot_hub_get_iot_client(adu_context->iot_context),
                                          request_decision,
                                          property_version,
                                          response_buffer,
                                          response_buffer_length,
                                          request_id);
}

AzureIoTResult_t azure_adu_send_agent_state(azure_adu_context_t *adu_context,
                                            AzureIoTADUClientDeviceProperties_t *device_properties,
                                            AzureIoTADUUpdateRequest_t *update_request,
                                            AzureIoTADUAgentState_t agent_state,
                                            AzureIoTADUClientInstallResult_t *update_results,
                                            uint8_t *buffer,
                                            uint32_t buffer_length,
                                            uint32_t *request_id)
{
    return AzureIoTADUClient_SendAgentState(&adu_context->adu_client,
                                            azure_iot_hub_get_iot_client(adu_context->iot_context),
                                            device_properties,
                                            update_request,
                                            agent_state,
                                            update_results,
                                            buffer,
                                            buffer_length,
                                            request_id);
}

void azure_adu_free(azure_adu_context_t *context)
{
    free(context);
}