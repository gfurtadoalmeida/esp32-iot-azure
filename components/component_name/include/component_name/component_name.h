#ifndef __COMPONENT_NAME_H__
#define __COMPONENT_NAME_H__

#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define COMPONENT_NAME_OK ESP_OK         /** @brief Success. */
#define COMPONENT_NAME_ERR_FAIL ESP_FAIL /** @brief Failure: generic. */

    /**
     * @typedef component_name_err_t
     * @brief Response code.
     */
    typedef esp_err_t component_name_err_t;

    void component_name_do_something();

#ifdef __cplusplus
}
#endif
#endif