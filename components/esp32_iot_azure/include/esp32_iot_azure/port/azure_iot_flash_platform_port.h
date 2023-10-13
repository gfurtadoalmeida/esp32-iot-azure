#ifndef __ESP32_IOT_AZURE_FLASH_PLAT_PORT_H__
#define __ESP32_IOT_AZURE_FLASH_PLAT_PORT_H__

#include <stdint.h>
#include "esp_partition.h"
#include "esp_ota_ops.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef AzureADUImageContext_t
     * @brief Context for partition update operations.
     */
    typedef struct AzureADUImageContext
    {
        const esp_partition_t *partition; /** @brief ESP partition context. */
        esp_ota_handle_t ota;             /** @brief ESP OTA context */
        uint32_t image_size;              /** @brief Image size to write. */
    } AzureADUImageContext_t;

    /**
     * @typedef AzureADUImage_t
     * @brief Context for partition update operations.
     * @note Redeclaration of @ref AzureADUImageContext_t
     */
    typedef AzureADUImageContext_t AzureADUImage_t;

#ifdef __cplusplus
}
#endif
#endif