# ESP32 IoT Azure

![GitHub Build Status](https://github.com/gfurtadoalmeida/esp32-iot-azure/actions/workflows/build.yml/badge.svg) [![Bugs](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=bugs)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=code_smells)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=sqale_rating)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=security_rating)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure) [![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=esp32_iot_azure&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=esp32_iot_azure)  
ESP32 library to access [Azure IoT Services](https://azure.microsoft.com/en-us/solutions/iot), in a easier way.  
Just clone, configure through [menuconfig](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html) and use!  

## Characteristics

* ESP-IDF: [v4.4.4](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32/index.html)
* API: all functions from [Azure IoT Middleware for FreeRTOS](https://github.com/Azure/azure-iot-middleware-freertos), plus custom extensions.
* Features supported ([IoT Hub](https://learn.microsoft.com/en-us/azure/iot-hub/)/[IoT Central](https://learn.microsoft.com/en-us/azure/iot-central/)):
    * [Device Provisioning Service (DPS)](https://learn.microsoft.com/en-us/azure/iot-dps/)
    * [Device Update](https://learn.microsoft.com/en-us/azure/iot-hub-device-update/) _(in progress)_
    * [Digital Twins](https://learn.microsoft.com/en-us/azure/digital-twins/)
    * [IoT Plug and Play](https://learn.microsoft.com/en-us/azure/iot-develop/overview-iot-plug-and-play)
* Azure libraries used: 
  * [Azure IoT Middleware for FreeRTOS](https://github.com/Azure/azure-iot-middleware-freertos)
  * [Azure SDK for Embedded C](https://github.com/Azure/azure-sdk-for-c)
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
  * Azure Device Update: _(in progress)_

## To Do

- [ ] Compile DPS sources only when `CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DPS_ENABLED == 1`.
- [ ] Compile Device Update sources only when `CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED == 1`.
- [ ] Compile coreHTTP only when compiling Device Update (`CONFIG_ESP32_IOT_AZURE_HUB_FEATURES_DU_ENABLED == 1`).
- [ ] Create `CONFIG_ESP32_IOT_AZURE_DEVICE_AUTH_MODE` and compile only the needed auth methods.
- [ ] Create separe buffer configs for IoT Hub and Device Provisioning.

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
    azure_dps_init(dps, &registration_id);
    azure_dps_auth_set_symmetric_key(dps, &symmetric_key);
    azure_dps_create_pnp_registration_payload(&registration_payload);
    azure_dps_set_registration_payload(dps, &registration_payload);
    azure_dps_register(dps);
    azure_dps_get_device_and_hub(dps, &hostname, &device_id);
    azure_dps_deinit(dps);
    azure_dps_free(dps);

    azure_iot_sdk_deinit();

    ESP_LOGI(TAG, "hostname: %.*s", hostname.length, (char *)hostname.buffer);
    ESP_LOGI(TAG, "device_id: %.*s", device_id.length, (char *)device_id.buffer);
}
```
