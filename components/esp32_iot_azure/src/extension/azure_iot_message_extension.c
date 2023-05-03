#include "esp32_iot_azure/extension/azure_iot_message_extension.h"

AzureIoTResult_t AzureIoTMessage_PropertiesAppendComponentName(AzureIoTMessageProperties_t *message_properties,
                                                               const utf8_string_t *component_name)
{
    return AzureIoTMessage_PropertiesAppend(message_properties,
                                            (uint8_t *)"$.sub",
                                            sizeof("$.sub") - 1,
                                            component_name->buffer,
                                            component_name->length);
}
