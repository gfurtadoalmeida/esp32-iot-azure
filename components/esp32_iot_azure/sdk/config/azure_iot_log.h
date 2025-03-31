#ifndef __AZURE_IOT_LOG_H__
#define __AZURE_IOT_LOG_H__

#include "esp_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME "AZ_SDK"
#endif

#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL LOG_INFO
#endif

#define SINGLE_PARENTHESIS_LOGE(x, ...) ESP_LOGE(LIBRARY_LOG_NAME, x, ##__VA_ARGS__)
#define SINGLE_PARENTHESIS_LOGI(x, ...) ESP_LOGI(LIBRARY_LOG_NAME, x, ##__VA_ARGS__)
#define SINGLE_PARENTHESIS_LOGW(x, ...) ESP_LOGW(LIBRARY_LOG_NAME, x, ##__VA_ARGS__)
#define SINGLE_PARENTHESIS_LOGD(x, ...) ESP_LOGD(LIBRARY_LOG_NAME, x, ##__VA_ARGS__)

#define LogError(message) SINGLE_PARENTHESIS_LOGE message
#define LogInfo(message) SINGLE_PARENTHESIS_LOGI message
#define LogWarn(message) SINGLE_PARENTHESIS_LOGW message
#define LogDebug(message) SINGLE_PARENTHESIS_LOGD message
#define SdkLog(message) SINGLE_PARENTHESIS_LOGI message

#ifdef __cplusplus
}
#endif
#endif