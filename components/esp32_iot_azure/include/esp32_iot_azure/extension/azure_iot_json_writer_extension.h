#ifndef __ESP32_IOT_AZURE_JSON_WRITER_EXT_H__
#define __ESP32_IOT_AZURE_JSON_WRITER_EXT_H__

#include "azure_iot_json_writer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Maximum characters needed by a int16_t type.
 * @details
 *   Max: 32767  = 5
 *   Min: -32768 = 6
 */
#define JSON_INT16_MAX_CHARS 6

/**
 * @brief Maximum characters needed by a int32_t type.
 * @details
 *   Max: 2147483647  = 10
 *   Min: -2147483648 = 11
 */
#define JSON_INT32_MAX_CHARS 11

/**
 * @brief Minimum space in bytes needed by a JSON object being written by @ref AzureIoTJSONWriter_t.
 * @note The length will exclude the `NULL` terminator.
 * @details JSON writer requires at least 64 bytes of slack within the output buffer,
 * above the theoretical minimal space needed. The JSON writer pessimistically
 * requires this extra space because it tries to write formatted text in chunks
 * rather than one character at a time, whenever the input data is dynamic in size.
 * @return Space need in bytes.
 */
#define AZURE_JSON_OBJECT_MIN_NEEDED_BYTES (sizeof("{}") - 1 + 64)

/**
 * @brief Calculate the space needed by a JSON property.
 * @note The length will include the property separator `,` and exclude the `NULL` terminator.
 * @param name Property name.
 * @param value_max_chars Property value maximum characters.
 * @return Space need in bytes.
 */
#define AZURE_JSON_CALC_PROPERTY_NEEDED_BYTES(name, value_max_chars) (sizeof("\"\":\"\",") + sizeof(name) + value_max_chars - 2)

#ifdef __cplusplus
}
#endif
#endif