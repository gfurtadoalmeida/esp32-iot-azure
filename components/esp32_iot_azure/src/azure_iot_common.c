#include "esp32_iot_azure/azure_iot_common.h"

uint32_t strlen_uint8_t(const uint8_t *buffer)
{
    uint32_t length = 0;

    for (length = 0; buffer[length]; length++)
        ;

    return length;
}