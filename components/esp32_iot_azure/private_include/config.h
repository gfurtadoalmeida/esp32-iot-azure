#ifndef __ESP32_IOT_AZURE_CONFIG_H__
#define __ESP32_IOT_AZURE_CONFIG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

   /* The "defines" below will also be defined in the
      "sdkconfig" file when running "idf.py menuconfig".
      Once defined there, the ones below will have no effect.
   */

   // =============
   // AZURE IOT HUB
   // =============

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED
/**
 * @brief Enables Azure Device Provisioning Service (DPS) feature.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED 1
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED
/**
 * @brief Enables Azure Device Update feature.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED 1
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_SERVER_HOSTNAME
/**
 * @brief Azure IoT Hub server hostname.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_SERVER_HOSTNAME ""
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_SERVER_PORT
/**
 * @brief Azure IoT Hub server port.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_SERVER_PORT 8883U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_IOT_CONNECT_TIMEOUT_MS
/**
 * @brief Azure IoT Hub server connection timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_IOT_CONNECT_TIMEOUT_MS 5000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_LOOP_TIMEOUT_MS
/**
 * @brief Azure IoT Hub loop timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_LOOP_TIMEOUT_MS 500U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_SUBSCRIBE_TIMEOUT_MS
/**
 * @brief Azure IoT Hub subscription timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_SUBSCRIBE_TIMEOUT_MS 5000U
#endif

   // ===========================
   // AZURE IOT HUB: CERTIFICATES
   // ===========================

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_RSA
/**
 * @brief Use Azure Cloud RSA certificate.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_RSA 1
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_ECC
/**
 * @brief Use Azure Cloud ECC certificate.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_ECC 0
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_DE
/**
 * @brief Use Azure Germany Cloud certificate.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_DE 0
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_CN
/**
 * @brief Use Azure China Cloud certificate.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_CN 0
#endif

   // ============
   // AZURE DEVICE
   // ============

#ifndef CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID
/**
 * @brief IoT Plug and Play device or module id implemented.
 * Can be empty if not used.
 */
#define CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID "dtmi:com:esp32iotazure:TemperatureController;1"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DEVICE_USER_AGENT
/**
 * @brief Azure DPS device user agent.
 */
#define CONFIG_ESP32_IOT_AZURE_DEVICE_USER_AGENT "esp32-iot-azure"
#endif

   // =======================================
   // AZURE DEVICE PROVISIONING SERVICE (DPS)
   // =======================================

#ifndef CONFIG_ESP32_IOT_AZURE_DPS_SERVER_HOSTNAME
/**
 * @brief Azure DPS server hostname.
 */
#define CONFIG_ESP32_IOT_AZURE_DPS_SERVER_HOSTNAME "global.azure-devices-provisioning.net"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DPS_SCOPE_ID
/**
 * @brief Azure DPS scope id.
 */
#define CONFIG_ESP32_IOT_AZURE_DPS_SCOPE_ID ""
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DPS_CONNECT_TIMEOUT_MS
/**
 * @brief Azure DPS server connection timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_DPS_CONNECT_TIMEOUT_MS 5000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DPS_REGISTRATION_TIMEOUT_MS
/**
 * @brief Azure DPS server registration timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_DPS_REGISTRATION_TIMEOUT_MS 5000U
#endif

   // ===================
   // AZURE DEVICE UPDATE
   // ===================

#ifndef CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER
/**
 * @brief Azure DU company name of the device manufacturer.
 * This could be the same as the name of the original
 * equipment manufacturer (OEM). Ex: Dell.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER "Contoso Inc"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL
/**
 * @brief Azure DU device model name or ID. Ex: Alienware m15 R7.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL "Library Tester"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_DOWNLOAD_CHUNCK_SIZE
/**
 * @brief Azure DU download chunck size.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_DOWNLOAD_CHUNCK_SIZE 4096U
#endif

   // ==============================
   // AZURE DEVICE UPDATE: UDPATE ID
   // ==============================

#ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_PROVIDER
/**
 * @brief Azure DU Entity who is creating or directly responsible
 * for the update. It will often be a company name. Example: Microsoft.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_PROVIDER "Contoso Software"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME
/**
 * @brief Azure DU identifier for a class of updates.
 * It will often be a device class or model name. Example: Windows 11.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME "Azure Library Tester"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION
/**
 * @brief Azure DU, two- to four-part dot-separated numerical
 * version numbers.
 * @note Each part must be a number between 0 and 2147483647
 * and leading zeroes will be dropped. Examples: "1.0", "2021.11.8".
 */
#define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION "1.0"
#endif

   // ==================
   // TRANSPORT BACK-OFF
   // ==================

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_BASE_MS
/**
 * @brief Transport base back-off delay, in milliseconds, to use for network operation retry
 * attempts.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_BASE_MS 500U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_MAX_DELAY_MS
/**
 * @brief Transport maximum back-off delay, in milliseconds, for retrying failed operation
 *  with the server.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_MAX_DELAY_MS 1000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_SEND_TIMEOUT_MS
/**
 * @brief Transport send timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_SEND_TIMEOUT_MS 1000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_RECEIVE_TIMEOUT_MS
/**
 * @brief Transport receive timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_RECEIVE_TIMEOUT_MS 1000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_RETRY_MAX_ATTEMPTS
/**
 * @brief Transport maximum number of retries for network operation with the server.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_RETRY_MAX_ATTEMPTS 5U
#endif

   // ==============
   // TRANSPORT HTTP
   // ==============

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_MAX_RESPONSE_HEADERS_SIZE_BYTES
/**
 * @brief Maximum size, in bytes, of headers allowed from the server for HTTP operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_MAX_RESPONSE_HEADERS_SIZE_BYTES 2048U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_RECV_RETRY_TIMEOUT_MS
/**
 * @brief Maximum receive retry timeout, in milliseconds, for HTTP operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_RECV_RETRY_TIMEOUT_MS 10U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_SEND_RETRY_TIMEOUT_MS
/**
 * @brief Maximum send retry timeout, in milliseconds, for HTTP operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_SEND_RETRY_TIMEOUT_MS 10U
#endif

   // ==============
   // TRANSPORT MQTT
   // ==============

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE_IOT_HUB
/**
 * @brief Size of the network buffer for IoT Hub MQTT packets.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE_IOT_HUB 2048U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE_DPS
/**
 * @brief Size of the network buffer for Device Provisioning Service MQTT packets.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE_DPS 2048U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_STATE_ARRAY_MAX_COUNT
/**
 * @brief Maximum PUBLISH messages pending, for MQTT operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_STATE_ARRAY_MAX_COUNT 10U
#endif

#ifdef __cplusplus
}
#endif
#endif
