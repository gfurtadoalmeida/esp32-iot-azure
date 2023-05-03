/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

// Url: https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/config/core_mqtt_config.h

#ifndef __CORE_MQTT_CONFIG_H__
#define __CORE_MQTT_CONFIG_H__

#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief The maximum number of MQTT PUBLISH messages that may be pending
 * acknowledgement at any time.
 *
 * QoS 1 and 2 MQTT PUBLISHes require acknowledgment from the server before
 * they can be completed. While they are awaiting the acknowledgment, the
 * client must maintain information about their state. The value of this
 * macro sets the limit on how many simultaneous PUBLISH states an MQTT
 * context maintains.
 */
#define MQTT_STATE_ARRAY_MAX_COUNT CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_STATE_ARRAY_MAX_COUNT

#ifdef __cplusplus
}
#endif
#endif