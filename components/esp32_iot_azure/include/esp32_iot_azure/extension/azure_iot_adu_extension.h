#ifndef __ESP32_IOT_AZURE_IOT_ADU_EXT_H__
#define __ESP32_IOT_AZURE_IOT_ADU_EXT_H__

#include <stdint.h>
#include "esp32_iot_azure/azure_iot_adu.h"
#include "esp32_iot_azure/extension/azure_iot_http_client_extension.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint8_t *hostname;        /** @brief Pointer to the point in the buffer where the hostname starts. */
        uint32_t hostname_length; /** @brief Length of the hostname returned by \p hostname, including the null-terminator. */
        uint8_t *path;            /** @brief Pointer to the point in the buffer where the path starts. */
        uint32_t path_length;     /** @brief Length of the path returned by \p path, including the null-terminator. */
    } parsed_file_url_t;

    /**
     * @brief Parse a @ref AzureIoTADUUpdateManifestFileUrl_t into hostname and path.
     * @param[in] file_url Pointer to a @ref AzureIoTADUUpdateManifestFileUrl_t.
     * @param[in] parse_buffer Buffer on which the parsed values will be copied.
     * It must have at least (file_url.ulUrlLength + 2) size.
     * @param[in] parsed_url Pointer to a @ref parsed_file_url_t structure
     * where the parsed content will be stored.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_file_parse_url(AzureIoTADUUpdateManifestFileUrl_t *file_url,
                                              uint8_t *parse_buffer,
                                              parsed_file_url_t *parsed_url);

    /**
     * @brief Download a file.
     * @param[in] parsed_url Parsed url of the file to be downloaded.
     * @param[in,out] data_buffer The buffer into which the response header and payload will be placed.
     * @param[in] data_buffer_length The length of \p data_buffer.
     * @param[in] chunck_size How many bytes should be read per range request.
     * @param[in] callback Callback invoked when a resource chunck is downloaded.
     * @param[in] callback_context Pointer to a context to pass to the callback.
     * @param[in] file_size Pointer to where to store the file total size.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_adu_file_download(parsed_file_url_t *parsed_url,
                                             uint8_t *data_buffer,
                                             uint32_t data_buffer_length,
                                             uint16_t chunck_size,
                                             azure_http_download_callback_t callback,
                                             void *callback_context,
                                             uint32_t *file_size);
#ifdef __cplusplus
}
#endif
#endif