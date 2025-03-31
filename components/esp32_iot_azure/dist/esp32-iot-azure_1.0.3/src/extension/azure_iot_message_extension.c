#include "esp32_iot_azure/extension/azure_iot_message_extension.h"

AzureIoTResult_t AzureIoTMessage_PropertiesAppendComponentName(AzureIoTMessageProperties_t *message_properties,
                                                               const uint8_t *component_name,
                                                               uint32_t component_name_length)
{
    return AzureIoTMessage_PropertiesAppend(message_properties,
                                            (uint8_t *)"$.sub",
                                            sizeof("$.sub") - 1,
                                            component_name,
                                            component_name_length);
}

AzureIoTResult_t AzureIoTMessage_PropertiesAppendContentType(AzureIoTMessageProperties_t *message_properties,
                                                             const uint8_t *content_type,
                                                             uint32_t content_type_length)
{
    return AzureIoTMessage_PropertiesAppend(message_properties,
                                            (uint8_t *)"$.ct",
                                            sizeof("$.ct") - 1,
                                            content_type,
                                            content_type_length);
}

AzureIoTResult_t AzureIoTMessage_PropertiesAppendContentEncoding(AzureIoTMessageProperties_t *message_properties,
                                                                 const uint8_t *content_encoding,
                                                                 uint32_t content_encoding_length)
{
    return AzureIoTMessage_PropertiesAppend(message_properties,
                                            (uint8_t *)"$.ce",
                                            sizeof("$.ce") - 1,
                                            content_encoding,
                                            content_encoding_length);
}
