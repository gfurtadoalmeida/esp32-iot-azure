#ifndef __ESP32_IOT_AZURE_IOT_HTTP_CLIENT_EXT_H__
#define __ESP32_IOT_AZURE_IOT_HTTP_CLIENT_EXT_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp32_iot_azure/azure_iot_http_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Callback to be invoked when a resource chunk is downloaded.
     * @param[in] chunk chunk pointer.
     * @param[in] chunk_length Downloaded chunk length returned by \p chunk.
     * @param[in] start_offset Resource starting point where the chunk was donwloaded.
     * @param[in] resource_size Resource total size.
     * @param[in] callback_context Context passed back to the caller.
     */
    typedef bool (*azure_http_download_callback_t)(uint8_t *chunk,
                                                   uint32_t chunk_length,
                                                   uint32_t start_offset,
                                                   uint32_t resource_size,
                                                   void *callback_context);

    /**
     * @brief Get a resource size.
     * @param[in] context HTTP context.
     * @param[in,out] data_buffer The buffer into which the response header and payload will be placed.
     * @param[in] data_buffer_length The length of \p data_buffer.
     * @param[out] resource_size Pointer to where to store the resource total size.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_get_resource_size(azure_http_context_t *context,
                                                      char *data_buffer,
                                                      uint32_t data_buffer_length,
                                                      uint32_t *resource_size);

    /**
     * @brief Download a resource.
     * @param[in] context HTTP context.
     * @param[in,out] data_buffer The buffer into which the response header and payload will be placed.
     * @param[in] data_buffer_length The length of \p data_buffer.
     * @param[in] chunk_size How many bytes should be read per range request.
     * @param[in] callback Optional callback invoked when a resource chunk is downloaded.
     * @param[in] callback_context Pointer to a context to pass to the callback.
     * @param[in] resource_size Resource total size.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_download_resource(azure_http_context_t *context,
                                                      char *data_buffer,
                                                      uint32_t data_buffer_length,
                                                      uint16_t chunk_size,
                                                      azure_http_download_callback_t callback,
                                                      void *callback_context,
                                                      uint32_t resource_size);

#ifdef __cplusplus
}
#endif
#endif