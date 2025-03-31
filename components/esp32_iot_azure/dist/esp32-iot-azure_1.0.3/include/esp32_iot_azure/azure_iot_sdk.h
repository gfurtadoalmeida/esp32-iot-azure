#ifndef __ESP32_IOT_AZURE_IOT_SDK_H__
#define __ESP32_IOT_AZURE_IOT_SDK_H__

#include "azure_iot_result.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize the Azure SDK.
     * @return @ref AzureIoTResult_t with the result of the operation.
     */
    AzureIoTResult_t azure_iot_sdk_init();

    /**
     * @brief Cleanup and free the Azure SDK.
     */
    void azure_iot_sdk_deinit();

#ifdef __cplusplus
}
#endif
#endif