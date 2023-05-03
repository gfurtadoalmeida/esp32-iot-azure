#include "esp32_iot_azure/azure_iot_sdk.h"
#include "azure_iot.h"

AzureIoTResult_t azure_iot_sdk_init()
{
    return AzureIoT_Init();
}

void azure_iot_sdk_deinit()
{
    AzureIoT_Deinit();
}