#include <string.h>
#include "esp32_iot_azure/azure_iot_adu.h"
#include "config.h"
#include "assertion.h"
#include "log.h"

static const char TAG_AZ_ADU[] = "AZ_ADU";

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
    AzureIoTResult_t result = AzureIoTADUClient_Init(&context->adu_client,
                                                     &context->adu_client_options);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU, "failure initializing: %d", result);
    }

    return result;
}

AzureIoTResult_t azure_adu_device_properties_init(AzureIoTADUClientDeviceProperties_t *device_properties)
{
    AzureIoTResult_t result = AzureIoTADUClient_DevicePropertiesInit(device_properties);

    device_properties->ucManufacturer = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER;
    device_properties->ulManufacturerLength = sizeof(CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER) - 1;
    device_properties->ucModel = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL;
    device_properties->ulModelLength = sizeof(CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL) - 1;

    return result;
}

bool azure_adu_is_adu_component(azure_adu_context_t *context, const utf8_string_t *component_name)
{
    return AzureIoTADUClient_IsADUComponent(&context->adu_client,
                                            component_name->buffer,
                                            component_name->length);
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
                                         utf8_string_t *response_buffer,
                                         uint32_t *request_id)
{
    return AzureIoTADUClient_SendResponse(&adu_context->adu_client,
                                          azure_iot_hub_get_context_client(adu_context->iot_context),
                                          request_decision,
                                          property_version,
                                          response_buffer->buffer,
                                          response_buffer->length,
                                          request_id);
}

AzureIoTResult_t azure_adu_send_agent_state(azure_adu_context_t *adu_context,
                                            AzureIoTADUClientDeviceProperties_t *device_properties,
                                            AzureIoTADUUpdateRequest_t *update_request,
                                            AzureIoTADUAgentState_t agent_state,
                                            AzureIoTADUClientInstallResult_t *update_results,
                                            utf8_string_t *buffer,
                                            uint32_t *request_id)
{
    return AzureIoTADUClient_SendAgentState(&adu_context->adu_client,
                                            azure_iot_hub_get_context_client(adu_context->iot_context),
                                            device_properties,
                                            update_request,
                                            agent_state,
                                            update_results,
                                            buffer->buffer,
                                            buffer->length,
                                            request_id);
}

void azure_adu_free(azure_adu_context_t *context)
{
    free(context);
}