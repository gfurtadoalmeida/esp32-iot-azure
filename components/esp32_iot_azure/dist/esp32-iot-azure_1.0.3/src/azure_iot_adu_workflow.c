#include <string.h>
#include "esp32_iot_azure/azure_iot_adu_workflow.h"
#include "esp32_iot_azure/azure_iot_http_client.h"
#include "esp32_iot_azure/extension/azure_iot_adu_extension.h"
#include "esp32_iot_azure/extension/azure_iot_http_client_extension.h"
#include "infrastructure/azure_adu_root_key.h"
#include "azure_iot_flash_platform.h"
#include "config.h"
#include "log.h"

static const char TAG_AZ_ADU_WKF[] = "AZ_ADU_WKF";

static AzureIoTResult_t azure_adu_workflow_cancel_update(azure_adu_workflow_t *context);
static AzureIoTADURequestDecision_t azure_adu_workflow_validate_installation_pre_requisites(const azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_workflow_download_and_enable_image(azure_adu_workflow_t *context,
                                                                     buffer_t *download_buffer,
                                                                     uint16_t chunk_size,
                                                                     azure_adu_workflow_download_progress_callback_t callback,
                                                                     void *callback_context);
static AzureIoTResult_t azure_adu_workflow_send_update_results(azure_adu_workflow_t *context);
static bool download_callback_write_to_flash(uint8_t *data,
                                             uint32_t data_length,
                                             uint32_t current_offset,
                                             uint32_t resource_size,
                                             void *callback_context);
typedef struct
{
    azure_adu_workflow_t *context;
    AzureADUImage_t *image;
    azure_adu_workflow_download_progress_callback_t callback;
    void *callback_context;
} download_callback_context_t;

struct azure_adu_workflow_t
{
    AzureIoTADUUpdateRequest_t update_request;
    azure_adu_context_t *adu_context;
    AzureIoTADUClientDeviceProperties_t *device_properties;
    buffer_t *scratch_buffer;
    uint32_t property_version;
    bool has_update;
};

azure_adu_workflow_t *azure_adu_workflow_create(azure_adu_context_t *adu_context, buffer_t *operation_buffer)
{
    if (operation_buffer == NULL || operation_buffer->buffer == NULL)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "operation_buffer null");
        return NULL;
    }
    azure_adu_workflow_t *context = (azure_adu_workflow_t *)malloc(sizeof(azure_adu_workflow_t));

    memset(context, 0, sizeof(azure_adu_workflow_t));

    context->adu_context = adu_context;
    context->property_version = 0;
    context->has_update = false;
    context->scratch_buffer = operation_buffer;

    return context;
}

AzureIoTResult_t azure_adu_workflow_init(azure_adu_workflow_t *context, AzureIoTADUClientDeviceProperties_t *device_properties)
{
    context->device_properties = device_properties;

    return azure_adu_send_agent_state(context->adu_context,
                                      context->device_properties,
                                      NULL,
                                      eAzureIoTADUAgentStateIdle,
                                      NULL,
                                      context->scratch_buffer->buffer,
                                      context->scratch_buffer->length,
                                      NULL);
}

bool azure_adu_workflow_has_update(const azure_adu_workflow_t *context)
{
    return context->has_update;
}

AzureIoTResult_t azure_adu_workflow_process_update_request(azure_adu_workflow_t *context,
                                                           AzureIoTJSONReader_t *json_reader,
                                                           uint32_t property_version)
{
    AzureIoTResult_t result = azure_adu_parse_request(context->adu_context, json_reader, &context->update_request);

    context->property_version = property_version;

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure parsing request: %d", result);
        return result;
    }

    if (context->update_request.xWorkflow.xAction == eAzureIoTADUActionApplyDownload)
    {
        const azure_jws_root_keys_t *jws_keys = azure_adu_root_key_get();

        if ((result = AzureIoTJWS_ManifestAuthenticate(context->update_request.pucUpdateManifest,
                                                       context->update_request.ulUpdateManifestLength,
                                                       context->update_request.pucUpdateManifestSignature,
                                                       context->update_request.ulUpdateManifestSignatureLength,
                                                       jws_keys->keys,
                                                       jws_keys->keys_count,
                                                       context->scratch_buffer->buffer,
                                                       context->scratch_buffer->length)) != eAzureIoTSuccess)
        {
            CMP_LOGE(TAG_AZ_ADU_WKF, "failure validating manifest: %d", result);
            return result;
        }

        if (azure_adu_workflow_validate_installation_pre_requisites(context) == eAzureIoTADURequestDecisionAccept)
        {
            context->has_update = true;

            return eAzureIoTSuccess;
        }
        else
        {
            return azure_adu_workflow_reject_update(context);
        }
    }
    else
    {
        return azure_adu_workflow_cancel_update(context);
    }
}

AzureIoTResult_t azure_adu_workflow_accept_update(azure_adu_workflow_t *context,
                                                  buffer_t *download_buffer,
                                                  uint16_t chunk_size,
                                                  azure_adu_workflow_download_progress_callback_t callback,
                                                  void *callback_context)
{
    if (!context->has_update)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "no update to accept");
        return eAzureIoTErrorFailed;
    }

    if (download_buffer == NULL || download_buffer->buffer == NULL)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "download_buffer null");
        return eAzureIoTErrorInvalidArgument;
    }

    if (download_buffer->length < chunk_size + ADU_WORKFLOW_DOWNLOAD_BUFFER_EXTRA_BYTES)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "not enough memory on download_buffer: has %lu needs %u", download_buffer->length, chunk_size + ADU_WORKFLOW_DOWNLOAD_BUFFER_EXTRA_BYTES);
        return eAzureIoTErrorInvalidArgument;
    }

    AzureIoTResult_t result = eAzureIoTSuccess;

    if ((result = azure_adu_send_response(context->adu_context,
                                          eAzureIoTADURequestDecisionAccept,
                                          context->property_version,
                                          context->scratch_buffer->buffer,
                                          context->scratch_buffer->length,
                                          NULL)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending accept response: %d", result);
        return result;
    }

    if ((result = azure_adu_send_agent_state(context->adu_context,
                                             context->device_properties,
                                             &context->update_request,
                                             eAzureIoTADUAgentStateDeploymentInProgress,
                                             NULL,
                                             context->scratch_buffer->buffer,
                                             context->scratch_buffer->length,
                                             NULL)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure setting in progress state: %d", result);
        return result;
    }

    if ((result = azure_adu_workflow_download_and_enable_image(context,
                                                               download_buffer,
                                                               chunk_size,
                                                               callback,
                                                               callback_context)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure downloading and enabling image: %d", result);
        return result;
    }

    context->has_update = false;

    // Once the image is downloaded and enabled, it does not matter
    // if we send the results or not.
    //
    // Do not send any eAzureIoTADUAgentState*
    // Doing so will change the device update status to failed.
    // A device update will only succeed when a azure_adu_send_agent_state
    // with eAzureIoTADUAgentStateIdle status and the new version is sent
    // to the service. This will occur when the device restarts and
    // boot with the new image version.
    if (azure_adu_workflow_send_update_results(context) != eAzureIoTSuccess)
    {
        CMP_LOGW(TAG_AZ_ADU_WKF, "failure updating results: %d", result);
    }

    return result;
}

AzureIoTResult_t azure_adu_workflow_reject_update(azure_adu_workflow_t *context)
{
    if (!context->has_update)
    {
        return eAzureIoTSuccess;
    }

    context->has_update = false;

    AzureIoTResult_t result = eAzureIoTSuccess;

    if ((result = azure_adu_send_response(context->adu_context,
                                          eAzureIoTADURequestDecisionReject,
                                          context->property_version,
                                          context->scratch_buffer->buffer,
                                          context->scratch_buffer->length,
                                          NULL)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending reject response: %d", result);
    }

    return azure_adu_workflow_cancel_update(context);
}

void azure_adu_workflow_reset_device()
{
    AzureIoTPlatform_ResetDevice(NULL);
}

void azure_adu_workflow_free(azure_adu_workflow_t *context)
{
    free(context);
}

//
// PRIVATE
//

static AzureIoTResult_t azure_adu_workflow_cancel_update(azure_adu_workflow_t *context)
{
    context->has_update = false;

    AzureIoTResult_t result = eAzureIoTSuccess;

    if ((result = azure_adu_send_agent_state(context->adu_context,
                                             context->device_properties,
                                             &context->update_request,
                                             eAzureIoTADUAgentStateIdle,
                                             NULL,
                                             context->scratch_buffer->buffer,
                                             context->scratch_buffer->length,
                                             NULL)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending idle state: %d", result);
    }

    return result;
}

static AzureIoTADURequestDecision_t azure_adu_workflow_validate_installation_pre_requisites(const azure_adu_workflow_t *context)
{
    if (context->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes < 0)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "invalid image: no content");
        return eAzureIoTADURequestDecisionReject;
    }

    int64_t free_space = AzureIoTPlatform_GetSingleFlashBootBankSize();

    if (free_space < context->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF,
                 "not flash space: has %ld but needs %ld",
                 (int32_t)free_space,
                 (int32_t)context->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes);

        return eAzureIoTADURequestDecisionReject;
    }

    return eAzureIoTADURequestDecisionAccept;
}

static AzureIoTResult_t azure_adu_workflow_download_and_enable_image(azure_adu_workflow_t *context,
                                                                     buffer_t *download_buffer,
                                                                     uint16_t chunk_size,
                                                                     azure_adu_workflow_download_progress_callback_t callback,
                                                                     void *callback_context)
{
    parsed_file_url_t parsed_url;
    AzureADUImage_t image;
    AzureIoTResult_t result;

    if ((result = AzureIoTPlatform_Init(&image)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure initializing flash: %d", result);
        return eAzureIoTErrorFailed;
    }

    if ((result = azure_adu_file_parse_url(&context->update_request.pxFileUrls[0],
                                           context->scratch_buffer->buffer,
                                           &parsed_url)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure parsing file url: %d", result);
        return eAzureIoTErrorFailed;
    }

    download_callback_context_t download_context = {
        .context = context,
        .image = &image,
        .callback = callback,
        .callback_context = callback_context};

    if ((result = azure_adu_file_download(&parsed_url,
                                          download_buffer->buffer,
                                          download_buffer->length,
                                          chunk_size,
                                          &download_callback_write_to_flash,
                                          &download_context,
                                          &image.image_size)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure downloading image: %d", result);
        return eAzureIoTErrorFailed;
    }

    if ((result = AzureIoTPlatform_VerifyImage(&image,
                                               context->update_request.xUpdateManifest.pxFiles[0].pxHashes[0].pucHash,
                                               context->update_request.xUpdateManifest.pxFiles[0].pxHashes[0].ulHashLength)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure validating image: %d", result);
        return eAzureIoTErrorFailed;
    }

    if ((result = AzureIoTPlatform_EnableImage(&image)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure enabling image: %d", result);
        return eAzureIoTErrorFailed;
    }

    return eAzureIoTSuccess;
}

static AzureIoTResult_t azure_adu_workflow_send_update_results(azure_adu_workflow_t *context)
{
    AzureIoTADUClientInstallResult_t update_results =
        {
            .lResultCode = 200,
            .lExtendedResultCode = 200,
            .pucResultDetails = (uint8_t *)"device updated",
            .ulResultDetailsLength = sizeof("device updated") - 1,
            .ulStepResultsCount = context->update_request.xUpdateManifest.xInstructions.ulStepsCount};

    // The order of the step results must match the order of the steps
    // in the the update manifest instructions.
    for (int32_t step_index = 0; step_index < update_results.ulStepResultsCount; step_index++)
    {
        update_results.pxStepResults[step_index].ulResultCode = 200;
        update_results.pxStepResults[step_index].ulExtendedResultCode = 200;
        update_results.pxStepResults[step_index].pucResultDetails = (uint8_t *)"device updated";
        update_results.pxStepResults[step_index].ulResultDetailsLength = sizeof("device updated") - 1;
    }

    AzureIoTResult_t result = azure_adu_send_agent_state(context->adu_context,
                                                         context->device_properties,
                                                         &context->update_request,
                                                         eAzureIoTADUAgentStateDeploymentInProgress,
                                                         &update_results,
                                                         context->scratch_buffer->buffer,
                                                         context->scratch_buffer->length,
                                                         NULL);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending results: %d", result);
        return result;
    }

    return eAzureIoTSuccess;
}

static bool download_callback_write_to_flash(uint8_t *chunk,
                                             uint32_t chunk_length,
                                             uint32_t start_offset,
                                             uint32_t resource_size,
                                             void *callback_context)
{
    download_callback_context_t *context = (download_callback_context_t *)callback_context;

    if (AzureIoTPlatform_WriteBlock(context->image,
                                    start_offset,
                                    chunk,
                                    chunk_length) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure writing to flash");
        return false;
    }

    if (context->callback != NULL)
    {
        context->callback(start_offset + chunk_length, resource_size, context->callback_context);
    }

    return true;
}
