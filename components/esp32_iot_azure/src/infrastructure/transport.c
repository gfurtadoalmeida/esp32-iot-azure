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

struct transport_t
{
    esp_transport_handle_t handle; /** @brief ESP transport handle. */
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
    uint16_t next_backoff_ms = 0U;
    transport_status_t transport_status = TRANSPORT_STATUS_FAILURE;
    backoff_algorithm_status_t backoff_status = BACKOFF_ALGORITHM_SUCCESS;
    backoff_algorithm_context_t backoff_context;

    backoff_algorithm_initialize(&backoff_context,
                                 CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_BASE_MS,
                                 CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_MAX_DELAY_MS,
                                 CONFIG_ESP32_IOT_AZURE_TRANSPORT_BACKOFF_RETRY_MAX_ATTEMPTS);
    do
    {
        if (esp_transport_connect(transport->handle, hostname, port, timeout_ms) != 0)
        {
            CMP_LOGW(TAG_TRANSPORT, "failure connecting to %s on %d: %d", hostname, port, esp_transport_get_errno(transport->handle));

            backoff_status = backoff_algorithm_get_next(&backoff_context, &next_backoff_ms);

            if (backoff_status == BACKOFF_ALGORITHM_SUCCESS)
            {
                CMP_LOGW(TAG_TRANSPORT, "will retry in %d ms", next_backoff_ms);

                vTaskDelay(pdMS_TO_TICKS(next_backoff_ms));
            }
        }
        else
        {
            transport_status = TRANSPORT_STATUS_SUCCESS;
        }
    } while (transport_status != TRANSPORT_STATUS_SUCCESS && backoff_status == BACKOFF_ALGORITHM_SUCCESS);

    return transport_status;
}

int32_t transport_write(transport_t *transport,
                        const uint8_t *buffer,
                        size_t length,
                        uint16_t timeout_ms)
{
    int result = esp_transport_write(transport->handle, (const char *)buffer, length, timeout_ms);

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