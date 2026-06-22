#include "example_provisioning.h"
#include "esp32_iot_azure/azure_iot_provisioning.h"
#include "esp32_iot_azure/extension/azure_iot_provisioning_extension.h"

static const char TAG_EX_DPS[] = "EX_DPS";

static bool example_dps_register_and_get(azure_dps_context_t *dps,
                                         const buffer_t *device_symmetric_key,
                                         const buffer_t *device_registration_id,
                                         buffer_t *iot_hub_hostname,
                                         buffer_t *device_id)
{
    AzureIoTProvisioningClientOptions_t *dps_client_options = NULL;
    buffer_t registration_payload = BUFFER_FOR_DPS_PNP_REGISTRATION_PAYLOAD();

    if (azure_dps_options_init(dps, &dps_client_options) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure initializing options");
        return false;
    }

    if (azure_dps_init_default(dps, device_registration_id->buffer, device_registration_id->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure initializing");
        return false;
    }

    if (azure_dps_auth_set_symmetric_key(dps, device_symmetric_key->buffer, device_symmetric_key->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure setting symmetric key");
        return false;
    }

    if (azure_dps_create_pnp_registration_payload(registration_payload.buffer, &registration_payload.length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure creating PnP registration payload");
        return false;
    }

    if (azure_dps_set_registration_payload(dps, registration_payload.buffer, registration_payload.length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure setting registration payload");
        return false;
    }

    if (azure_dps_register(dps) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure registering");
        return false;
    }

    if (azure_dps_get_device_and_hub(dps,
                                     iot_hub_hostname->buffer,
                                     &iot_hub_hostname->length,
                                     device_id->buffer,
                                     &device_id->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure getting device and hub");
        return false;
    }

    return true;
}

bool example_dps_run(const buffer_t *device_symmetric_key,
                     const buffer_t *device_registration_id,
                     buffer_t *iot_hub_hostname,
                     buffer_t *device_id)
{
    buffer_t buffer = {
        .length = 2048,
        .buffer = (uint8_t *)malloc(2048)};

    azure_dps_context_t *dps = azure_dps_create(&buffer);

    bool success = example_dps_register_and_get(dps,
                                                device_symmetric_key,
                                                device_registration_id,
                                                iot_hub_hostname,
                                                device_id);

    azure_dps_deinit(dps);
    azure_dps_free(dps);

    free(buffer.buffer);

    ESP_LOGI(TAG_EX_DPS, "hostname: %.*s", (int)iot_hub_hostname->length, (char *)iot_hub_hostname->buffer);
    ESP_LOGI(TAG_EX_DPS, "device_id: %.*s", (int)device_id->length, (char *)device_id->buffer);

    return success;
}