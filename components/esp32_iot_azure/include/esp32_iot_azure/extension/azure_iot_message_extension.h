#ifndef __ESP32_IOT_AZURE_MESSAGE_EXT_H__
#define __ESP32_IOT_AZURE_MESSAGE_EXT_H__

#include "azure_iot_message.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Append a property that refers to a device component. Useful to relate telemetry to a component.
     * @note It will add a property with name "$.sub" (5 chars) and value \p component_name
     * @note The properties init API will not encode properties. In order to support the following characters,
     * they must be percent-encoded (RFC3986) as follows: - `/` : `%2F` - `%` : `%25` - `#` : `%23` - `&` : `%26`.
     * Only these characters would have to be encoded. If you would like to avoid the need to encode the names/values,
     * avoid using these characters in names and values.
     * @param[in] message_properties @ref AzureIoTMessageProperties_t to use for the operation.
     * @param[in] component_name Component name.
     * @param[in] component_name_length Component name length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t AzureIoTMessage_PropertiesAppendComponentName(AzureIoTMessageProperties_t *message_properties,
                                                                   const uint8_t *component_name,
                                                                   uint32_t component_name_length);

    /**
     * @brief Append a property that refers to the message content type.
     * @note It will add a property with name "$.ct" (4 chars) and value \p content_type
     * @note The properties init API will not encode properties. In order to support the following characters,
     * they must be percent-encoded (RFC3986) as follows: - `/` : `%2F` - `%` : `%25` - `#` : `%23` - `&` : `%26`.
     * Only these characters would have to be encoded. If you would like to avoid the need to encode the names/values,
     * avoid using these characters in names and values.
     * @param[in] message_properties @ref AzureIoTMessageProperties_t to use for the operation.
     * @param[in] content_type Content type name.
     * @param[in] content_type_length Content type name length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t AzureIoTMessage_PropertiesAppendContentType(AzureIoTMessageProperties_t *message_properties,
                                                                 const uint8_t *content_type,
                                                                 uint32_t content_type_length);

    /**
     * @brief Append a property that refers to the message content encoding.
     * @note It will add a property with name "$.ce" (4 chars) and value \p content_encoding
     * @note The properties init API will not encode properties. In order to support the following characters,
     * they must be percent-encoded (RFC3986) as follows: - `/` : `%2F` - `%` : `%25` - `#` : `%23` - `&` : `%26`.
     * Only these characters would have to be encoded. If you would like to avoid the need to encode the names/values,
     * avoid using these characters in names and values.
     * @param[in] message_properties @ref AzureIoTMessageProperties_t to use for the operation.
     * @param[in] content_encoding Content type name.
     * @param[in] content_encoding_length Content type name length.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t AzureIoTMessage_PropertiesAppendContentEncoding(AzureIoTMessageProperties_t *message_properties,
                                                                     const uint8_t *content_encoding,
                                                                     uint32_t content_encoding_length);

#ifdef __cplusplus
}
#endif
#endif