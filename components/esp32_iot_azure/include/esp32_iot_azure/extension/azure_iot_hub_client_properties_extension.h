#ifndef __ESP32_IOT_AZURE_HUB_CLI_PROP_EXT_H__
#define __ESP32_IOT_AZURE_HUB_CLI_PROP_EXT_H__

#include "esp32_iot_azure/azure_iot_common.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Minimum space in bytes needed by a IoT Hub Client Property Component JSON
 * object being written by @ref AzureIoTJSONWriter_t.
 * @note The length will exclude the `NULL` terminator.
 * @param component_name Component name.
 * @return Space need in bytes.
 */
#define AZURE_JSON_CALC_CLIENT_PROPERTY_MIN_COMPONENT_OBJECT_NEEDED_BYTES(component_name) (sizeof("\"\":{\"__t\":\"c\",}") + sizeof(component_name) - 2)

/**
 * @brief Minimum space in bytes needed by a IoT Hub Client Property Response Status JSON
 * object being written by @ref AzureIoTJSONWriter_t.
 * @note The length will exclude the `NULL` terminator.
 * @param property_name Property name.
 * @param ack_description Acknowledgement description.
 * @param value_max_chars Property value maximum characters.
 * @return Space need in bytes.
 */
#define AZURE_JSON_CALC_CLIENT_PROPERTY_MIN_RESPONSE_STATUS_PROPERTY_NEEDED_BYTES(property_name, ack_description, value_max_chars) (AZURE_JSON_CALC_PROPERTY_NEEDED_BYTES(property_name, (sizeof("{\"ac\":999,\"av\":,\"ad\":\"\",\"value\":\"\"}") + JSON_INT32_MAX_CHARS + sizeof(ack_description) + value_max_chars - 2)))

#ifdef __cplusplus
}
#endif
#endif