# ESP32 IoT Azure

![GitHub Build Status](https://github.com/gfurtadoalmeida/esp32-iot-azure/actions/workflows/build.yml/badge.svg) [![Bugs](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=bugs)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure)  
ESP32 library to access [Azure IoT Services](https://azure.microsoft.com/en-us/solutions/iot), in an easier and cleaner way.  
Just clone, configure using [menuconfig](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html) and connect!  

## Characteristics

* ESP-IDF: [v4.4.4](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/index.html)
* Written in **C** using:
   * [ESP-IDF Framework](https://github.com/espressif/esp-idf)
   * [Azure IoT Middleware for FreeRTOS](https://github.com/Azure/azure-iot-middleware-freertos)
   * [Azure SDK for Embedded C](https://github.com/Azure/azure-sdk-for-c)
* APIs: 
   * All functions from [Azure IoT Middleware for FreeRTOS](https://github.com/Azure/azure-iot-middleware-freertos)
   * [Custom extensions](/components/esp32_iot_azure/include/esp32_iot_azure/extension/)
* Features supported ([IoT Hub](https://learn.microsoft.com/en-us/azure/iot-hub/)/[IoT Central](https://learn.microsoft.com/en-us/azure/iot-central/)):
    * [Device Provisioning Service (DPS)](https://learn.microsoft.com/en-us/azure/iot-dps/)
    * [Device Update](https://learn.microsoft.com/en-us/azure/iot-hub-device-update/)
    * [Digital Twins](https://learn.microsoft.com/en-us/azure/digital-twins/)
    * [IoT Plug and Play](https://learn.microsoft.com/en-us/azure/iot-develop/overview-iot-plug-and-play)
* Transport: 
  * Socket: [esp_tls](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_tls.html) with built-in connection retries and updated Azure certificates.
  * HTTP: [FreeRTOS coreHTTP](https://github.com/FreeRTOS/coreHTTP)
  * MQTT: [FreeRTOS coreMQTT](https://github.com/FreeRTOS/coreMQTT)
* Cryptography: [mbedtls](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/mbedtls.html)
* Configurable: using [menuconfig](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html)

## Documentation

* Everything is on the [wiki](https://github.com/gfurtadoalmeida/esp32-project-template/wiki).
* Examples:
  * Azure IoT Hub: [examples/iot_hub](/main/examples/iot_hub)
  * Azure Device Provisioning Service: [examples/provisioning](/main/examples/provisioning)
  * Azure Device Update: [examples/adu](/main/examples/adu)

## Code Size

Build options:

* Compile optimized for size (`CONFIG_COMPILER_OPTIMIZATION_SIZE=y`).
* Error logging (`CONFIG_LOG_DEFAULT_LEVEL_ERROR=y`).
* Merged buffers (`CONFIG_ESP32_IOT_AZURE_TRANSPORT_MQTT_MERGE_IOT_HUB_DPS_BUFFERS=y`).

This chart include the following configurable memory buffers, on DRAM:

* IoT Hub + DPS: 5.12 KB
* Device Update Workflow: 8.19 KB

| | DRAM | Flash (code,rodata) |
|-|:-:|:-:|
| IoT Hub + PnP + DPS + ADU + ADU workflow | buffers: 13.31 KB <br/> bss,data: 842 B | 97.31 KB |
| IoT Hub + PnP + DPS | buffers: 5.12 KB <br/> bss,data: 24 B | 54.62 KB |
| IoT Hub + PnP | buffers: 5.12 KB <br/> bss,data: 20 B | 44.30 KB |

## Example: Getting Device and Hub Info from DPS

```cpp
#include "esp32_iot_azure/azure_iot_sdk.h"
#include "esp32_iot_azure/azure_iot_provisioning.h"
#include "esp32_iot_azure/extension/azure_iot_provisioning_extension.h"

void app_main(void)
{
    // 1. Configure this library via menuconfig.
    // 2. Connect to WiFi.
    // 3. Set up SNTP.

    utf8_string_t symmetric_key = UTF8_STRING_FROM_LITERAL("you_device_enrollment_symmetric_key");
    utf8_string_t registration_id = UTF8_STRING_FROM_LITERAL("your_device_registration_id");

    utf8_string_t registration_payload = UTF8_STRING_FOR_DPS_PNP_REGISTRATION_PAYLOAD();
    utf8_string_t hostname = UTF8_STRING_WITH_FIXED_LENGTH(AZURE_CONST_HOSTNAME_MAX_LENGTH);
    utf8_string_t device_id = UTF8_STRING_WITH_FIXED_LENGTH(AZURE_CONST_DEVICE_ID_MAX_LENGTH);

    azure_iot_sdk_init();

    AzureIoTProvisioningClientOptions_t *dps_client_options = NULL;

    azure_dps_context_t *dps = azure_dps_create();
    azure_dps_options_init(dps, &dps_client_options);
    azure_dps_init(dps, registration_id->buffer, registration_id->length);
    azure_dps_auth_set_symmetric_key(dps, symmetric_key->buffer, symmetric_key->length);
    azure_dps_create_pnp_registration_payload(registration_payload.buffer, &registration_payload.length);
    azure_dps_set_registration_payload(dps, registration_payload.buffer, registration_payload.length);
    azure_dps_register(dps);
    azure_dps_get_device_and_hub(dps,
                                 hostname->buffer,
                                 &hostname->length,
                                 device_id->buffer,
                                 &device_id->length);
    azure_dps_deinit(dps);
    azure_dps_free(dps);

    azure_iot_sdk_deinit();

    ESP_LOGI(TAG, "hostname: %.*s", hostname.length, (char *)hostname.buffer);
    ESP_LOGI(TAG, "device_id: %.*s", device_id.length, (char *)device_id.buffer);
}
```
