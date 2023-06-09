#ifndef __ESP32_IOT_AZURE_INFRA_AZURE_ROOT_KEY_H__
#define __ESP32_IOT_AZURE_INFRA_AZURE_ROOT_KEY_H__

#include "azure_iot_jws.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef azure_jws_root_keys_t
     * @brief Azure Device Update JWS root keys.
     */
    typedef struct
    {
        AzureIoTJWS_RootKey_t *keys; /** @brief Pointer to an array of @ref AzureIoTJWS_RootKey_t. */
        uint8_t keys_count;          /** @brief Number of keys in the array. */
    } azure_jws_root_keys_t;

    /**
     * @brief Get the Azure Device Update root keys used to verify
     * the update JWS signature.
     * @return Pointer to the root keys.
     */
    const azure_jws_root_keys_t *azure_adu_root_key_get();

#endif
#ifdef __cplusplus
}
#endif