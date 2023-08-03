#include "sntp_helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "time.h"

bool helper_sntp_init()
{
    setenv("TZ", "<-03>3", 1);
    tzset();

    esp_sntp_config_t config = {
        .smooth_sync = false,
        .server_from_dhcp = false,
        .wait_for_sync = true,
        .start = true,
        .sync_cb = NULL,
        .renew_servers_after_new_IP = false,
        .ip_event_to_renew = IP_EVENT_STA_GOT_IP,
        .index_of_first_server = 0,
        .num_of_servers = 2,
        .servers = ESP_SNTP_SERVER_LIST("pool.ntp.org", "pool.ntp.br")};

    esp_netif_sntp_init(&config);
    esp_netif_sntp_start();

    return true;
}

bool helper_sntp_wait_for_sync()
{
    return esp_netif_sntp_sync_wait(portMAX_DELAY) == ESP_OK;
}

bool helper_sntp_deinit()
{
    esp_netif_sntp_deinit();

    return true;
}