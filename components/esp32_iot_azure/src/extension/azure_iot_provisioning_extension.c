#include "esp32_iot_azure/extension/azure_iot_provisioning_extension.h"
#include "assertion.h"
#include "config.h"

static const char TAG_AZ_DPS_EXT[] = "AZ_DPS_EXT";

AzureIoTResult_t azure_dps_create_pnp_registration_payload(utf8_string_t *registration_payload)
{
    CMP_CHECK(TAG_AZ_DPS_EXT,
              registration_payload->length >= AZURE_DPS_BASIC_REGISTRATION_PAYLOAD_NEEDED_BYTES,
              "small registration_payload buffer",
              eAzureIoTErrorOutOfMemory)

    AZ_CHECK_BEGIN()

    AzureIoTJSONWriter_t json_writer;

    AZ_CHECK(AzureIoTJSONWriter_Init(&json_writer, registration_payload->buffer, registration_payload->length))
    AZ_CHECK(AzureIoTJSONWriter_AppendBeginObject(&json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
                                                              (uint8_t *)"modelId",
                                                              sizeof("modelId") - 1,
                                                              (uint8_t *)CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID,
                                                              sizeof(CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID) - 1))
    AZ_CHECK(AzureIoTJSONWriter_AppendEndObject(&json_writer))

    registration_payload->length = (uint32_t)AzureIoTJSONWriter_GetBytesUsed(&json_writer);

    AZ_CHECK_RETURN_LAST()
}