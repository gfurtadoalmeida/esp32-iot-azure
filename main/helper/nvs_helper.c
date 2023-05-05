#include "nvs_helper.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

static const char TAG_HELPER_NVS[] = "HELPER_NVS";

bool helper_nvs_init()
{
    esp_err_t flash_result = nvs_flash_init();

    if (flash_result == ESP_ERR_NVS_NO_FREE_PAGES || flash_result == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased.
        // Retry nvs_flash_init.

        if (nvs_flash_erase() != ESP_OK)
        {
            ESP_LOGE(TAG_HELPER_NVS, "failure initializing flash");
            return false;
        }

        if (nvs_flash_init() != ESP_OK)
        {
            ESP_LOGE(TAG_HELPER_NVS, "failure initializing flash");
            return false;
        }
    }
    else if (flash_result != ESP_OK)
    {
        ESP_LOGE(TAG_HELPER_NVS, "failure initializing flash");
        return false;
    }

    return true;
}