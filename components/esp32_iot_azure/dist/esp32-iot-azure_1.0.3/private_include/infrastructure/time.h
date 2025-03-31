#ifndef __ESP32_IOT_AZURE_INFRA_TIME_H__
#define __ESP32_IOT_AZURE_INFRA_TIME_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Time function that complies with @ref AzureIoTGetCurrentTimeFunc_t contract.
     * @return The time in seconds since Unix epoch.
     */
    uint64_t time_get_unix();

#endif
#ifdef __cplusplus
}
#endif