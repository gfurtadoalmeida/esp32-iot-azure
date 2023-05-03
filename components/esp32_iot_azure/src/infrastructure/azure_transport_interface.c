#include "infrastructure/azure_transport_interface.h"
#include <stdint.h>
#include "config.h"

struct NetworkContext
{
    esp_transport_handle_t transport;
};

static int32_t azure_transport_send(struct NetworkContext *pxNetworkContext,
                                    const void *pvBuffer,
                                    size_t xBytesToSend)
{
    return transport_write(pxNetworkContext->transport,
                           (const uint8_t *)pvBuffer,
                           xBytesToSend,
                           CONFIG_ESP32_IOT_AZURE_TRANSPORT_SEND_TIMEOUT_MS);
}

static int32_t azure_transport_receive(struct NetworkContext *pxNetworkContext,
                                       void *pvBuffer,
                                       size_t xBytesToRecv)
{
    return transport_read(pxNetworkContext->transport,
                          (uint8_t *)pvBuffer,
                          xBytesToRecv,
                          CONFIG_ESP32_IOT_AZURE_TRANSPORT_RECEIVE_TIMEOUT_MS);
}

void azure_transport_interface_init(esp_transport_handle_t transport,
                                    AzureIoTTransportInterface_t *interface)
{
    interface->pxNetworkContext = (struct NetworkContext *)malloc(sizeof(struct NetworkContext));
    interface->pxNetworkContext->transport = transport;
    interface->xSend = azure_transport_send;
    interface->xRecv = azure_transport_receive;
}

void azure_transport_interface_free(AzureIoTTransportInterface_t *interface)
{
    free(interface->pxNetworkContext);
}