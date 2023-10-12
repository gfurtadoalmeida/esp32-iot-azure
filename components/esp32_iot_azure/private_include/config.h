#ifndef __ESP32_IOT_AZURE_CONFIG_H__
#define __ESP32_IOT_AZURE_CONFIG_H__

#include "sdkconfig.h"

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
#define CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED 0
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED
/**
 * @brief Enables Azure Device Update feature.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED 0
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

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_CONNECT_TIMEOUT_MS
/**
 * @brief Azure IoT Hub server connection timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_CONNECT_TIMEOUT_MS 10000U
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
#define CONFIG_ESP32_IOT_AZURE_HUB_SUBSCRIBE_TIMEOUT_MS 10000U
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

#ifndef CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_RSA_ADD_BALTIMORE
/**
 * @brief Add Baltimore CyberTrust Root certificate to
 * the Azure Cloud RSA certificate chain.
 * @warning Baltimore CyberTrust Root certificate expires in 2025.
 * Starting in February 2023, all IoT hubs in the global Azure cloud
 * will migrate to a new TLS certificate issued by the DigiCert Global Root G2.
 * @link https://techcommunity.microsoft.com/t5/internet-of-things-blog/azure-iot-tls-critical-changes-are-almost-here-and-why-you/ba-p/2393169
 * @link https://learn.microsoft.com/en-us/azure/iot-hub/migrate-tls-certificate
 */
#define CONFIG_ESP32_IOT_AZURE_HUB_CERT_USE_AZURE_RSA_ADD_BALTIMORE 1
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
#define CONFIG_ESP32_IOT_AZURE_DEVICE_MODEL_ID "dtmi:azure:iot:deviceUpdateContractModel;2"
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
#define CONFIG_ESP32_IOT_AZURE_DPS_CONNECT_TIMEOUT_MS 10000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DPS_REGISTRATION_TIMEOUT_MS
/**
 * @brief Azure DPS server registration timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_DPS_REGISTRATION_TIMEOUT_MS 10000U
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
#define CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MANUFACTURER "ESPRESSIF"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL
/**
 * @brief Azure DU device model name or ID. Ex: Alienware m15 R7.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_DEVICE_MODEL "ESP-WROOM-32"
#endif

   // ==============================
   // AZURE DEVICE UPDATE: UDPATE ID
   // ==============================

#ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_PROVIDER
/**
 * @brief Azure DU Entity who is creating or directly responsible
 * for the update. It will often be a company name. Example: Microsoft.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_PROVIDER "Contoso"
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME_USE_PROJECT_NAME
/**
 * @brief Get the update name from project name.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME_USE_PROJECT_NAME 0
#endif

#if CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME_USE_PROJECT_NAME == 1
   /**
    * @brief Azure DU identifier for a class of updates.
    * It will often be a device class or model name. Example: Windows 11.
    */
   #define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME PROJECT_NAME
#else
   #ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME
   /**
    * @brief Azure DU identifier for a class of updates.
    * It will often be a device class or model name. Example: Windows 11.
    */
   #define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_NAME "ESP32-IoT-Azure"
   #endif
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION_USE_PROJECT_VERSION
/**
 * @brief Get the update version from project version.
 */
#define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION_USE_PROJECT_VERSION 0
#endif

#if CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION_USE_PROJECT_VERSION == 1
    /**
    * @brief Azure DU, two- to four-part dot-separated numerical
    * version numbers.
    * @note Each part must be a number between 0 and 2147483647
    * and leading zeroes will be dropped. Examples: "1.0", "2021.11.8".
    */
   #define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION PROJECT_VER
#else
   #ifndef CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION
   /**
    * @brief Azure DU, two- to four-part dot-separated numerical
    * version numbers.
    * @note Each part must be a number between 0 and 2147483647
    * and leading zeroes will be dropped. Examples: "1.0", "2021.11.8".
    */
   #define CONFIG_ESP32_IOT_AZURE_DU_UPDATE_ID_VERSION "1.0"
   #endif
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
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_MAX_DELAY_MS 2000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_SEND_TIMEOUT_MS
/**
 * @brief Transport send timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_SEND_TIMEOUT_MS 10000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_RECEIVE_TIMEOUT_MS
/**
 * @brief Transport receive timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_RECEIVE_TIMEOUT_MS 10000U
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

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_CONNECT_TIMEOUT_MS
/**
 * @brief Connection timeout, in milliseconds.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_CONNECT_TIMEOUT_MS 10000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_RECV_RETRY_TIMEOUT_MS
/**
 * @brief Maximum receive retry timeout, in milliseconds, for HTTP operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_RECV_RETRY_TIMEOUT_MS 1000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_SEND_RETRY_TIMEOUT_MS
/**
 * @brief Maximum send retry timeout, in milliseconds, for HTTP operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_SEND_RETRY_TIMEOUT_MS 1000U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_MAX_RESPONSE_HEADERS_SIZE_BYTES
/**
 * @brief Maximum size, in bytes, of headers allowed from the server for HTTP operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_MAX_RESPONSE_HEADERS_SIZE_BYTES 3072U
#endif

   // ==============
   // TRANSPORT MQTT
   // ==============

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
