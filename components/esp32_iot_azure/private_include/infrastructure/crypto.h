#ifndef __ESP32_IOT_AZURE_INFRA_CRYPTO_H__
#define __ESP32_IOT_AZURE_INFRA_CRYPTO_H__

// Reference: https://github.com/Azure-Samples/iot-middleware-freertos-samples/blob/main/demos/projects/ESPRESSIF/adu/components/sample-azure-iot/crypto_esp32.c

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief HMAC256 function that complies with @ref AzureIoTGetHMACFunc_t contract.
     * @param[in] key The key to use for the HMAC operation.
     * @param[in] key_length The length of \p key.
     * @param[in] data The data on which the operation will take place.
     * @param[in] data_length The length of \p data.
     * @param[out] output_buffer The buffer into which the processed data will be placed.
     * @param[in] output_buffer_length The size of \p output_buffer.
     * @param[out] bytes_copied The number of bytes copied into \p output_buffer.
     * @return 0 on success, 1 on failure.
     * @link https://en.wikipedia.org/wiki/HMAC
     */
    uint32_t crypto_hmac_256(const uint8_t *key,
                             uint32_t key_length,
                             const uint8_t *data,
                             uint32_t data_length,
                             uint8_t *output_buffer,
                             uint32_t output_buffer_length,
                             uint32_t *bytes_copied);
#endif
#ifdef __cplusplus
}
#endif