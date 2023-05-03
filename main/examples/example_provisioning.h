#ifndef __MAIN_EX_DPS_H__
#define __MAIN_EX_DPS_H__

#include <stdbool.h>
#include "esp32_iot_azure/azure_iot_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Run the Azure IoT Device Provisioning Service example.
     * @param[in] device_symmetric_key Device symmetric key.
     * @param[in] device_registration_id Device registration id.
     * @param[in,out] iot_hub_hostname IoT Hub hostname buffer with capacity for AZURE_CONST_HOSTNAME_MAX_LENGTH chars.
     * @param[in,out] device_id Device Id buffer with capacity for AZURE_CONST_DEVICE_ID_MAX_LENGTH chars.
     * @return Success or failure.
     */
    bool example_dps_run(const utf8_string_t *device_symmetric_key,
                         const utf8_string_t *device_registration_id,
                         utf8_string_t *iot_hub_hostname,
                         utf8_string_t *device_id);

#ifdef __cplusplus
}
#endif
#endif