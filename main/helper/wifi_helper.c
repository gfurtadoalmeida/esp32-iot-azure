#include "wifi_helper.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"

#define ESP_RETURN_FALSE_ON_ERROR(method, error_msg) \
    if (method != ESP_OK)                            \
    {                                                \
        ESP_LOGE(TAG_HELPER_WIFI, error_msg);        \
        return false;                                \
    }

static const char TAG_HELPER_WIFI[] = "HELPER_WIFI";

static volatile bool s_wifi_response_gotten = false;
static volatile bool s_wifi_connected = false;
static esp_netif_t *s_netif = NULL;

static void handle_wifi_event(void const *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

bool helper_wifi_init()
{
    ESP_RETURN_FALSE_ON_ERROR(esp_netif_init(), "esp_netif_init")

    s_netif = esp_netif_create_default_wifi_sta();

    if (s_netif == NULL)
    {
        ESP_LOGE(TAG_HELPER_WIFI, "esp_netif_create_default_wifi_sta");
        return false;
    }

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_init(&config), "esp_wifi_init")
    ESP_RETURN_FALSE_ON_ERROR(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &handle_wifi_event, NULL, NULL), "esp_event_handler_register <got_ip>")
    ESP_RETURN_FALSE_ON_ERROR(esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handle_wifi_event, NULL, NULL), "esp_event_handler_register <disconnected>")
    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_set_storage(WIFI_STORAGE_RAM), "esp_wifi_set_storage")
    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), "esp_wifi_set_mode")
    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_start(), "esp_wifi_start")

    return true;
}

bool helper_wifi_connect(const char *ssid, uint16_t ssid_length, const char *password, uint16_t password_length)
{
    s_wifi_response_gotten = false;
    s_wifi_connected = false;

    wifi_config_t wifi_config;
    wifi_config.sta.bssid_set = false;

    memset(&wifi_config, 0, sizeof(wifi_config_t));
    memcpy(wifi_config.sta.ssid, ssid, ssid_length);
    memcpy(wifi_config.sta.password, password, password_length);

    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config), "esp_wifi_set_config")
    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_connect(), "esp_wifi_connect")

    while (!s_wifi_response_gotten)
    {
        ESP_LOGI(TAG_HELPER_WIFI, "waiting for connection");

        vTaskDelay(pdMS_TO_TICKS(500));
    }

    return s_wifi_connected;
}

bool helper_wifi_deinit()
{
    ESP_RETURN_FALSE_ON_ERROR(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &handle_wifi_event), "failure unregistering event handler for got ip")
    ESP_RETURN_FALSE_ON_ERROR(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handle_wifi_event), "failure unregistering event handler for wifi disconnected")
    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_stop(), "esp_wifi_stop")
    ESP_RETURN_FALSE_ON_ERROR(esp_wifi_deinit(), "esp_wifi_deinit")

    esp_netif_destroy_default_wifi(s_netif);

    return true;
}

static void handle_wifi_event(void const *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG_HELPER_WIFI, "wifi event");

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG_HELPER_WIFI, "connection established");

        s_wifi_connected = true;
        s_wifi_response_gotten = true;
    }
    else if (event_base == WIFI_EVENT)
    {
        wifi_event_sta_disconnected_t *disconnect_data = (wifi_event_sta_disconnected_t *)event_data;

        ESP_LOGE(TAG_HELPER_WIFI, "disconnected from: %s", disconnect_data->ssid);
        ESP_LOGE(TAG_HELPER_WIFI, "disconnection reason: %d", disconnect_data->reason);

        s_wifi_connected = false;
        s_wifi_response_gotten = true;
    }
}