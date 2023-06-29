#include "esp32_iot_azure/extension/azure_iot_provisioning_extension.h"
#include "assertion.h"
#include "config.h"

static const char TAG_AZ_DPS_EXT[] = "AZ_DPS_EXT";

AzureIoTResult_t azure_dps_init_default(azure_dps_context_t *context,
                                        const uint8_t *registration_id,
                                        uint32_t registration_id_length)
{
    return azure_dps_init(context,
                          (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DPS_SERVER_HOSTNAME,
                          sizeof(CONFIG_ESP32_IOT_AZURE_DPS_SERVER_HOSTNAME) - 1,
                          (const uint8_t *)CONFIG_ESP32_IOT_AZURE_DPS_SCOPE_ID,
                          sizeof(CONFIG_ESP32_IOT_AZURE_DPS_SCOPE_ID) - 1,
                          registration_id,
                          registration_id_length);
}

AzureIoTResult_t azure_dps_create_pnp_registration_payload(uint8_t *registration_payload,
                                                           uint32_t *registration_payload_length)
{
    CMP_CHECK(TAG_AZ_DPS_EXT,
              *registration_payload_length >= AZURE_DPS_SIZEOF_PNP_REGISTRATION_PAYLOAD,
              "small registration_payload buffer",
              eAzureIoTErrorOutOfMemory)

    AZ_CHECK_BEGIN()

    AzureIoTJSONWriter_t json_writer;

    AZ_CHECK(AzureIoTJSONWriter_Init(&json_writer, registration_payload, *registration_payload_length))
    AZ_CHECK(AzureIoTJSONWriter_AppendBeginObject(&json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
                                                              (uint8_t *)"modelId",
                                                              sizeof("modelId") - 1,
                                                              (uint8_t *)CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID,
                                                              sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID) - 1))
    AZ_CHECK(AzureIoTJSONWriter_AppendEndObject(&json_writer))

    *registration_payload_length = (uint32_t)AzureIoTJSONWriter_GetBytesUsed(&json_writer);

    AZ_CHECK_RETURN_LAST()
}