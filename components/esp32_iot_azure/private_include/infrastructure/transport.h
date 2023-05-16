#ifndef __ESP32_IOT_AZURE_INFRA_TRANSPORT_H__
#define __ESP32_IOT_AZURE_INFRA_TRANSPORT_H__

#include <stdint.h>
#include "esp_transport.h"
#include "esp32_iot_azure/azure_iot_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Certificate format for @ref tls_certificate_t.
     */
    typedef enum
    {
        TLS_CERT_FORMAT_PEM = 0,
        TLS_CERT_FORMAT_DER = 1
    } tls_certificate_format_t;

    /**
     * @brief Represents a TLS certificate.
     */
    typedef struct
    {
        const uint8_t *data;             /** @brief Certificate bytes. */
        tls_certificate_format_t format; /** @brief Certificate format. */
        uint16_t length;                 /** @brief Certificate bytes length. */
    } tls_certificate_t;

    /**
     * @brief Status for transport @ref transport_connect.
     */
    typedef enum
    {
        TRANSPORT_STATUS_SUCCESS = 0,
        TRANSPORT_STATUS_FAILURE = 1
    } transport_status_t;

    /**
     * @brief Creates a raw TCP transport.
     * @note The transport handle must be released by @ref transport_free.
     * @return @ref esp_transport_handle_t on success or null on failure.
     */
    esp_transport_handle_t transport_create_tcp();

    /**
     * @brief Creates a TLS transport.
     * @note The transport handle must be released by @ref transport_free.
     * @param[in] certificate Server certificate.
     * @return @ref esp_transport_handle_t on success or null on failure.
     */
    esp_transport_handle_t transport_create_tls(const tls_certificate_t *certificate);

    /**
     * @brief Creates a TLS transport configured with Azure IoT root certificate.
     * @note The transport handle must be released by @ref transport_free.
     * @return @ref esp_transport_handle_t on success or null on failure.
     */
    esp_transport_handle_t transport_create_azure();

    /**
     * @brief Configures a TLS transport with a client certificate.
     * @note The transport must be a TLS one.
     * @param[in] transport Transport handle.
     * @param[in] certificate Client certificate.
     */
    transport_status_t transport_set_client_certificate(esp_transport_handle_t transport,
                                                        const client_certificate_t *certificate);

    /**
     * @brief Establish a connection to a server.
     * @note The connection must be closed by @ref transport_disconnect.
     * @param[in] transport Transport handle.
     * @param[in] hostname Server address. Must be null-terminated.
     * @param[in] port Server port.
     * @param[in] timeout_ms Connection timeout in milliseconds.
     * @return @ref transport_status_t with the result of the operation.
     */
    transport_status_t transport_connect(esp_transport_handle_t transport,
                                         const char *hostname,
                                         uint16_t port,
                                         uint16_t timeout_ms);
    /**
     * @brief Write bytes to a transport.
     * @param[in] transport Transport handle.
     * @param[in] buffer Data to write.
     * @param[in] length How many bytes to write.
     * @param[in] timeout_ms Timeout in milliseconds (-1 indicates wait forever)
     * @return Number of bytes written or (-1) if there are any errors.
     */
    int32_t transport_write(esp_transport_handle_t transport,
                            const uint8_t *buffer,
                            size_t length,
                            uint16_t timeout_ms);

    /**
     * @brief Read bytes from a transport.
     * @param[in] transport Transport handle.
     * @param[out] buffer Buffer to receive data.
     * @param[in] expected_length Maximum number of bytes to read.
     * @param[in] timeout_ms Timeout in milliseconds (-1 indicates wait forever)
     * @return Number of bytes read or (-1) if there are any errors.
     */
    int32_t transport_read(esp_transport_handle_t transport,
                           uint8_t *buffer,
                           size_t expected_length,
                           uint16_t timeout_ms);

    /**
     * @brief Close a connection to the server.
     * @param[in] transport Transport handle.
     */
    void transport_disconnect(esp_transport_handle_t transport);

    /**
     * @brief Cleanup and free the transport memory.
     * @param[in] transport Transport handle.
     */
    void transport_free(esp_transport_handle_t transport);
#endif
#ifdef __cplusplus
}
#endif