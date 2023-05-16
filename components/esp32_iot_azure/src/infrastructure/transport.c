#include "infrastructure/transport.h"
#include "infrastructure/backoff_algorithm.h"
#include "infrastructure/azure_iot_certificate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_transport_ssl.h"
#include "log.h"
#include "config.h"

static const char TAG_TRANSPORT[] = "AZ_TRANSPORT";

esp_transport_handle_t transport_create()
{
    return esp_transport_init();
}

esp_transport_handle_t transport_create_tls(const tls_certificate_t *certificate)
{
    esp_transport_handle_t transport = esp_transport_ssl_init();

    switch (certificate->format)
    {
    case TLS_CERT_FORMAT_PEM:
        esp_transport_ssl_set_cert_data(transport,
                                        (const char *)certificate->data,
                                        certificate->length);
        break;

    case TLS_CERT_FORMAT_DER:
        esp_transport_ssl_set_cert_data_der(transport,
                                            (const char *)certificate->data,
                                            certificate->length);
        break;

    default:
        CMP_LOGE(TAG_TRANSPORT, "invalid certificate format: %d", certificate->format);
        break;
    }

    return transport;
}

esp_transport_handle_t transport_create_azure()
{
    return transport_create_tls(AZURE_IOT_CERTIFICATE);
}

transport_status_t transport_set_client_certificate(esp_transport_handle_t transport,
                                                    const client_certificate_t *certificate)
{
    transport_status_t result = TRANSPORT_STATUS_SUCCESS;

    switch (certificate->format)
    {
    case CLIENT_CERT_FORMAT_PEM:
        esp_transport_ssl_set_client_cert_data(transport,
                                               (const char *)certificate->data,
                                               certificate->data_length);
        esp_transport_ssl_set_client_key_data(transport,
                                              (const char *)certificate->private_key,
                                              certificate->private_key_length);
        break;

    case CLIENT_CERT_FORMAT_DER:
        esp_transport_ssl_set_client_cert_data_der(transport,
                                                   (const char *)certificate->data,
                                                   certificate->data_length);
        esp_transport_ssl_set_client_key_data_der(transport,
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

transport_status_t transport_connect(esp_transport_handle_t transport,
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
        if (esp_transport_connect(transport, hostname, port, timeout_ms) == -1)
        {
            CMP_LOGW(TAG_TRANSPORT, "failure connecting to %s on %d: %d", hostname, port, esp_transport_get_errno(transport));

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

int32_t transport_write(esp_transport_handle_t transport,
                        const uint8_t *buffer,
                        size_t length,
                        uint16_t timeout_ms)
{
    int32_t result = esp_transport_write(transport, (const char *)buffer, length, timeout_ms);

    if (result == -1)
    {
        CMP_LOGE(TAG_TRANSPORT, "failure writing: %d", esp_transport_get_errno(transport));
    }

    return result;
}

int32_t transport_read(esp_transport_handle_t transport,
                       uint8_t *buffer,
                       size_t expected_length,
                       uint16_t timeout_ms)
{
    int32_t result = esp_transport_read(transport, (char *)buffer, expected_length, timeout_ms);

    if (result == -1)
    {
        CMP_LOGE(TAG_TRANSPORT, "failure reading: %d", esp_transport_get_errno(transport));
    }

    return result;
}

void transport_disconnect(esp_transport_handle_t transport)
{
    esp_transport_close(transport);
}

void transport_free(esp_transport_handle_t transport)
{
    esp_transport_destroy(transport);
}