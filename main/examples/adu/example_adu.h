#ifndef __MAIN_EX_ADU_H__
#define __MAIN_EX_ADU_H__

#include <stdbool.h>
#include "esp32_iot_azure/azure_iot_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Run the Azure IoT Hub example.
     * @param[in] iot_hub_hostname IoT Hub hostname.
     * @param[in] device_id Device Id.
     * @param[in] device_symmetric_key Device symmetric key.
     * @return Success or failure.
     */
    bool example_adu_run(const utf8_string_t *iot_hub_hostname,
                         const utf8_string_t *device_id,
                         const utf8_string_t *device_symmetric_key);

#ifdef __cplusplus
}
#endif
#endif