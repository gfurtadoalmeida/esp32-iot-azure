#ifndef __MAIN_EX_IOT_HUB_H__
#define __MAIN_EX_IOT_HUB_H__

#include <stdbool.h>
#include "esp32_iot_azure/azure_iot_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    bool example_iot_hub_run(const utf8_string_t *iot_hub_hostname,
                             const utf8_string_t *device_id,
                             const utf8_string_t *device_symmetric_key);

#ifdef __cplusplus
}
#endif
#endif