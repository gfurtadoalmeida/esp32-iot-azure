#ifndef __ESP32_IOT_AZURE_INFRA_AZURE_TRANSPORT_INTERFACEH__
#define __ESP32_IOT_AZURE_INFRA_AZURE_TRANSPORT_INTERFACEH__

#include "infrastructure/transport.h"
#include "azure_iot_transport_interface.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Create an @ref AzureIoTTransportInterface_t that
     * uses @ref esp_transport_handle_t to send and receive data.
     * @note The interface must be released by @ref azure_transport_interface_free.
     * @param[in] transport Transport that will be used.
     * @param[out] interface Azure transport interface to be configured.
     */
    void azure_transport_interface_init(esp_transport_handle_t transport,
                                        AzureIoTTransportInterface_t *interface);

    /**
     * @brief Cleanup and free the interface.
     * @param[out] interface Azure transport interface.
     */
    void azure_transport_interface_free(AzureIoTTransportInterface_t *interface);

#endif
#ifdef __cplusplus
}
#endif