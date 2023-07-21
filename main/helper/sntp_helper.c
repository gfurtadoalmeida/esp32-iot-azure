#include "sntp_helper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sntp.h"
#include "esp_log.h"

static const char TAG_HELPER_SNTP[] = "HELPER_SNTP";

static bool s_got_time = false;

static void handle_sntp_event(const struct timeval *time_received);

bool helper_sntp_init()
{
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "pool.ntp.br");
    sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);

    setenv("TZ", "<-03>3", 1);
    tzset();

    sntp_set_time_sync_notification_cb(&handle_sntp_event);
    sntp_init();

    return true;
}

void helper_sntp_wait_for_sync()
{
    while (!s_got_time)
    {
        ESP_LOGI(TAG_HELPER_SNTP, "waiting for time");

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

bool helper_sntp_deinit()
{
    sntp_stop();

    return true;
}

static void handle_sntp_event(const struct timeval *time_received)
{
    ESP_LOGI(TAG_HELPER_SNTP, "got time: %lld", time_received->tv_sec);

    s_got_time = true;
}