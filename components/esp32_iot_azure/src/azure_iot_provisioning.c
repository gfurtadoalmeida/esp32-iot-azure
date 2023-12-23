#include <string.h>
#include <stdint.h>
#include "esp32_iot_azure/azure_iot_provisioning.h"
#include "infrastructure/time.h"
#include "infrastructure/crypto.h"
#include "infrastructure/transport.h"
#include "infrastructure/azure_transport_interface.h"
#include "config.h"
#include "assertion.h"
#include "log.h"

static const char TAG_AZ_DPS[] = "AZ_DPS";

struct azure_dps_context_t
{
    AzureIoTProvisioningClient_t dps_client;
    AzureIoTTransportInterface_t transport_interface;
    AzureIoTProvisioningClientOptions_t dps_client_options;
    transport_t *transport;
    buffer_t *mqtt_buffer;
};

azure_dps_context_t *azure_dps_create(buffer_t *mqtt_buffer)
{
    if (mqtt_buffer == NULL || mqtt_buffer->buffer == NULL)
    {
        CMP_LOGE(TAG_AZ_DPS, "mqtt_buffer null");
        return NULL;
    }

    azure_dps_context_t *context = (azure_dps_context_t *)malloc(sizeof(azure_dps_context_t));

    memset(context, 0, sizeof(azure_dps_context_t));

    context->transport = transport_create_azure();
    context->mqtt_buffer = mqtt_buffer;

    return context;
}

AzureIoTResult_t azure_dps_options_init(azure_dps_context_t *context, AzureIoTProvisioningClientOptions_t **client_options)
{
    AzureIoTResult_t result = AzureIoTProvisioningClient_OptionsInit(&context->dps_client_options);

    context->dps_client_options.pucUserAgent = (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DEVICE_USER_AGENT;
    context->dps_client_options.ulUserAgentLength = sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_USER_AGENT) - 1;

    if (client_options != NULL)
    {
        *client_options = &context->dps_client_options;
    }

    return result;
}

AzureIoTResult_t azure_dps_init(azure_dps_context_t *context,
                                const uint8_t *hostname,
                                uint32_t hostname_length,
                                const uint8_t *scope_id,
                                uint32_t scope_id_length,
                                const uint8_t *registration_id,
                                uint32_t registration_id_length)
{
    azure_transport_interface_init(context->transport, &context->transport_interface);

    return AzureIoTProvisioningClient_Init(&context->dps_client,
                                           hostname,
                                           hostname_length,
                                           scope_id,
                                           scope_id_length,
                                           registration_id,
                                           registration_id_length,
                                           &context->dps_client_options,
                                           context->mqtt_buffer->buffer,
                                           context->mqtt_buffer->length,
                                           &time_get_unix,
                                           &context->transport_interface);
}

AzureIoTResult_t azure_dps_auth_set_symmetric_key(azure_dps_context_t *context,
                                                  const uint8_t *symmetric_key,
                                                  uint32_t symmetric_key_length)
{
    return AzureIoTProvisioningClient_SetSymmetricKey(&context->dps_client,
                                                      symmetric_key,
                                                      symmetric_key_length,
                                                      &crypto_hash_hmac_256);
}

void azure_dps_auth_set_client_certificate(azure_dps_context_t *context,
                                           const client_certificate_t *certificate)
{
    transport_set_client_certificate(context->transport, certificate);
}

AzureIoTResult_t azure_dps_set_registration_payload(azure_dps_context_t *context,
                                                    const uint8_t *payload,
                                                    uint32_t payload_length)
{
    return AzureIoTProvisioningClient_SetRegistrationPayload(&context->dps_client,
                                                             payload,
                                                             payload_length);
}

AzureIoTResult_t azure_dps_register(azure_dps_context_t *context)
{
    if (transport_connect(context->transport,
                          (const char *)context->dps_client._internal.pucEndpoint,
                          CONFIG_ESP32_IOT_AZURE_HUB_SERVER_PORT,
                          CONFIG_ESP32_IOT_AZURE_DPS_CONNECT_TIMEOUT_MS) != TRANSPORT_STATUS_SUCCESS)
    {
        CMP_LOGE(TAG_AZ_DPS, "failure connecting transport");
        return eAzureIoTErrorFailed;
    }

    AzureIoTResult_t result = eAzureIoTErrorFailed;

    do
    {
        result = AzureIoTProvisioningClient_Register(&context->dps_client,
                                                     CONFIG_ESP32_IOT_AZURE_DPS_REGISTRATION_TIMEOUT_MS);
    } while (result == eAzureIoTErrorPending);

    transport_disconnect(context->transport);

    return result;
}

AzureIoTResult_t azure_dps_get_device_and_hub(azure_dps_context_t *context,
                                              uint8_t *hostname,
                                              uint32_t *hostname_length,
                                              uint8_t *device_id,
                                              uint32_t *device_id_length)
{
    CMP_CHECK(TAG_AZ_DPS, (*hostname_length >= AZURE_CONST_HOSTNAME_MAX_LENGTH), "small hostname buffer", eAzureIoTErrorOutOfMemory)
    CMP_CHECK(TAG_AZ_DPS, (*device_id_length >= AZURE_CONST_DEVICE_ID_MAX_LENGTH), "small device_id buffer", eAzureIoTErrorOutOfMemory)

    return AzureIoTProvisioningClient_GetDeviceAndHub(&context->dps_client,
                                                      hostname,
                                                      hostname_length,
                                                      device_id,
                                                      device_id_length);
}

AzureIoTResult_t azure_dps_get_extended_code(azure_dps_context_t *context, uint32_t *extended_code)
{
    return AzureIoTProvisioningClient_GetExtendedCode(&context->dps_client, extended_code);
}

void azure_dps_deinit(azure_dps_context_t *context)
{
    AzureIoTProvisioningClient_Deinit(&context->dps_client);
}

void azure_dps_free(azure_dps_context_t *context)
{
    azure_transport_interface_free(&context->transport_interface);

    transport_free(context->transport);

    free(context);
}