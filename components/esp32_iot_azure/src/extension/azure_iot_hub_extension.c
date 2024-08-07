#include "esp32_iot_azure/extension/azure_iot_hub_extension.h"
#include "esp32_iot_azure/extension/azure_iot_message_extension.h"
#include "config.h"

AzureIoTResult_t azure_iot_hub_init_default(azure_iot_hub_context_t *context,
                                            const uint8_t *device_id,
                                            uint16_t device_id_length)
{
    return azure_iot_hub_init(context,
                              (const uint8_t *)CONFIG_ESP32_IOT_AZURE_HUB_SERVER_HOSTNAME,
                              sizeof(CONFIG_ESP32_IOT_AZURE_HUB_SERVER_HOSTNAME) - 1,
                              device_id,
                              device_id_length);
}

AzureIoTResult_t azure_iot_hub_send_telemetry_from_component(azure_iot_hub_context_t *context,
                                                             const uint8_t *component_name,
                                                             uint32_t component_name_length,
                                                             const uint8_t *payload,
                                                             uint32_t payload_length,
                                                             AzureIoTHubMessageQoS_t qos,
                                                             uint16_t *packet_id)
{
    // Message format: /?property=value
    // 6 -> $.sub=

    uint8_t properties_buffer[AZURE_CONST_COMPONENT_NAME_MAX_LENGTH + 6];
    AzureIoTMessageProperties_t properties_message;

    AZ_CHECK_BEGIN()

    AZ_CHECK(AzureIoTMessage_PropertiesInit(&properties_message, properties_buffer, 0, sizeof(properties_buffer)))
    AZ_CHECK(AzureIoTMessage_PropertiesAppendComponentName(&properties_message, component_name, component_name_length))

    AZ_CHECK(azure_iot_hub_send_telemetry(context, payload, payload_length, &properties_message, qos, packet_id))

    AZ_CHECK_RETURN_LAST()
}

AzureIoTResult_t azure_iot_hub_send_json_telemetry_from_component(azure_iot_hub_context_t *context,
                                                                  const uint8_t *component_name,
                                                                  uint32_t component_name_length,
                                                                  const uint8_t *payload,
                                                                  uint32_t payload_length,
                                                                  AzureIoTHubMessageQoS_t qos,
                                                                  uint16_t *packet_id)
{
    // Message format: /?property=value
    // 6 -> $.sub=
    // 5 -> $.ct=
    // 5 -> $.ce=

    uint8_t properties_buffer[AZURE_CONST_COMPONENT_NAME_MAX_LENGTH + 6 + 5 + 5];
    AzureIoTMessageProperties_t properties_message;

    AZ_CHECK_BEGIN()

    AZ_CHECK(AzureIoTMessage_PropertiesInit(&properties_message, properties_buffer, 0, sizeof(properties_buffer)))
    AZ_CHECK(AzureIoTMessage_PropertiesAppendComponentName(&properties_message, component_name, component_name_length))
    AZ_CHECK(AzureIoTMessage_PropertiesAppendContentType(&properties_message, (uint8_t*)"application/json", sizeof("application/json") - 1))
    AZ_CHECK(AzureIoTMessage_PropertiesAppendContentEncoding(&properties_message, (uint8_t*)"utf-8", sizeof("utf-8") - 1))

    AZ_CHECK(azure_iot_hub_send_telemetry(context, payload, payload_length, &properties_message, qos, packet_id))

    AZ_CHECK_RETURN_LAST()
}
