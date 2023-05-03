#ifndef __MAIN_EX_DPS_H__
#define __MAIN_EX_DPS_H__

#include <stdbool.h>
#include "esp32_iot_azure/azure_iot_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    bool example_dps_run(const utf8_string_t *device_symmetric_key,
                         const utf8_string_t *device_registration_id,
                         utf8_string_t *iot_hub_hostname,
                         utf8_string_t *device_id);

#ifdef __cplusplus
}
#endif
#endif