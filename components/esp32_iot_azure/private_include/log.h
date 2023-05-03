#ifndef __ESP32_IOT_AZURE_LOG_H__
#define __ESP32_IOT_AZURE_LOG_H__

#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Log a verbose message.
 * @param tag Description tag.
 * @param format String format.
 * @param ... Arguments used by the string format.
 */
#define CMP_LOGV(tag, format, ...) ESP_LOGV(tag, format, ##__VA_ARGS__)
/**
 * @brief Log a debug message.
 * @param tag Description tag.
 * @param format String format.
 * @param ... Arguments used by the string format.
 */
#define CMP_LOGD(tag, format, ...) ESP_LOGD(tag, format, ##__VA_ARGS__)
/**
 * @brief Log an informational message.
 * @param tag Description tag.
 * @param format String format.
 * @param ... Arguments used by the string format.
 */
#define CMP_LOGI(tag, format, ...) ESP_LOGI(tag, format, ##__VA_ARGS__)
/**
 * @brief Log a warning message.
 * @param tag Description tag.
 * @param format String format.
 * @param ... Arguments used by the string format.
 */
#define CMP_LOGW(tag, format, ...) ESP_LOGW(tag, format, ##__VA_ARGS__)
/**
 * @brief Log an error message.
 * @param tag Description tag.
 * @param format String format.
 * @param ... Arguments used by the string format.
 */
#define CMP_LOGE(tag, format, ...) ESP_LOGE(tag, format, ##__VA_ARGS__)
/**
 * @brief Log a buffer of characters, separated into 16 bytes each line,
 * at debug level.
 * @note Buffer should contain only printable characters.
 * @param tag Description tag.
 * @param buffer Buffer array.
 * @param length Length of buffer in bytes.
 */
#define CMP_LOG_BUFFER_CHAR(tag, buffer, length) ESP_LOG_BUFFER_CHAR_LEVEL(tag, buffer, length, ESP_LOG_DEBUG)
/**
 * @brief Log a buffer of hex bytes at debug level.
 * @param tag Description tag.
 * @param buffer Buffer array.
 * @param length Length of buffer in bytes.
 */
#define CMP_LOG_BUFFER_HEX(tag, buffer, length) ESP_LOG_BUFFER_HEXDUMP(tag, buffer, length, ESP_LOG_DEBUG)

#ifdef __cplusplus
}
#endif
#endif
