#include <string.h>
#include "azure_iot_flash_platform.h"
#include "esp_system.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "assertion.h"
#include "log.h"

#define AZURE_IOT_SHA_256_SIZE 32

static const char TAG_FLASH_PORT[] = "AZ_FLASH_PORT";

static AzureIoTResult_t base64_decode(const uint8_t *encoded, size_t encoded_length, uint8_t *output_buffer, size_t output_buffer_length, size_t *bytes_written);
static AzureIoTResult_t image_calculate_hmac_256(const AzureADUImage_t *adu_image, uint8_t *output_buffer);

int64_t AzureIoTPlatform_GetSingleFlashBootBankSize()
{
    const esp_partition_t *current_partition = esp_ota_get_running_partition();

    CMP_CHECK(TAG_FLASH_PORT, (current_partition != NULL), "failure getting current OTA partition", -1)

    const esp_partition_t *next_partition = esp_ota_get_next_update_partition(current_partition);

    CMP_CHECK(TAG_FLASH_PORT, (next_partition != NULL), "failure getting next OTA partition", -1)

    return next_partition->size;
}

AzureIoTResult_t AzureIoTPlatform_Init(AzureADUImage_t *const pxAduImage)
{
    const esp_partition_t *current_partition = esp_ota_get_running_partition();

    CMP_CHECK(TAG_FLASH_PORT, (current_partition != NULL), "failure getting current OTA partition", eAzureIoTErrorFailed)

    pxAduImage->partition = esp_ota_get_next_update_partition(current_partition);
    pxAduImage->image_size = 0;

    CMP_CHECK(TAG_FLASH_PORT, (pxAduImage->partition != NULL), "failure getting next OTA partition", eAzureIoTErrorFailed)

    CMP_CHECK(TAG_FLASH_PORT, (esp_ota_begin(pxAduImage->partition, OTA_SIZE_UNKNOWN, &pxAduImage->ota) == ESP_OK), "failure starting OTA", eAzureIoTErrorFailed)

    return eAzureIoTSuccess;
}

AzureIoTResult_t AzureIoTPlatform_WriteBlock(AzureADUImage_t *const pxAduImage,
                                             uint32_t offset,
                                             uint8_t *const pData,
                                             uint32_t ulBlockSize)
{
    esp_err_t result = esp_ota_write_with_offset(pxAduImage->ota, pData, (size_t)ulBlockSize, offset);

    if (result != ESP_OK)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure writing: %d", result);
        return eAzureIoTErrorFailed;
    }

    return eAzureIoTSuccess;
}

AzureIoTResult_t AzureIoTPlatform_VerifyImage(AzureADUImage_t *const pxAduImage,
                                              uint8_t *pucSHA256Hash,
                                              uint32_t ulSHA256HashLength)
{
    uint8_t decoded_hash[AZURE_IOT_SHA_256_SIZE];
    uint8_t calculated_hash[AZURE_IOT_SHA_256_SIZE];
    size_t base64_decoded_length;

    CMP_LOGI(TAG_FLASH_PORT, "base64 encoded hash from ADU: %.*s", (int)ulSHA256HashLength, pucSHA256Hash);

    if (pxAduImage->image_size == 0)
    {
        CMP_LOGE(TAG_FLASH_PORT, "invalid image: no content");
        goto ERROR;
    }

    if (base64_decode(pucSHA256Hash, (unsigned int)ulSHA256HashLength, decoded_hash, sizeof(decoded_hash), &base64_decoded_length) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure decoding base64 SHA256");
        goto ERROR;
    }

    if (image_calculate_hmac_256(pxAduImage, calculated_hash) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure calculating image hash");
        goto ERROR;
    }

    if (memcmp(decoded_hash, calculated_hash, AZURE_IOT_SHA_256_SIZE) != 0)
    {
        CMP_LOGE(TAG_FLASH_PORT, "hashes does not match");
        CMP_LOGE(TAG_FLASH_PORT, "hash wanted: ");
        CMP_LOG_BUFFER_HEX(TAG_FLASH_PORT, decoded_hash, sizeof(decoded_hash));
        CMP_LOGE(TAG_FLASH_PORT, "hash calculated: ");
        CMP_LOG_BUFFER_HEX(TAG_FLASH_PORT, calculated_hash, sizeof(calculated_hash));
        goto ERROR;
    }

    if (esp_ota_end(pxAduImage->ota) != ESP_OK)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure ending OTA");
        goto ERROR;
    }

    return eAzureIoTSuccess;

ERROR:
    esp_ota_abort(pxAduImage->ota);

    return eAzureIoTErrorFailed;
}

AzureIoTResult_t AzureIoTPlatform_EnableImage(AzureADUImage_t *const pxAduImage)
{
    esp_err_t result = esp_ota_set_boot_partition(pxAduImage->partition);

    if (result != ESP_OK)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failure setting boot partition: %d", result);
        return eAzureIoTErrorFailed;
    }

    return eAzureIoTSuccess;
}

__attribute__((noreturn)) AzureIoTResult_t AzureIoTPlatform_ResetDevice(AzureADUImage_t *const)
{
    // This functions restart the device, therefore never returning.
    esp_restart();
}

static AzureIoTResult_t base64_decode(const uint8_t *encoded,
                                      size_t encoded_length,
                                      uint8_t *output_buffer,
                                      size_t output_buffer_length,
                                      size_t *bytes_written)
{
    int result = mbedtls_base64_decode(output_buffer, output_buffer_length, bytes_written, encoded, encoded_length);

    if (result != 0)
    {
        CMP_LOGE(TAG_FLASH_PORT, "failed decoding base64: %d", result);
        return eAzureIoTErrorFailed;
    }

    return eAzureIoTSuccess;
}

static AzureIoTResult_t image_calculate_hmac_256(const AzureADUImage_t *adu_image, uint8_t *output_buffer)
{
    AzureIoTResult_t result = eAzureIoTSuccess;
    mbedtls_md_context_t context;
    uint8_t read_buffer[32];
    uint32_t size_read;
    esp_err_t read_partition_result;

    mbedtls_md_init(&context);
    mbedtls_md_setup(&context, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    mbedtls_md_starts(&context);

    CMP_LOGD(TAG_FLASH_PORT, "calculating hash for image with size: %lu", adu_image->image_size);

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

        if ((read_partition_result = esp_partition_read(adu_image->partition, offset, read_buffer, (size_t)size_read)) != ESP_OK)
        {
            CMP_LOGE(TAG_FLASH_PORT, "failure reading partition: %d", read_partition_result);

            result = eAzureIoTErrorFailed;

            break;
        }

        mbedtls_md_update(&context, (const unsigned char *)read_buffer, (size_t)size_read);
    }

    mbedtls_md_finish(&context, output_buffer);
    mbedtls_md_free(&context);

    CMP_LOGD(TAG_FLASH_PORT, "image hash calculated");

    return result;
}