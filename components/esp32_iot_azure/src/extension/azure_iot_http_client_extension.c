#include "esp32_iot_azure/extension/azure_iot_http_client_extension.h"
#include "log.h"
#include "config.h"

static const char TAG_AZ_HTTP_EXT[] = "AZ_HTTP_EXT";

AzureIoTHTTPResult_t azure_http_get_resource_size(azure_http_context_t *context,
                                                  char *data_buffer,
                                                  uint32_t data_buffer_length,
                                                  uint32_t *resource_size)
{
    AzureIoTHTTPResult_t http_result = eAzureIoTHTTPSuccess;

    if ((http_result = azure_http_request_size_init(context,
                                                    data_buffer,
                                                    data_buffer_length)) != eAzureIoTHTTPSuccess)
    {
        CMP_LOGE(TAG_AZ_HTTP_EXT, "failure initializing request size: %d", http_result);
        return http_result;
    }

    int32_t total_size = 0;

    if ((total_size = azure_http_request_size(context, data_buffer, data_buffer_length)) == -1)
    {
        CMP_LOGE(TAG_AZ_HTTP_EXT, "failure getting Content-Length header");
        return eAzureIoTHTTPError;
    }

    *resource_size = (uint32_t)total_size;

    return eAzureIoTHTTPSuccess;
}

AzureIoTHTTPResult_t azure_http_download_resource(azure_http_context_t *context,
                                                  char *data_buffer,
                                                  uint32_t data_buffer_length,
                                                  uint16_t chunk_size,
                                                  azure_http_download_callback_t callback,
                                                  void *callback_context,
                                                  uint32_t resource_size)
{
    AzureIoTHTTPResult_t http_result = eAzureIoTHTTPSuccess;
    uint32_t current_offset = 0;
    char *data_buffer_payload_pointer = NULL;
    uint32_t data_buffer_payload_length = 0;

    while (current_offset < resource_size)
    {
        if ((http_result = azure_http_init(context, data_buffer, data_buffer_length)) != eAzureIoTHTTPSuccess)
        {
            CMP_LOGE(TAG_AZ_HTTP_EXT, "failure initializing request: %d", http_result);
            return http_result;
        }

        http_result = azure_http_request(context,
                                         current_offset,
                                         current_offset + chunk_size - 1,
                                         data_buffer,
                                         data_buffer_length,
                                         &data_buffer_payload_pointer,
                                         &data_buffer_payload_length);

        if (http_result == eAzureIoTHTTPSuccess)
        {
            if (callback != NULL && !callback((uint8_t *)data_buffer_payload_pointer,
                                              data_buffer_payload_length,
                                              current_offset,
                                              resource_size,
                                              callback_context))
            {
                CMP_LOGE(TAG_AZ_HTTP_EXT, "failure calling donwload callback");
                return eAzureIoTHTTPError;
            }

            current_offset += data_buffer_payload_length;
        }
        else if (http_result == eAzureIoTHTTPPartialResponse || http_result == eAzureIoTHTTPNoResponse || http_result == eAzureIoTHTTPNetworkError)
        {
            CMP_LOGW(TAG_AZ_HTTP_EXT, "reconnecting");

            azure_http_disconnect(context);

            if ((http_result = azure_http_connect(context)) != eAzureIoTHTTPSuccess)
            {
                CMP_LOGE(TAG_AZ_HTTP_EXT, "failure reconnecting");
                return http_result;
            }
        }
        else
        {
            CMP_LOGE(TAG_AZ_HTTP_EXT, "failure sending request: %d", http_result);
            return http_result;
        }

        azure_http_deinit(context);
    }

    return http_result;
}