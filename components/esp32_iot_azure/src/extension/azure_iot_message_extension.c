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
