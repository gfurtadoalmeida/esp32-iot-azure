#include "esp32_iot_azure/extension/azure_iot_json_reader_extension.h"
#include "esp32_iot_azure/azure_iot_common.h"

AzureIoTResult_t AzureIoTJSONReader_SkipPropertyAndValue(AzureIoTJSONReader_t *json_reader)
{
    AZ_CHECK_BEGIN()

    AZ_CHECK(AzureIoTJSONReader_NextToken(json_reader))
    AZ_CHECK(AzureIoTJSONReader_SkipChildren(json_reader))
    AZ_CHECK(AzureIoTJSONReader_NextToken(json_reader))

    AZ_CHECK_RETURN_LAST()
}

AzureIoTResult_t AzureIoTJSONReader_GetTokenUInt16(AzureIoTJSONReader_t *json_reader, uint16_t *value)
{
    int32_t int32_value = 0;

    AZ_CHECK_BEGIN()
    AZ_CHECK(AzureIoTJSONReader_GetTokenInt32(json_reader, &int32_value))

    *value = (uint16_t)int32_value;

    AZ_CHECK_RETURN_LAST()
}

AzureIoTResult_t AzureIoTJSONReader_GetTokenUInt8(AzureIoTJSONReader_t *json_reader, uint8_t *value)
{
    int32_t int32_value = 0;

    AZ_CHECK_BEGIN()
    AZ_CHECK(AzureIoTJSONReader_GetTokenInt32(json_reader, &int32_value))

    *value = (uint8_t)int32_value;

    AZ_CHECK_RETURN_LAST()
}
