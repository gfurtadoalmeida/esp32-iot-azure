#include <string.h>
#include "esp32_iot_azure/azure_iot_hub.h"
#include "infrastructure/time.h"
#include "infrastructure/crypto.h"
#include "infrastructure/transport.h"
#include "infrastructure/azure_transport_interface.h"
#include "config.h"
#include "log.h"

static const char TAG_AZ_IOT[] = "AZ_IOT_HUB";

struct azure_iot_hub_context_t
{
    AzureIoTHubClient_t iot_client;
    AzureIoTTransportInterface_t transport_interface;
    AzureIoTHubClientOptions_t iot_client_options;
    esp_transport_handle_t transport;
    uint8_t *mqtt_buffer;
};

azure_iot_hub_context_t *azure_iot_hub_create()
{
    azure_iot_hub_context_t *context = (azure_iot_hub_context_t *)malloc(sizeof(azure_iot_hub_context_t));

    memset(context, 0, sizeof(azure_iot_hub_context_t));

    context->transport = transport_create_azure();
    context->mqtt_buffer = (uint8_t *)malloc(CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE);

    return context;
}

AzureIoTHubClient_t *azure_iot_hub_get_context_client(azure_iot_hub_context_t *context)
{
    return &context->iot_client;
}

AzureIoTResult_t azure_iot_hub_options_init(azure_iot_hub_context_t *context, AzureIoTHubClientOptions_t **client_options)
{
    AzureIoTResult_t result = AzureIoTHubClient_OptionsInit(&context->iot_client_options);

    context->iot_client_options.pucUserAgent = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DEVICE_USER_AGENT;
    context->iot_client_options.ulUserAgentLength = sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_USER_AGENT) - 1;
    context->iot_client_options.pucModelID = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID;
    context->iot_client_options.ulModelIDLength = sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID) - 1;

    if (client_options != NULL)
    {
        *client_options = &context->iot_client_options;
    }

    return result;
}

AzureIoTResult_t azure_iot_hub_init(azure_iot_hub_context_t *context,
                                    const utf8_string_t *hostname,
                                    const utf8_string_t *device_id)
{
    azure_transport_interface_init(context->transport, &context->transport_interface);

    AzureIoTResult_t result = AzureIoTHubClient_Init(&context->iot_client,
                                                     hostname->buffer,
                                                     (uint16_t)hostname->length,
                                                     device_id->buffer,
                                                     (uint16_t)device_id->length,
                                                     &context->iot_client_options,
                                                     context->mqtt_buffer,
                                                     CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE,
                                                     &time_get_unix,
                                                     &context->transport_interface);
    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_IOT, "failure initializing: %d", result);
    }

    return result;
}

AzureIoTResult_t azure_iot_hub_auth_set_symmetric_key(azure_iot_hub_context_t *context, const utf8_string_t *symmetric_key)
{
    AzureIoTResult_t result = AzureIoTHubClient_SetSymmetricKey(&context->iot_client,
                                                                symmetric_key->buffer,
                                                                symmetric_key->length,
                                                                &crypto_hmac_256);
    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_IOT, "failure setting symmetric key: %d", result);
    }

    return result;
}

void azure_iot_hub_auth_set_client_certificate(azure_iot_hub_context_t *context, const client_certificate_t *certificate)
{
    transport_set_client_certificate(context->transport, certificate);
}

AzureIoTResult_t azure_iot_hub_connect(azure_iot_hub_context_t *context)
{
    bool session_present = false;

    if (transport_connect(context->transport,
                          (const char *)context->iot_client._internal.pucHostname,
                          CONFIG_ESP32_IOT_AZURE_HUB_SERVER_PORT,
                          CONFIG_ESP32_IOT_AZURE_IOT_CONNECT_TIMEOUT_MS) != TRANSPORT_STATUS_SUCCESS)
    {
        CMP_LOGE(TAG_AZ_IOT, "failure connecting transport");
        return eAzureIoTErrorFailed;
    }

    AzureIoTResult_t result = AzureIoTHubClient_Connect(&context->iot_client,
                                                        false,
                                                        &session_present,
                                                        CONFIG_ESP32_IOT_AZURE_IOT_CONNECT_TIMEOUT_MS);
    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_IOT, "failure connecting to hub: %d", result);
    }

    return result;
}

AzureIoTResult_t azure_iot_hub_disconnect(azure_iot_hub_context_t *context)
{
    AzureIoTResult_t result = AzureIoTHubClient_Disconnect(&context->iot_client);

    transport_disconnect(context->transport);

    return result;
}

AzureIoTResult_t azure_iot_hub_subscribe_cloud_to_device_message(azure_iot_hub_context_t *context,
                                                                 AzureIoTHubClientCloudToDeviceMessageCallback_t callback,
                                                                 void *callback_context)
{
    return AzureIoTHubClient_SubscribeCloudToDeviceMessage(&context->iot_client,
                                                           callback,
                                                           callback_context,
                                                           CONFIG_ESP32_IOT_AZURE_HUB_SUBSCRIBE_TIMEOUT_MS);
}

AzureIoTResult_t azure_iot_hub_subscribe_command(azure_iot_hub_context_t *context,
                                                 AzureIoTHubClientCommandCallback_t callback,
                                                 void *callback_context)
{
    return AzureIoTHubClient_SubscribeCommand(&context->iot_client,
                                              callback,
                                              callback_context,
                                              CONFIG_ESP32_IOT_AZURE_HUB_SUBSCRIBE_TIMEOUT_MS);
}

AzureIoTResult_t azure_iot_hub_subscribe_properties(azure_iot_hub_context_t *context,
                                                    AzureIoTHubClientPropertiesCallback_t callback,
                                                    void *callback_context)
{
    return AzureIoTHubClient_SubscribeProperties(&context->iot_client,
                                                 callback,
                                                 callback_context,
                                                 CONFIG_ESP32_IOT_AZURE_HUB_SUBSCRIBE_TIMEOUT_MS);
}

AzureIoTResult_t azure_iot_hub_unsubscribe_cloud_to_device_message(azure_iot_hub_context_t *context)
{
    return AzureIoTHubClient_UnsubscribeCloudToDeviceMessage(&context->iot_client);
}
AzureIoTResult_t azure_iot_hub_unsubscribe_command(azure_iot_hub_context_t *context)
{
    return AzureIoTHubClient_UnsubscribeCommand(&context->iot_client);
}
AzureIoTResult_t azure_iot_hub_unsubscribe_properties(azure_iot_hub_context_t *context)
{
    return AzureIoTHubClient_UnsubscribeProperties(&context->iot_client);
}

AzureIoTResult_t azure_iot_hub_send_command_response(azure_iot_hub_context_t *context,
                                                     const AzureIoTHubClientCommandRequest_t *command_request,
                                                     const utf8_string_t *payload,
                                                     uint32_t status_code)
{
    if (payload == NULL)
    {
        return AzureIoTHubClient_SendCommandResponse(&context->iot_client,
                                                     command_request,
                                                     status_code,
                                                     NULL,
                                                     0U);
    }
    else
    {
        return AzureIoTHubClient_SendCommandResponse(&context->iot_client,
                                                     command_request,
                                                     status_code,
                                                     payload->buffer,
                                                     payload->length);
    }
}

AzureIoTResult_t azure_iot_hub_request_properties_async(azure_iot_hub_context_t *context)
{
    return AzureIoTHubClient_RequestPropertiesAsync(&context->iot_client);
}

AzureIoTResult_t azure_iot_hub_send_properties_reported(azure_iot_hub_context_t *context,
                                                        const utf8_string_t *payload,
                                                        uint32_t *request_id)
{
    return AzureIoTHubClient_SendPropertiesReported(&context->iot_client,
                                                    payload->buffer,
                                                    payload->length,
                                                    request_id);
}

AzureIoTResult_t azure_iot_hub_send_telemetry(azure_iot_hub_context_t *context,
                                              const utf8_string_t *payload,
                                              AzureIoTMessageProperties_t *properties,
                                              AzureIoTHubMessageQoS_t qos,
                                              uint16_t *packet_id)
{
    return AzureIoTHubClient_SendTelemetry(&context->iot_client,
                                           payload->buffer,
                                           payload->length,
                                           properties,
                                           qos,
                                           packet_id);
}

AzureIoTResult_t azure_iot_hub_process_loop(azure_iot_hub_context_t *context)
{
    return AzureIoTHubClient_ProcessLoop(&context->iot_client, CONFIG_ESP32_IOT_AZURE_HUB_LOOP_TIMEOUT_MS);
}

void azure_iot_hub_deinit(azure_iot_hub_context_t *context)
{
    AzureIoTHubClient_Deinit(&context->iot_client);
}

void azure_iot_hub_free(azure_iot_hub_context_t *context)
{
    azure_transport_interface_free(&context->transport_interface);

    transport_free(context->transport);

    free(context->mqtt_buffer);
    free(context);
}