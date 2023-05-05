#include "infrastructure/crypto.h"
#include "mbedtls/md.h"
#include "assertion.h"

static const char TAG_CRYPTO[] = "AZ_CRYPTO";

uint32_t crypto_hash_hmac_256(const uint8_t *key,
                              uint32_t key_length,
                              const uint8_t *data,
                              uint32_t data_length,
                              uint8_t *output_buffer,
                              uint32_t output_buffer_length,
                              uint32_t *bytes_copied)
{
    CMP_CHECK(TAG_CRYPTO, (output_buffer_length > 32), "invalid output buffer length", 1)

    mbedtls_md_context_t context;

    mbedtls_md_init(&context);

    int mbedtls_result = mbedtls_md_setup(&context, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1)
                         || mbedtls_md_hmac_starts(&context, key, key_length)
                         || mbedtls_md_hmac_update(&context, data, data_length)
                         || mbedtls_md_hmac_finish(&context, output_buffer);

    uint32_t result;

    if (mbedtls_result)
    {
        CMP_LOGE(TAG_CRYPTO, "failure hashing: %d", mbedtls_result);

        result = 1U;
        *bytes_copied = 0U;
    }
    else
    {
        result = 0U;
        *bytes_copied = 32U;
    }

    mbedtls_md_free(&context);

    return result;
}