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

   // =====================
   // AZURE IOT Hub
   // =====================

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

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE
/**
 * @brief Size of the network buffer for MQTT packets.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_BUFFER_SIZE 5120U
#endif

#ifndef CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_STATE_ARRAY_MAX_COUNT
/**
 * @brief Maximum PUBLISH messages pending, for MQTT operations.
 */
#define CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_STATE_ARRAY_MAX_COUNT 10U
#endif

   // ============================
   // AZURE ROOT CERTIFICATE CHAIN
   // ============================

   /**
    * @brief Required Azure certificates.
    * @warning Hard coding certificates is not recommended by Microsoft as a best
    * practice for production scenarios. Please see our document here for notes on best practices.
    * https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/docs/certificate-notice.md
    * @warning There are critical changes on Azure IoT Hub certificates.
    * https://techcommunity.microsoft.com/t5/internet-of-things-blog/azure-iot-tls-critical-changes-are-almost-here-and-why-you/ba-p/2393169
    * @details Certificate list:
    *  - Baltimore CyberTrust Root
    *  - DigiCert Global Root G2
    *  - Microsoft RSA Root Certificate Authority 2017
    * @details Microsoft certificates: https://learn.microsoft.com/en-us/azure/security/fundamentals/azure-ca-details?tabs=root-and-subordinate-cas-list
    * @details C/C++ certificates: https://github.com/Azure/azure-iot-sdk-c/blob/main/certs/certs.c
    */
   static const char AZURE_CLOUD_RSA_CERT[] =
       /* Baltimore CyberTrust Root */
       "-----BEGIN CERTIFICATE-----\r\n"
       "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
       "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n"
       "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
       "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n"
       "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
       "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n"
       "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
       "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n"
       "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
       "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n"
       "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
       "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n"
       "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
       "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n"
       "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
       "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n"
       "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
       "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n"
       "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"
       "-----END CERTIFICATE-----\r\n"

       /* DigiCert Global Root G2 */
       "-----BEGIN CERTIFICATE-----\r\n"
       "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\r\n"
       "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"
       "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\r\n"
       "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\r\n"
       "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\r\n"
       "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\r\n"
       "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\r\n"
       "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\r\n"
       "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\r\n"
       "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\r\n"
       "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\r\n"
       "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\r\n"
       "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\r\n"
       "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\r\n"
       "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\r\n"
       "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\r\n"
       "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\r\n"
       "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\r\n"
       "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\r\n"
       "MrY=\r\n"
       "-----END CERTIFICATE-----\r\n"

       /* Microsoft RSA Root Certificate Authority 2017 */
       "-----BEGIN CERTIFICATE-----\r\n"
       "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\r\n"
       "RA+GsCyRxj3qrg+E\r\n"
       "-----END CERTIFICATE-----\r\n";

#ifdef __cplusplus
}
#endif
#endif
