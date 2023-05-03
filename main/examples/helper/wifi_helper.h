#ifndef __MAIN_EX_HELPER_WIFI_H__
#define __MAIN_EX_HELPER_WIFI_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize WiFi service.
     * @return Success or failure.
     */
    bool helper_wifi_init();

    /**
     * @brief Connect to a WiFi network.
     * @param[in] ssid Network SSID.
     * @param[in] ssid_length Network SSID length.
     * @param[in] password Network password.
     * @param[in] password_length Network password length.
     * @return Success or failure.
     */
    bool helper_wifi_connect(const char *ssid,
                             uint16_t ssid_length,
                             const char *password,
                             uint16_t password_length);

    /**
     * @brief Free WiFi service resources.
     * @return Success or failure.
     */
    bool helper_wifi_deinit();
#ifdef __cplusplus
}
#endif
#endif