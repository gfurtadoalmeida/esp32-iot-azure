#include "example_provisioning.h"
#include "esp32_iot_azure/azure_iot_provisioning.h"
#include "esp32_iot_azure/extension/azure_iot_provisioning_extension.h"

static const char TAG_EX_DPS[] = "EX_DPS";

bool example_dps_run(const utf8_string_t *device_symmetric_key,
                     const utf8_string_t *device_registration_id,
                     utf8_string_t *iot_hub_hostname,
                     utf8_string_t *device_id)
{
    bool success = false;

    AzureIoTProvisioningClientOptions_t *dps_client_options = NULL;
    utf8_string_t registration_payload = UTF8_STRING_FOR_DPS_PNP_REGISTRATION_PAYLOAD();

    azure_dps_context_t *dps = azure_dps_create();

    if (azure_dps_options_init(dps, &dps_client_options) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure initializing options");
        goto CLEAN_UP;
    }

    if (azure_dps_init(dps, device_registration_id) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure initializing");
        goto CLEAN_UP;
    }

    if (azure_dps_auth_set_symmetric_key(dps, device_symmetric_key) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure setting symmetric key");
        goto CLEAN_UP;
    }

    if (azure_dps_create_pnp_registration_payload(&registration_payload) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure creating PnP registration payload");
        goto CLEAN_UP;
    }

    if (azure_dps_set_registration_payload(dps, &registration_payload) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure setting registration payload");
        goto CLEAN_UP;
    }

    if (azure_dps_register(dps) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure registering");
        goto CLEAN_UP;
    }

    if (azure_dps_get_device_and_hub(dps, iot_hub_hostname, device_id) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_DPS, "failure getting device and hub");
        goto CLEAN_UP;
    }

    success = true;

CLEAN_UP:
    azure_dps_deinit(dps);
    azure_dps_free(dps);

    ESP_LOGI(TAG_EX_DPS, "hostname: %.*s", iot_hub_hostname->length, (char *)iot_hub_hostname->buffer);
    ESP_LOGI(TAG_EX_DPS, "device_id: %.*s", device_id->length, (char *)device_id->buffer);

    return success;
}