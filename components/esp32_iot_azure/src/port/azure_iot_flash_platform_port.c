#include <string.h>
#include "azure_iot_flash_platform.h"
#include "esp_system.h"
#include "esp_ota_ops.h"
#include "mbedtls/md.h"
#include "mbedtls/base64.h"
#include "assertion.h"
#include "log.h"

#define AZURE_IOT_SHA_256_SIZE 32

static const char TAG_FLASH_PORT[] = "AZ_FLASH_PORT";

static AzureIoTResult_t base64_decode(const uint8_t *encoded, size_t encoded_length, uint8_t *output_buffer, size_t output_buffer_length, size_t *bytes_written);
static AzureIoTResult_t image_calculate_hmac_256(const AzureADUImage_t *adu_image, uint8_t *output_buffer);

AzureIoTResult_t AzureIoTPlatform_Init(AzureADUImage_t *const pxAduImage)
{
    const esp_partition_t *current_partition = esp_ota_get_running_partition();

    CMP_CHECK(TAG_FLASH_PORT, (current_partition != NULL), "failure getting current OTA partition", eAzureIoTErrorFailed)

    pxAduImage->partition = esp_ota_get_next_update_partition(current_partition);
    pxAduImage->partition_offset = 0;
    pxAduImage->image_buffer = NULL;
    pxAduImage->image_buffer_length = 0;
    pxAduImage->image_size = 0;

    CMP_CHECK(TAG_FLASH_PORT, (pxAduImage->partition != NULL), "failure getting next OTA partition", eAzureIoTErrorFailed)
    CMP_CHECK(TAG_FLASH_PORT, (esp_partition_erase_range(pxAduImage->partition, 0, pxAduImage->partition->size) == ESP_OK), "failure erasing OTA partition", eAzureIoTErrorFailed)

    return eAzureIoTSuccess;
}

int64_t AzureIoTPlatform_GetSingleFlashBootBankSize()
{
    const esp_partition_t *current_partition = esp_ota_get_running_partition();

    CMP_CHECK(TAG_FLASH_PORT, (current_partition != NULL), "failure getting current OTA partition", -1)

    const esp_partition_t *next_partition = esp_ota_get_next_update_partition(current_partition);

    CMP_CHECK(TAG_FLASH_PORT, (next_partition != NULL), "failure getting current OTA partition", -1)

    return next_partition->size;
}

AzureIoTResult_t AzureIoTPlatform_WriteBlock(AzureADUImage_t *const pxAduImage,
                                             uint32_t offset,
                                             uint8_t *const pData,
                                             uint32_t ulBlockSize)
{
    CMP_CHECK(TAG_FLASH_PORT, (esp_partition_write(pxAduImage->partition, offset, pData, ulBlockSize) == ESP_OK), "failure writing", eAzureIoTErrorFailed)

    return eAzureIoTSuccess;
}

AzureIoTResult_t AzureIoTPlatform_VerifyImage(AzureADUImage_t *const pxAduImage,
                                              uint8_t *pucSHA256Hash,
                                              uint32_t ulSHA256HashLength)
{
    uint8_t decoded_hash[AZURE_IOT_SHA_256_SIZE];
    uint8_t calculated_hash[AZURE_IOT_SHA_256_SIZE];
    uint32_t base64_decoded_length;

    CMP_LOGI(TAG_FLASH_PORT, "base64 encoded hash from ADU: %.*s", ulSHA256HashLength, pucSHA256Hash);

    if (base64_decode(pucSHA256Hash, ulSHA256HashLength, decoded_hash, AZURE_IOT_SHA_256_SIZE, &base64_decoded_length))
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure decoding base64 SHA256");
        return eAzureIoTErrorFailed;
    }

    if (image_calculate_hmac_256(pxAduImage, calculated_hash))
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure calculating image hash");
        return eAzureIoTErrorFailed;
    }

    if (memcmp(decoded_hash, calculated_hash, AZURE_IOT_SHA_256_SIZE) == 0)
    {
        return eAzureIoTSuccess;
    }

    CMP_LOGE(TAG_FLASH_PORT, "failure verifying image: hashes does not match");
    CMP_LOGE(TAG_FLASH_PORT, "hash wanted: ");

    for (int i = 0; i < AZURE_IOT_SHA_256_SIZE; ++i)
    {
        CMP_LOGE(TAG_FLASH_PORT, "%x", decoded_hash[i]);
    }

    CMP_LOGI(TAG_FLASH_PORT, "hash calculated: ");

    for (int i = 0; i < AZURE_IOT_SHA_256_SIZE; ++i)
    {
        CMP_LOGE(TAG_FLASH_PORT, "%x", calculated_hash[i]);
    }

    return eAzureIoTErrorFailed;
}

AzureIoTResult_t AzureIoTPlatform_EnableImage(AzureADUImage_t *const pxAduImage)
{
    CMP_CHECK(TAG_FLASH_PORT, (esp_ota_set_boot_partition(pxAduImage->partition) == ESP_OK), "failure setting boot partition", eAzureIoTErrorFailed)

    return eAzureIoTSuccess;
}

__attribute__((noreturn)) AzureIoTResult_t AzureIoTPlatform_ResetDevice(AzureADUImage_t *const pxAduImage)
{
    // This functions restart the device, therefore never returning.
    esp_restart();
}

static AzureIoTResult_t image_calculate_hmac_256(const AzureADUImage_t *adu_image, uint8_t *output_buffer)
{
    AzureIoTResult_t result = eAzureIoTSuccess;
    uint8_t read_buffer[32];
    uint32_t size_read;
    mbedtls_md_context_t context;

    mbedtls_md_init(&context);
    mbedtls_md_setup(&context, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    mbedtls_md_starts(&context);

    for (size_t offset = 0; offset < adu_image->image_size; offset += sizeof(read_buffer))
    {
        if (adu_image->image_size - offset < sizeof(read_buffer))
        {
            size_read = adu_image->image_size - offset;
        }
        else
        {
            size_read = sizeof(read_buffer);
        }

        if (esp_partition_read_raw(adu_image->partition, offset, read_buffer, size_read) != ESP_OK)
        {
            CMP_LOGE(TAG_FLASH_PORT, "failure reading partition");

            result = eAzureIoTErrorFailed;

            break;
        }

        mbedtls_md_update(&context, (const unsigned char *)read_buffer, size_read);
    }

    mbedtls_md_finish(&context, output_buffer);
    mbedtls_md_free(&context);

    return result;
}

static AzureIoTResult_t base64_decode(const uint8_t *encoded,
                                      size_t encoded_length,
                                      uint8_t *output_buffer,
                                      size_t output_buffer_length,
                                      size_t *bytes_written)
{
    int32_t result = mbedtls_base64_decode(output_buffer, output_buffer_length, bytes_written, encoded, encoded_length);

    if (result != 0)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure decoding base64: %d", result);

        return eAzureIoTErrorFailed;
    }

    return eAzureIoTSuccess;
}