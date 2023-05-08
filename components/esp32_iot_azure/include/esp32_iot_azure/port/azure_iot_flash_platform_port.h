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
        uint8_t *image_buffer;            /** @brief The buffer containing the bytes to write to the flash. */
        uint32_t image_buffer_length;     /** @brief The length of the buffer from which to write the bytes. */
        uint32_t image_size;              /** @brief The total size of the image to write. */
        uint32_t partition_offset;        /** @brief The offset for the partition to write the bytes. */
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