#ifndef __ESP32_IOT_AZURE_JSON_READER_EXT_H__
#define __ESP32_IOT_AZURE_JSON_READER_EXT_H__

#include "azure_iot_json_reader.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Skip over a property and its value.
     * @param[in] json_reader A pointer to an @ref AzureIoTJSONReader_t.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t AzureIoTJSONReader_SkipPropertyAndValue(AzureIoTJSONReader_t *json_reader);

    /**
     * @brief Get the JSON token's number as a 16-bit unsigned integer.
     * @param[in] json_reader A pointer to an @ref AzureIoTJSONReader_t.
     * @param[out] value A pointer to a variable to receive the value.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t AzureIoTJSONReader_GetTokenUInt16(AzureIoTJSONReader_t *json_reader, uint16_t *value);

#ifdef __cplusplus
}
#endif
#endif