#ifndef __MAIN_EX_HELPER_NVS_H__
#define __MAIN_EX_HELPER_NVS_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize NVS flash.
     * @note Must be called just once.
     * @return Success or failure.
     */
    bool helper_nvs_init();

#ifdef __cplusplus
}
#endif
#endif