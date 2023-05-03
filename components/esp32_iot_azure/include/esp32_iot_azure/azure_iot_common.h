#ifndef __ESP32_IOT_AZURE_STRUCTURES_H__
#define __ESP32_IOT_AZURE_STRUCTURES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    // ==============
    // SIZE CONSTANTS
    // ==============

/**
 * @brief IoT Hub hostname max length.
 */
#define AZURE_CONST_HOSTNAME_MAX_LENGTH 128U

/**
 * @brief IoT device id max length.
 */
#define AZURE_CONST_DEVICE_ID_MAX_LENGTH 128U

/**
 * @brief IoT device registration id max length.
 */
#define AZURE_CONST_REGISTRATION_ID_MAX_LENGTH 128U

/**
 * @brief IoT device symmetric key max length.
 */
#define AZURE_CONST_SYMMETRIC_KEY_MAX_LENGTH 128U

/**
 * @brief IoT device model id max length.
 */
#define AZURE_CONST_MODEL_ID_MAX_LENGTH 128U

/**
 * @brief IoT device component name max length.
 */
#define AZURE_CONST_COMPONENT_NAME_MAX_LENGTH 64U

    // ===========
    // AZURE CHECK
    // ===========

/**
 * @brief Variable used to hold AZ_CHECK* results.
 */
#define AZ_CHECK_RESULT_VAR azure_result

/**
 * @brief Start a simple check sequence on Azure IoT results.
 */
#define AZ_CHECK_BEGIN() AzureIoTResult_t AZ_CHECK_RESULT_VAR = eAzureIoTErrorFailed;

/**
 * @brief Check if a \p operation returns @ref eAzureIoTSuccess, returning
 * from the calling function if the result is not @ref eAzureIoTSuccess.
 */
#define AZ_CHECK(operation)                             \
    if ((azure_result = operation) != eAzureIoTSuccess) \
    {                                                   \
        return azure_result;                            \
    }

/**
 * @brief Return from the calling function with the last result
 * gotten from @ref AZ_CHECK.
 */
#define AZ_CHECK_RETURN_LAST() return azure_result;

    // ============
    // UTF-8 STRING
    // ============

/**
 * @brief Create an @ref utf8_string_t from a literal.
 * @note The length will exclude the `NULL` terminator.
 * @param text Text literal.
 * @example utf8_string_t string = UTF8_STRING_FROM_CONST("my_value");
 */
#define UTF8_STRING_FROM_LITERAL(text) \
    {                                  \
        .length = sizeof(text) - 1,    \
        .buffer = (uint8_t *)text      \
    }

/**
 * @brief Create an @ref utf8_string_t with a fixed size buffer.
 * @param buffer_length Length of the buffer being allocated on stack.
 * @example utf8_string_t string = UTF8_STRING_WITH_FIXED_LENGTH(128);
 */
#define UTF8_STRING_WITH_FIXED_LENGTH(buffer_length) \
    {                                                \
        .length = buffer_length,                     \
        .buffer = (uint8_t[buffer_length])           \
        {                                            \
        }                                            \
    }

    /**
     * @typedef utf8_string_t
     * @brief Represents a UTF-8 encoded string.
     */
    typedef struct
    {
        uint32_t length; /** @brief String length, excluding the `NULL` terminator. */
        uint8_t *buffer; /** @brief UTF-8 string characters. */
    } utf8_string_t;

    /**
     * @brief Calculate the length of a string represented
     * as a sequence of @ref uint8_t characters.
     * @param[in] buffer Pointer to a buffer holding the string.
     * @return String length.
     */
    uint32_t strlen_uint8_t(const uint8_t *buffer);

    // ==================
    // CLIENT CERTIFICATE
    // ==================

    /**
     * @brief Client certificate format for @ref client_certificate_t.
     */
    typedef enum
    {
        CLIENT_CERT_FORMAT_PEM = 0,
        CLIENT_CERT_FORMAT_DER = 1
    } client_certificate_format_t;

    /**
     * @typedef client_certificate_t
     * @brief Represents a client certificate.
     */
    typedef struct
    {
        const uint8_t *data;                /** @brief Certificate bytes. */
        const uint8_t *private_key;         /** @brief Certificate private key bytes. */
        uint16_t data_length;               /** @brief Certificate bytes length. */
        uint16_t private_key_length;        /** @brief Certificate private key bytes length. */
        client_certificate_format_t format; /** @brief Certificate format. */
    } client_certificate_t;

#ifdef __cplusplus
}
#endif
#endif