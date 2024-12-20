#include <stdbool.h>
#include "infrastructure/transport.h"
#include "infrastructure/backoff_algorithm.h"
#include "infrastructure/azure_iot_certificate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_transport.h"
#include "esp_transport_tcp.h"
#include "esp_transport_ssl.h"
#include "log.h"
#include "config.h"

static const char TAG_TRANSPORT[] = "AZ_TRANSPORT";

static transport_status_t transport_reconnect(transport_t *transport);
static bool should_try_reconnection(int error_num);

struct transport_t
{
    esp_transport_handle_t handle; /** @brief ESP transport handle. */
    const char *hostname;          /** @brief Server address. Must be null-terminated. */
    uint16_t port;                 /** @brief Server port */
    uint16_t timeout_ms;           /** @brief Connection timeout in milliseconds. */
};

transport_t *transport_create_tcp()
{
    transport_t *transport = (transport_t *)malloc(sizeof(transport_t));
    transport->handle = esp_transport_tcp_init();

    return transport;
}

transport_t *transport_create_tls(const tls_certificate_t *certificate)
{
    transport_t *transport = (transport_t *)malloc(sizeof(transport_t));
    transport->handle = esp_transport_ssl_init();

    switch (certificate->format)
    {
    case TLS_CERT_FORMAT_PEM:
        esp_transport_ssl_set_cert_data(transport->handle,
                                        (const char *)certificate->data,
                                        certificate->length);
        break;

    case TLS_CERT_FORMAT_DER:
        esp_transport_ssl_set_cert_data_der(transport->handle,
                                            (const char *)certificate->data,
                                            certificate->length);
        break;

    default:
        CMP_LOGE(TAG_TRANSPORT, "invalid certificate format: %d", certificate->format);
        break;
    }

    return transport;
}

transport_t *transport_create_azure()
{
    return transport_create_tls(azure_iot_certificate_get());
}

transport_status_t transport_set_client_certificate(transport_t *transport,
                                                    const client_certificate_t *certificate)
{
    transport_status_t result = TRANSPORT_STATUS_SUCCESS;

    switch (certificate->format)
    {
    case CLIENT_CERT_FORMAT_PEM:
        esp_transport_ssl_set_client_cert_data(transport->handle,
                                               (const char *)certificate->data,
                                               certificate->data_length);
        esp_transport_ssl_set_client_key_data(transport->handle,
                                              (const char *)certificate->private_key,
                                              certificate->private_key_length);
        break;

    case CLIENT_CERT_FORMAT_DER:
        esp_transport_ssl_set_client_cert_data_der(transport->handle,
                                                   (const char *)certificate->data,
                                                   certificate->data_length);
        esp_transport_ssl_set_client_key_data_der(transport->handle,
                                                  (const char *)certificate->private_key,
                                                  certificate->private_key_length);
        break;

    default:
        result = TRANSPORT_STATUS_FAILURE;
        CMP_LOGE(TAG_TRANSPORT, "invalid certificate format: %d", certificate->format);
        break;
    }

    return result;
}

transport_status_t transport_connect(transport_t *transport,
                                     const char *hostname,
                                     uint16_t port,
                                     uint16_t timeout_ms)
{
    transport->hostname = hostname;
    transport->port = port;
    transport->timeout_ms = timeout_ms;

    return transport_reconnect(transport);
}

int32_t transport_write(transport_t *transport,
                        const uint8_t *buffer,
                        size_t length,
                        uint16_t timeout_ms)
{
    int result = esp_transport_write(transport->handle, (const char *)buffer, length, timeout_ms);

    if (result > -1)
    {
        return result;
    }

    if (should_try_reconnection(esp_transport_get_errno(transport->handle)))
    {
        transport_reconnect(transport);

        result = esp_transport_write(transport->handle, (const char *)buffer, length, timeout_ms);
    }

    if (result < 0)
    {
        CMP_LOGE(TAG_TRANSPORT, "failure writing: %d", esp_transport_get_errno(transport->handle));
    }

    return result;
}

int32_t transport_read(transport_t *transport,
                       uint8_t *buffer,
                       size_t expected_length,
                       uint16_t timeout_ms)
{
    int result = esp_transport_read(transport->handle, (char *)buffer, expected_length, timeout_ms);

    if (result > -1)
    {
        return result;
    }

    if (should_try_reconnection(esp_transport_get_errno(transport->handle)))
    {
        transport_reconnect(transport);

        result = esp_transport_read(transport->handle, (char *)buffer, expected_length, timeout_ms);
    }

    if (result < 0)
    {
        CMP_LOGE(TAG_TRANSPORT, "failure reading: %d", esp_transport_get_errno(transport->handle));
    }

    return result;
}

void transport_disconnect(transport_t *transport)
{
    if (esp_transport_close(transport->handle) < 0)
    {
        CMP_LOGE(TAG_TRANSPORT, "failure disconnecting: %d", esp_transport_get_errno(transport->handle));
    }
}

void transport_free(transport_t *transport)
{
    esp_transport_destroy(transport->handle);

    free(transport);
}

static transport_status_t transport_reconnect(transport_t *transport)
{
    uint16_t next_backoff_ms = 0U;
    transport_status_t transport_status = TRANSPORT_STATUS_FAILURE;
    backoff_algorithm_status_t backoff_status = BACKOFF_ALGORITHM_SUCCESS;
    backoff_algorithm_context_t backoff_context;

    transport_disconnect(transport);

    backoff_algorithm_initialize(&backoff_context,
                                 CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_BASE_MS,
                                 CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_MAX_DELAY_MS,
                                 CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_RETRY_MAX_ATTEMPTS);
    do
    {
        CMP_LOGI(TAG_TRANSPORT, "connecting to '%s' on %d", transport->hostname, transport->port);

        if (esp_transport_connect(transport->handle, transport->hostname, transport->port, transport->timeout_ms) != 0)
        {
            CMP_LOGW(TAG_TRANSPORT, "failure connecting: %d", esp_transport_get_errno(transport->handle));

            backoff_status = backoff_algorithm_get_next(&backoff_context, &next_backoff_ms);

            if (backoff_status == BACKOFF_ALGORITHM_SUCCESS)
            {
                CMP_LOGW(TAG_TRANSPORT, "will retry in %d ms", next_backoff_ms);

                vTaskDelay(pdMS_TO_TICKS(next_backoff_ms));
            }
        }
        else
        {
            CMP_LOGI(TAG_TRANSPORT, "connected");

            transport_status = TRANSPORT_STATUS_SUCCESS;
        }
    } while (transport_status != TRANSPORT_STATUS_SUCCESS && backoff_status == BACKOFF_ALGORITHM_SUCCESS);

    return transport_status;
}

static bool should_try_reconnection(int error_num)
{
    switch (error_num)
    {
    case EPIPE:        // Writing to a closed connection.
    case ECONNRESET:   // The connection was closed by the remote peer.
    case EADDRINUSE:   // The local address is in use. Might occur if trying to reconnect too quickly. Wait before retrying.
    case ENETUNREACH:  // The local network interface is unavailable. Reconnection might not succeed until the network is restored.
    case ETIMEDOUT:    // The connection attempt or existing connection timed out.
    case EHOSTUNREACH: // The remote host is unreachable. Reconnect after a delay or check network availability.
    case ENOTCONN:     // The transport is no longer connected.
        return true;
    default:
        return false;
    }
}
