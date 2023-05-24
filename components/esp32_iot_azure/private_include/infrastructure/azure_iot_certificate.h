#ifndef __ESP32_IOT_AZURE_INFRA_AZURE_IOT_CERT_H__
#define __ESP32_IOT_AZURE_INFRA_AZURE_IOT_CERT_H__

#include "infrastructure/transport.h"

#ifdef __cplusplus
extern "C"
{
#endif

        /**
         * @brief Get the Azure IoT root certificate chain used by Azure IoT services.
         * @return Poiter to the certificate.
         */
        const tls_certificate_t *azure_iot_certificate_get();

#endif
#ifdef __cplusplus
}
#endif