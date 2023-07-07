#include "esp_log.h"
#include "esp_event.h"

#include "helper/nvs_helper.h"
#include "helper/wifi_helper.h"
#include "helper/dns_helper.h"
#include "helper/sntp_helper.h"

#include "examples/provisioning/example_provisioning.h"
#include "examples/iot_hub/example_iot_hub.h"
#include "examples/adu/example_adu.h"

#include "esp32_iot_azure/azure_iot_sdk.h"

static const char TAG[] = "main";
static void initialize_infra();

static char WIFI_SSID[] = "Yggdrasil";
static char WIFI_PASSWORD[] = "gfa18021988";
static buffer_t DEVICE_SYMMETRIC_KEY = BUFFER_FROM_LITERAL("w5vkMc/rp+yMfKzFgAgqaxmOH5L7JKYYnbQSLMfAUkfeUEhHFErNIy5fRSBZuD9N06pzOr6EnArVx3tBm+Y9wg==");
static buffer_t DEVICE_REGISTRATION_ID = BUFFER_FROM_LITERAL("test-device");

void app_main(void)
{
    initialize_infra();

    buffer_t hostname = BUFFER_WITH_FIXED_LENGTH(AZURE_CONST_HOSTNAME_MAX_LENGTH);
    buffer_t device_id = BUFFER_WITH_FIXED_LENGTH(AZURE_CONST_DEVICE_ID_MAX_LENGTH);

    azure_iot_sdk_init();

    if (!example_dps_run(&DEVICE_SYMMETRIC_KEY, &DEVICE_REGISTRATION_ID, &hostname, &device_id))
    {
        ESP_LOGE(TAG, "failure running example: example_dps_run");
        abort();
    }

    // This example only returns when a "restart" command is sent.
    // To run example_adu_run example you can:
    //   A) comment this run.
    //   B) send a "restart" command.
    if (!example_iot_hub_run(&hostname, &device_id, &DEVICE_SYMMETRIC_KEY))
    {
        ESP_LOGE(TAG, "failure running example: example_iot_hub_run");
        abort();
    }

    // if (!example_adu_run(&hostname, &device_id, &DEVICE_SYMMETRIC_KEY))
    // {
    //     ESP_LOGE(TAG, "failure running example: example_adu_run");
    //     abort();
    // }

    azure_iot_sdk_deinit();

    ESP_LOGW(TAG, "examples finished");
}

//
// INFRA
//

static void initialize_infra()
{
    if (esp_event_loop_create_default() != ESP_OK)
    {
        ESP_LOGE(TAG, "failure creating default event loop");
        abort();
    }

    if (!helper_nvs_init())
    {
        ESP_LOGE(TAG, "failure initializing flash");
        abort();
    }

    if (!helper_wifi_init())
    {
        ESP_LOGE(TAG, "failure initializing wifi");
        abort();
    }

    if (!helper_wifi_connect(WIFI_SSID, sizeof(WIFI_SSID) - 1, WIFI_PASSWORD, sizeof(WIFI_PASSWORD) - 1))
    {
        ESP_LOGE(TAG, "failure connecting to wifi");
        abort();
    }

    if (!helper_dns_set_servers())
    {
        ESP_LOGE(TAG, "failure setting dns");
        abort();
    }

    if (!helper_sntp_init())
    {
        ESP_LOGE(TAG, "failure initializing sntp");
        abort();
    }

    helper_sntp_wait_for_sync();
}