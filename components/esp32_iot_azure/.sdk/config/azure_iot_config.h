/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

// Url: https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/config/azure_iot_config.h
// Url: https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/esp32/config/demo_config.h

#ifndef __AZURE_IOT_CONFIG_H__
#define __AZURE_IOT_CONFIG_H__

#include "azure_iot_log.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define AZLogError(message) SINGLE_PARENTHESIS_LOGE message
#define AZLogInfo(message) SINGLE_PARENTHESIS_LOGI message
#define AZLogWarn(message) SINGLE_PARENTHESIS_LOGW message
#define AZLogDebug(message) SINGLE_PARENTHESIS_LOGD message

/**
 * @brief Default timeout used for generating SAS token.
 */
#define azureiotconfigDEFAULT_TOKEN_TIMEOUT_IN_SEC (60 * 60U)

/**
 * @brief MQTT keep alive.
 */
#define azureiotconfigKEEP_ALIVE_TIMEOUT_SECONDS (60U)

/**
 * @brief Receive timeout for MQTT CONNACK.
 */
#define azureiotconfigCONNACK_RECV_TIMEOUT_MS (1000U)

/**
 * @brief Wait timeout of MQTT SUBACK.
 */
#define azureiotconfigSUBACK_WAIT_INTERVAL_MS (10U)

/**
 * @brief Max MQTT username.
 */
#define azureiotconfigUSERNAME_MAX (256U)

/**
 * @brief Max MQTT password.
 */
#define azureiotconfigPASSWORD_MAX (384U)

/**
 * @brief Max MQTT topic length.
 */
#define azureiotconfigTOPIC_MAX (128U)

/**
 * @brief Max provisioning response payload supported.
 */
#define azureiotconfigPROVISIONING_REQUEST_PAYLOAD_MAX (512U)

#ifdef __cplusplus
}
#endif
#endif