#ifndef __ESP32_IOT_AZURE_ADU_WORKFLOW_H__
#define __ESP32_IOT_AZURE_ADU_WORKFLOW_H__

#include <stdint.h>
#include "esp32_iot_azure/azure_iot_adu.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @typedef azure_adu_workflow_t
     * @brief Azure IoT Device Update Workflow context.
     */
    typedef struct azure_adu_workflow_t azure_adu_workflow_t;

    typedef bool (*azure_adu_workflow_should_install_callback_t)(AzureIoTADUUpdateRequest_t *update_request);
    typedef bool (*azure_adu_workflow_download_progress_callback_t)(uint32_t image_size, uint32_t downloaded_size);

    azure_adu_workflow_t *azure_adu_workflow_create(azure_adu_context_t *adu_context);

    AzureIoTResult_t azure_adu_workflow_init_agent(azure_adu_workflow_t *workflow,
                                                   AzureIoTADUClientDeviceProperties_t *device_properties);

    AzureIoTResult_t azure_adu_workflow_process_update_request_manifest(azure_adu_workflow_t *workflow,
                                                                        AzureIoTJSONReader_t *json_reader,
                                                                        uint32_t property_version);

    AzureIoTResult_t azure_adu_workflow_process(azure_adu_workflow_t *workflow);

    void azure_adu_workflow_free(azure_adu_workflow_t *workflow);

#ifdef __cplusplus
}
#endif
#endif