#ifndef __ESP32_IOT_AZURE_FLASH_PLAT_PORT_H__
#define __ESP32_IOT_AZURE_FLASH_PLAT_PORT_H__

#include <stdint.h>
#include "esp_partition.h"

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
        const esp_partition_t *partition; /** @brief Partition context for ESP. */
        uint32_t image_size;              /** @brief The total size of the image to write. */
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