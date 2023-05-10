#ifndef __MAIN_DTDL_TEMP_CTRL_H__
#define __MAIN_DTDL_TEMP_CTRL_H__

#include "esp32_iot_azure/azure_iot_common.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief Temperature Controller device status.
     */
    typedef enum
    {
        TEMP_CONTROLLER_STATUS_NORMAL = 1,
        TEMP_CONTROLLER_STATUS_ERROR = 2
    } temperature_controller_status_t;

/** @brief Temperature Controller `restart` command name. */
#define TEMP_CTRL_CMD_RESTART_NAME "restart"

/** @brief Temperature Controller `deviceStatus` property name. */
#define TEMP_CTRL_PRP_DEVICE_STATUS_NAME "deviceStatus"

/** @brief Temperature Controller `display` component name. */
#define TEMP_CTRL_CMP_DISPLAY_NAME "display"

/** @brief Temperature Controller, `display` component, `brightness` property name. */
#define TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME "brightness"

/** @brief Temperature Controller `thermostat` component name. */
#define TEMP_CTRL_CMP_THERMOSTAT_NAME "thermostat"

/** @brief Temperature Controller, `thermostat` component, `temperature` telemetry name. */
#define TEMP_CTRL_CMP_THERMOSTAT_PRP_TLY_TEMPERATURE_NAME "temp"

#ifdef __cplusplus
}
#endif
#endif