#include "esp32_iot_azure/extension/azure_iot_adu_extension.h"
#include "log.h"

static const char TAG_AZ_ADU_EXT[] = "AZ_ADU_EXT";

AzureIoTResult_t azure_adu_file_parse_url(AzureIoTADUUpdateManifestFileUrl_t *file_url,
                                          uint8_t *parse_buffer,
                                          parsed_file_url_t *parsed_url)
{
    // Skip the protocol prefix.
    uint8_t *url_start_index = file_url->pucUrl + sizeof("http://") - 1;

    // Find the first '/' after the url.
    const char *path_start_index = strstr((const char *)url_start_index, "/");

    if (path_start_index == NULL)
    {
        CMP_LOGE(TAG_AZ_ADU_EXT, "file url has no path");
        return eAzureIoTErrorInvalidArgument;
    }

    uint8_t *hostname = parse_buffer;

    // Extra space for the null-terminator.
    uint32_t hostname_length = (uint32_t)(path_start_index - (char *)url_start_index + 1);

    uint8_t *path = parse_buffer + hostname_length;

    // Discouting the host and its null-terminator,
    // and protocol prefix size, but adding space for
    // a null-terminator.
    uint32_t path_length = file_url->ulUrlLength - (hostname_length - 1) - (sizeof("http://") - 1) + 1;

    // Final memory layout:
    // parse_buffer = www.hostname.com\n/some/path\n
    //                ^hostname         ^path

    memcpy(hostname, url_start_index, hostname_length - 1);
    memset(hostname + hostname_length - 1, 0, 1);
    memcpy(path, path_start_index, path_length);
    memset(path + path_length - 1, 0, 1);

    parsed_url->hostname = hostname;
    parsed_url->hostname_length = hostname_length;
    parsed_url->path = path;
    parsed_url->path_length = path_length;

    return eAzureIoTSuccess;
}

AzureIoTResult_t azure_adu_file_download(parsed_file_url_t *parsed_url,
                                         uint8_t *data_buffer,
                                         uint32_t data_buffer_length,
                                         uint16_t chunck_size,
                                         azure_http_download_callback_t callback,
                                         void *callback_context,
                                         uint32_t *file_size)
{
    AzureIoTResult_t result = eAzureIoTErrorFailed;
    azure_http_context_t *http = azure_http_create((const char *)parsed_url->hostname,
                                                   parsed_url->hostname_length - 1,
                                                   (const char *)parsed_url->path,
                                                   parsed_url->path_length - 1);

    if (azure_http_connect(http) != eAzureIoTHTTPSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_EXT, "failure connecting to: %s", parsed_url->hostname);
        return eAzureIoTErrorFailed;
    }

    if (azure_http_get_resource_size(http, (char *)data_buffer, data_buffer_length, file_size) != eAzureIoTHTTPSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_EXT, "failure getting image size: %s", parsed_url->hostname);
    }
    else
    {
        result = azure_http_download_resource(http,
                                              (char *)data_buffer,
                                              data_buffer_length,
                                              chunck_size,
                                              callback,
                                              callback_context,
                                              *file_size) == eAzureIoTHTTPSuccess
                     ? eAzureIoTSuccess
                     : eAzureIoTErrorFailed;
    }

    azure_http_disconnect(http);
    azure_http_free(http);

    return result;
}