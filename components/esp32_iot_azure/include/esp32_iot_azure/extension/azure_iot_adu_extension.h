#ifndef __ESP32_IOT_AZURE_IOT_ADU_EXT_H__
#define __ESP32_IOT_AZURE_IOT_ADU_EXT_H__

#include <stdint.h>
#include "esp32_iot_azure/azure_iot_adu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Parse a @ref AzureIoTADUUpdateManifestFileUrl_t into hostname and path.
     * @param[in] file_url Pointer to a @ref AzureIoTADUUpdateManifestFileUrl_t.
     * @param[in] parse_buffer Buffer on which the parsed values will be copied.
     * It must have at least file_url.ulUrlLength size.
     * @param[in] hostname Pointer to the point in the buffer where the hostname starts.
     * @param[in] hostname_length Length of the hostname returned by \p hostname, including the null-terminator.
     * @param[in] path Pointer to the point in the buffer where the path starts.
     * @param[in] path_length Length of the path returned by \p path, including the null-terminator.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_parse_file_url(AzureIoTADUUpdateManifestFileUrl_t *file_url,
                                              uint8_t *parse_buffer,
                                              uint8_t **hostname,
                                              uint32_t *hostname_length,
                                              uint8_t **path,
                                              uint32_t *path_length);

#ifdef __cplusplus
}
#endif
#endif