#include "esp32_iot_azure/extension/azure_iot_hub_extension.h"
#include "esp32_iot_azure/extension/azure_iot_message_extension.h"
#include "config.h"

static const utf8_string_t DEFAULT_HOSTNAME = UTF8_STRING_FROM_LITERAL(CONFIG_ESP32_IOT_AZURE_HUB_SERVER_HOSTNAME);

AzureIoTResult_t azure_iot_hub_init_default(azure_iot_hub_context_t *context,
                                            const utf8_string_t *device_id)
{
    return azure_iot_hub_init(context, &DEFAULT_HOSTNAME, device_id);
}

AzureIoTResult_t azure_iot_hub_send_telemetry_from_component(azure_iot_hub_context_t *context,
                                                             const utf8_string_t *component_name,
                                                             const utf8_string_t *payload,
                                                             AzureIoTHubMessageQoS_t qos,
                                                             uint16_t *packet_id)
{
    // Message format: /?prop=value
    // 5 -> $.sub
    // 1 -> =
    // There's no need to count for -> /?

    uint8_t properties_buffer[AZURE_CONST_COMPONENT_NAME_MAX_LENGTH + 5 + 1];
    AzureIoTMessageProperties_t properties_message;

    AZ_CHECK_BEGIN()

    AZ_CHECK(AzureIoTMessage_PropertiesInit(&properties_message, properties_buffer, 0, sizeof(properties_buffer)))
    AZ_CHECK(AzureIoTMessage_PropertiesAppendComponentName(&properties_message, component_name))

    AZ_CHECK(azure_iot_hub_send_telemetry(context, payload, &properties_message, qos, packet_id))

    AZ_CHECK_RETURN_LAST()
}
