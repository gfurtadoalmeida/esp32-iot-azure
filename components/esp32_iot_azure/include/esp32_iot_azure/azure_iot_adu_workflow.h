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
   * @typedef azure_adu_workflow_download_progress_callback_t
   * @brief Callback to indicate the download progress.
   * @param[in] downloaded_size Total downloaded bytes.
   * @param[in] image_size Total image size, in bytes, to be downloaded.
   * @param[in] callback_context Pointer to a context to pass to the callback.
   */
  typedef void (*azure_adu_workflow_download_progress_callback_t)(uint32_t downloaded_size,
                                                                  uint32_t image_size,
                                                                  void *callback_context);

  /**
   * @brief Create a Device Update Workflow context.
   * @note The context must be released by @ref azure_adu_workflow_free.
   * @param[in] adu_context Azure Device Update context.
   * @return @ref azure_adu_workflow_t on success or null on failure.
   */
  azure_adu_workflow_t *azure_adu_workflow_create(azure_adu_context_t *adu_context);

  /**
   * @brief Initialize the workflow, setting the state to idle.
   * @note Will set the agent state to eAzureIoTADUAgentStateIdle.
   * @note After updating the device, this function must be called after a restart,
   * to notify Azure Device Update that the deployment was successful.
   * @param[in] context Workflow context.
   * @param[in] device_properties Azure IoT Device Update client properties.
   * @return @ref AzureIoTResult_t with the result of the operation.
   */
  AzureIoTResult_t azure_adu_workflow_init(azure_adu_workflow_t *context,
                                           AzureIoTADUClientDeviceProperties_t *device_properties);

  /**
   * @brief Verify is there is an update available.
   * @param[in] context Workflow context.
   * @return true/false.
   */
  bool azure_adu_workflow_has_update(const azure_adu_workflow_t *context);

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
  AzureIoTResult_t azure_adu_workflow_process_update_request(azure_adu_workflow_t *context,
                                                             AzureIoTJSONReader_t *json_reader,
                                                             uint32_t property_version);

  /**
   * @brief Accept and download a pending update.
   * @param[in] context Workflow context.
   * @param[in] callback Callback to be invoked on download progress.
   * @param[in] callback_context Pointer to a context to pass to the callback.
   * @return @ref AzureIoTResult_t with the result of the operation.
   */
  AzureIoTResult_t azure_adu_workflow_accept_update(azure_adu_workflow_t *context,
                                                    azure_adu_workflow_download_progress_callback_t callback,
                                                    void *callback_context);

  /**
   * @brief Reject a pending update.
   * @param[in] context Workflow context.
   * @return @ref AzureIoTResult_t with the result of the operation.
   */
  AzureIoTResult_t azure_adu_workflow_reject_update(azure_adu_workflow_t *context);

  /**
   * @brief Reset the device. Should be called after a successful update.
   * @note After restarting the device, @ref azure_adu_workflow_init_agent must
   * be called to notify Azure Device Update that the deployment was successful.
   */
  void azure_adu_workflow_reset_device();

  /**
   * @brief Cleanup and free the context.
   * @param[in] context Workflow context.
   */
  void azure_adu_workflow_free(azure_adu_workflow_t *context);

#ifdef __cplusplus
}
#endif
#endif