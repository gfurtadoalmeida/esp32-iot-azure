#ifndef __ESP32_IOT_AZURE_INFRA_AZURE_IOT_CERT_H__
#define __ESP32_IOT_AZURE_INFRA_AZURE_IOT_CERT_H__

#include "infrastructure/transport.h"

#ifdef __cplusplus
extern "C"
{
#endif

        /** @brief Azure IoT root certificate chain. */
        extern const tls_certificate_t *AZURE_IOT_CERTIFICATE;

#endif
#ifdef __cplusplus
}
#endif