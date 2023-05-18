#include "esp32_iot_azure/extension/azure_iot_adu_extension.h"
#include "log.h"

static const char TAG_AZ_ADU_EXT[] = "AZ_ADU_EXT";

AzureIoTResult_t azure_adu_parse_file_url(AzureIoTADUUpdateManifestFileUrl_t *file_url,
                                          uint8_t *parse_buffer,
                                          uint8_t **hostname,
                                          uint32_t *hostname_length,
                                          uint8_t **path,
                                          uint32_t *path_length)
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

    *hostname = parse_buffer;

    // Extra space for the null-terminator.
    *hostname_length = (uint32_t)(path_start_index - (char *)url_start_index + 1);

    *path = parse_buffer + *hostname_length;

    // Discouting the host and its null-terminator,
    // and protocol prefix size, but adding space for
    // a null-terminator.
    *path_length = file_url->ulUrlLength - (*hostname_length - 1) - (sizeof("http://") - 1) + 1;

    // Final memory layout:
    // parse_buffer = www.hostname.com\n/some/path\n
    //                ^hostname         ^path

    memcpy(*hostname, url_start_index, *hostname_length - 1);
    memset(*hostname + *hostname_length - 1, 0, 1);
    memcpy(*path, path_start_index, *path_length);
    memset(*path + *path_length - 1, 0, 1);

    return eAzureIoTSuccess;
}