#ifndef __MAIN_EX_HELPER_DNS_H__
#define __MAIN_EX_HELPER_DNS_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Set the internal DNS address to CloudFlare ones.
     * @note Primary: 1.1.1.1 | Secondary: 1.0.0.1
     * @return Success or failure.
     */
    bool helper_dns_set_servers();

#ifdef __cplusplus
}
#endif
#endif