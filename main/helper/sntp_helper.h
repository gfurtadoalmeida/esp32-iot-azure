#ifndef __MAIN_EX_HELPER_SNTP_H__
#define __MAIN_EX_HELPER_SNTP_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize SNTP service.
     * @return Success or failure.
     */
    bool helper_sntp_init();

    /**
     * @brief Wait until the SNTP service receives the
     * correct time.
     */
    void helper_sntp_wait_for_sync();

    /**
     * @brief Free SNTP service resources.
     * @return Success or failure.
     */
    bool helper_sntp_deinit();

#ifdef __cplusplus
}
#endif
#endif