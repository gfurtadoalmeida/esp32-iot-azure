#ifndef __ESP32_IOT_AZURE_COMMON_H__
#define __ESP32_IOT_AZURE_COMMON_H__

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
 * @note Does not include null termination.
 */
#define AZURE_CONST_HOSTNAME_MAX_LENGTH 128U

/**
 * @brief IoT device id max length.
 * @note Does not include null termination.
 */
#define AZURE_CONST_DEVICE_ID_MAX_LENGTH 128U

/**
 * @brief IoT device registration id max length.
 * @note Does not include null termination.
 */
#define AZURE_CONST_REGISTRATION_ID_MAX_LENGTH 128U

/**
 * @brief IoT device symmetric key max length.
 * @note Does not include null termination.
 */
#define AZURE_CONST_SYMMETRIC_KEY_MAX_LENGTH 128U

/**
 * @brief IoT device model id max length.
 * @note Does not include null termination.
 */
#define AZURE_CONST_MODEL_ID_MAX_LENGTH 128U

/**
 * @brief IoT device component name max length.
 * @note Does not include null termination.
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

    // ======
    // BUFFER
    // ======

    /**
     * @typedef buffer_t
     * @brief Represents a memory buffer.
     */
    typedef struct
    {
        uint32_t length; /** @brief Buffer length. */
        uint8_t *buffer; /** @brief Buffer bytes. */
    } buffer_t;

/**
 * @brief Create a @ref buffer_t from a literal.
 * @note The length will exclude the `NULL` terminator.
 * @param text Text literal.
 * @example buffer_t buffer = BUFFER_FROM_CONST("my_value");
 */
#define BUFFER_FROM_LITERAL(text)   \
    {                               \
        .length = sizeof(text) - 1, \
        .buffer = (uint8_t *)text   \
    }

/**
 * @brief Create a @ref buffer_t from a fixed size array.
 * @param fixed_size_array Fixed size uint8_t array.
 * @example
 * uint8_t array[50] = {0};
 * buffer_t buffer = BUFFER_WITH_FIXED_LENGTH(array);
 */
#define BUFFER_FROM_FIXED_ARRAY(fixed_size_array) \
    {                                             \
        .length = sizeof(fixed_size_array),       \
        .buffer = fixed_size_array                \
    }

#ifndef __cplusplus
// C++ compound literal semantic differs from C,
// giving undefined behavior, besides not being
// accepted by GCC; only enable for C code.

/**
 * @brief Create a @ref buffer_t with a fixed size.
 * @param buffer_length Length of the buffer being allocated on stack.
 * @example buffer_t buffer = BUFFER_WITH_FIXED_LENGTH(128);
 */
#define BUFFER_WITH_FIXED_LENGTH(buffer_length)  \
    {                                            \
        .length = buffer_length,                 \
        .buffer = (uint8_t[buffer_length]) { 0 } \
    }
#endif

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

/**
 * @brief Return the size of a literal, excluding the `NULL` terminator.
 * @param text Text value.
 * @return Text length excluding the `NULL` terminator.
 */
#define sizeof_l(text) (sizeof(text) - 1)

#ifdef __cplusplus
}
#endif
#endif