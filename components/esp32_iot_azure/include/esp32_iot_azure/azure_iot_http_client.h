#ifndef __ESP32_IOT_AZURE_IOT_HTTP_CLIENT_H__
#define __ESP32_IOT_AZURE_IOT_HTTP_CLIENT_H__

#include <stdint.h>
#include "azure_iot_http.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef azure_http_context_t
     * @brief Azure HTTP context.
     */
    typedef struct azure_http_context_t azure_http_context_t;

    /**
     * @brief Create an Azure HTTP context.
     * @note The context must be released by @ref azure_http_free.
     * @param[in] url URL to use for this request. Must be null-terminated.
     * @param[in] url_length Length of \p url without null-termination.
     * @param[in] path Path to use for this request. Must be null-terminated.
     * @param[in] path_length Length of \p path without null-termination.
     * @return @ref azure_http_context_t on success or null on failure.
     */
    azure_http_context_t *azure_http_create(const char *url,
                                            uint32_t url_length,
                                            const char *path,
                                            uint32_t path_length);

    /**
     * @brief Connect the Azure HTTP client.
     * @param[in] context HTTP context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_connect(azure_http_context_t *context);

    /**
     * @brief Disconnect the Azure HTTP client.
     * @param[in] context HTTP context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    void azure_http_disconnect(azure_http_context_t *context);

    /**
     * @brief Initialize a size request.
     * @param[in] context HTTP context.
     * @param[out] header_buffer The buffer into which the headers for the request will be placed.
     * @param[in] header_buffer_length The length of \p header_buffer.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_request_size_init(azure_http_context_t *context,
                                                      char *header_buffer,
                                                      uint32_t header_buffer_length);

    /**
     * @brief Send a size request.
     * @param[in] context HTTP context.
     * @param[out] data_buffer The buffer where the response will be placed.
     * @param[in] data_buffer_length The size of \p data_buffer.
     * @return The size of the file if success, -1 if failure.
     */
    int32_t azure_http_request_size(azure_http_context_t *context,
                                    char *data_buffer,
                                    uint32_t data_buffer_length);

    /**
     * @brief Initialize a get content request.
     * @param[in] context HTTP context.
     * @param[out] header_buffer The buffer into which the headers for the request will be placed.
     * @param[in] header_buffer_length The length of \p header_buffer.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_init(azure_http_context_t *context,
                                         char *header_buffer,
                                         uint32_t header_buffer_length);

    /**
     * @brief Send a get content request.
     * @param[in] context HTTP context.
     * @param[in] range_start The start point for the request payload.
     * @param[in] range_end The end point for the request payload.
     * @param[out] data_buffer The buffer into which the response header and payload will be placed.
     * @param[in] data_buffer_length The length of \p data_buffer.
     * @param[out] output_data The pointer to the point in the buffer where the payload starts.
     * @param[out] output_data_length The length of the payload returned by \p output_data.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_request(azure_http_context_t *context,
                                            int32_t range_start,
                                            int32_t range_end,
                                            char *data_buffer,
                                            uint32_t data_buffer_length,
                                            char **output_data,
                                            uint32_t *output_data_length);

    /**
     * @brief Deinitialize a get content request.
     * @param[in] context HTTP context.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTHTTPResult_t azure_http_deinit(azure_http_context_t *context);

    /**
     * @brief Cleanup and free the context.
     * @param[in] context HTTP context.
     */
    void azure_http_free(azure_http_context_t *context);

#ifdef __cplusplus
}
#endif
#endif