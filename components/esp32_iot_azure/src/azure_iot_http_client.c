#include <string.h>
#include "esp32_iot_azure/azure_iot_http_client.h"
#include "infrastructure/transport.h"
#include "infrastructure/azure_transport_interface.h"
#include "config.h"

struct azure_http_context_t
{
    AzureIoTTransportInterface_t transport_interface;
    AzureIoTHTTP_t http;
    transport_t *transport;
    const char *url;
    const char *path;
    uint32_t url_length;
    uint32_t path_length;
};

azure_http_context_t *azure_http_create(const char *url,
                                        uint32_t url_length,
                                        const char *path,
                                        uint32_t path_length)
{
    azure_http_context_t *context = (azure_http_context_t *)malloc(sizeof(azure_http_context_t));

    memset(context, 0, sizeof(azure_http_context_t));

    context->url = url;
    context->url_length = url_length;
    context->path = path;
    context->path_length = path_length;
    context->transport = transport_create_tcp();

    azure_transport_interface_init(context->transport, &context->transport_interface);

    return context;
}

AzureIoTHTTPResult_t azure_http_connect(azure_http_context_t *context)
{
    if (transport_connect(context->transport,
                          context->url,
                          80,
                          CONFIG_ESP32_IOT_AZURE_TRANSPORT_HTTP_CONNECT_TIMEOUT_MS) == TRANSPORT_STATUS_SUCCESS)
    {
        return eAzureIoTHTTPSuccess;
    }

    return eAzureIoTHTTPError;
}

void azure_http_disconnect(azure_http_context_t *context)
{
    transport_disconnect(context->transport);
}

AzureIoTHTTPResult_t azure_http_request_size_init(azure_http_context_t *context,
                                                  char *header_buffer,
                                                  uint32_t header_buffer_length)
{
    return AzureIoTHTTP_RequestSizeInit(&context->http,
                                        &context->transport_interface,
                                        context->url,
                                        context->url_length,
                                        context->path,
                                        context->path_length,
                                        header_buffer,
                                        header_buffer_length);
}

int32_t azure_http_request_size(azure_http_context_t *context,
                                char *data_buffer,
                                uint32_t data_buffer_length)
{
    return AzureIoTHTTP_RequestSize(&context->http,
                                    data_buffer,
                                    data_buffer_length);
}

AzureIoTHTTPResult_t azure_http_init(azure_http_context_t *context,
                                     char *header_buffer,
                                     uint32_t header_buffer_length)
{
    return AzureIoTHTTP_Init(&context->http,
                             &context->transport_interface,
                             context->url,
                             context->url_length,
                             context->path,
                             context->path_length,
                             header_buffer,
                             header_buffer_length);
}

AzureIoTHTTPResult_t azure_http_request(azure_http_context_t *context,
                                        int32_t range_start,
                                        int32_t range_end,
                                        char *data_buffer,
                                        uint32_t data_buffer_length,
                                        char **output_data,
                                        uint32_t *output_data_length)
{
    return AzureIoTHTTP_Request(&context->http,
                                range_start,
                                range_end,
                                data_buffer,
                                data_buffer_length,
                                output_data,
                                output_data_length);
}

AzureIoTHTTPResult_t azure_http_deinit(azure_http_context_t *context)
{
    return AzureIoTHTTP_Deinit(&context->http);
}

void azure_http_free(azure_http_context_t *context)
{
    azure_transport_interface_free(&context->transport_interface);

    transport_free(context->transport);

    free(context);
}