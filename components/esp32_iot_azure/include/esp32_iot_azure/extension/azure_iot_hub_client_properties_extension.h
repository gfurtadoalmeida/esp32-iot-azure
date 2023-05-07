#ifndef __ESP32_IOT_AZURE_HUB_CLI_PROP_EXT_H__
#define __ESP32_IOT_AZURE_HUB_CLI_PROP_EXT_H__

#include "esp32_iot_azure/azure_iot_common.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Space needed for a JSON object belonging to a component, being written
 * by @ref AzureIoTHubClientProperties_BuilderBeginComponent and @ref AzureIoTHubClientProperties_BuilderEndComponent.
 * @note The length will exclude the `NULL` terminator.
 * @note About: https://learn.microsoft.com/en-us/azure/iot-develop/concepts-convention#sample-multiple-components-writable-property
 * @param component_name Component name.
 * @return Space need in bytes.
 */
#define AZURE_JSON_SIZEOF_IOTHUB_CLIENT_PROPERTY_BUILDER_COMPONENT(component_name) (sizeof("\"" component_name "\":{\"__t\":\"c\",}") - 1)

/**
 * @brief Space needed for a property response to a writable property request, being
 * written by @ref AzureIoTHubClientProperties_BuilderBeginResponseStatus and @ref AzureIoTHubClientProperties_BuilderEndResponseStatus.
 * @note The length will exclude the `NULL` terminator.
 * @param property_name Property name.
 * @param description_length Acknowledgement description length.
 * @param value_max_chars Property value maximum characters.
 * @return Space need in bytes.
 */
#define AZURE_JSON_SIZEOF_IOTHUB_CLIENT_PROPERTY_BUILDER_RESPONSE_STATUS(property_name, description_length, value_max_chars) (AZURE_JSON_SIZEOF_OBJECT_PROPERTY(property_name, (sizeof("{\"ac\":,\"av\":,\"ad\":\"\",\"value\":\"\"}") + JSON_INT16_MAX_CHARS + JSON_INT32_MAX_CHARS + description_length + value_max_chars - 1)))

#ifdef __cplusplus
}
#endif
#endif