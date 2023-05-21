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

  /**
   * @typedef azure_adu_workflow_decide_installation_callback_t
   * @brief Callback to decide if the update should be accepted ou rejected.
   * @param[in] update_request @ref Pointer to an AzureIoTADUUpdateRequest_t structure with update information.
   * @return AzureIoTADURequestDecision_t with the decision.
   */
  typedef AzureIoTADURequestDecision_t (*azure_adu_workflow_decide_installation_callback_t)(AzureIoTADUUpdateRequest_t *update_request);

  /**
   * @typedef azure_adu_workflow_download_progress_callback_t
   * @brief Callback to decide if the update should be accepted ou rejected.
   * @param[in] downloaded_size Total downloaded bytes.
   * @param[in] image_size Total image size, in bytes, to be downloaded.
   * @return Boolean indicating if the update must proceed or not.
   */
  typedef void (*azure_adu_workflow_download_progress_callback_t)(uint32_t downloaded_size, uint32_t image_size);

  /**
   * @brief Create a Device Update Workflow context.
   * @note The context must be released by @ref azure_adu_workflow_free.
   * @param[in] adu_context Azure Device Update context.
   * @return @ref azure_adu_workflow_t on success or null on failure.
   */
  azure_adu_workflow_t *azure_adu_workflow_create(azure_adu_context_t *adu_context);

  /**
   * @brief Initialize the workflow.
   * @note Will set the agent state to eAzureIoTADUAgentStateIdle.
   * @note After updating the device, this function must be called after a restart,
   * to notify Azure Device Update that the deployment was successful.
   * @param[in] context Workflow context.
   * @param[in] device_properties Azure IoT Device Update client properties.
   * @return @ref AzureIoTResult_t with the result of the operation.
   */
  AzureIoTResult_t azure_adu_workflow_init_agent(azure_adu_workflow_t *context,
                                                 AzureIoTADUClientDeviceProperties_t *device_properties);

  /**
   * @brief Process an Update Request Manifest sent by Azure IoT Hub.
   * @param[in] context Workflow context.
   * @param[in] json_reader @ref AzureIoTJSONReader_t positioned at the ADU component.
   * @param[in] property_version Azure IoT Plug and Play property version received.
   * @return @ref AzureIoTResult_t with the result of the operation.
   * @example
     if (azure_adu_is_adu_component(context->adu, component_name, component_name_length))
     {
          azure_adu_workflow_process_update_request_manifest(context->adu_workflow, &json_reader, *version))
     }
   */
  AzureIoTResult_t azure_adu_workflow_process_update_request_manifest(azure_adu_workflow_t *context,
                                                                      AzureIoTJSONReader_t *json_reader,
                                                                      uint32_t property_version);

  /**
   * @brief Process the workflow.
   * @note This function should be called after the @ref azure_iot_hub_process_loop,
   * in the same loop.
   * @param[in] context Workflow context.
   * @return @ref AzureIoTResult_t with the result of the operation.
   * @example
     azure_iot_hub_process_loop(iot);
     azure_adu_workflow_process(adu_workflow);
   */
  AzureIoTResult_t azure_adu_workflow_process_loop(azure_adu_workflow_t *context);

  /**
   * @brief Cleanup and free the context.
   * @param[in] context Workflow context.
   */
  void azure_adu_workflow_free(azure_adu_workflow_t *context);

#ifdef __cplusplus
}
#endif
#endif