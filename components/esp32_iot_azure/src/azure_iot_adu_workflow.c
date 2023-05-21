#include <string.h>
#include "esp32_iot_azure/azure_iot_adu_workflow.h"
#include "esp32_iot_azure/azure_iot_http_client.h"
#include "esp32_iot_azure/extension/azure_iot_adu_extension.h"
#include "esp32_iot_azure/extension/azure_iot_http_client_extension.h"
#include "infrastructure/azure_adu_root_key.h"
#include "infrastructure/static_memory.h"
#include "azure_iot_flash_platform.h"
#include "config.h"
#include "log.h"

static const char TAG_AZ_ADU_WKF[] = "AZ_ADU_WKF";

#define CALLBACK_UPDATE(context, success, error_code) \
    if (context->callback_update_finished != NULL)    \
        context->callback_update_finished(success, error_code);

static AzureIoTResult_t azure_adu_step_accept(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_step_reject(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_step_download(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_step_report_success(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_step_finish(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_step_error(azure_adu_workflow_t *context);
static AzureIoTADURequestDecision_t azure_adu_workflow_validate_installation_pre_requisites(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_workflow_download_and_enable_image(azure_adu_workflow_t *context);
static AzureIoTResult_t azure_adu_workflow_download_image(azure_adu_workflow_t *context,
                                                          AzureADUImage_t *image,
                                                          const uint8_t *file_url,
                                                          uint32_t file_url_length,
                                                          const uint8_t *path,
                                                          uint32_t path_length);
static AzureIoTResult_t azure_adu_workflow_enable_image(const azure_adu_workflow_t *context, AzureADUImage_t *image);
static AzureIoTResult_t azure_adu_workflow_send_update_results(azure_adu_workflow_t *context);
static bool download_callback_write_to_flash(uint8_t *data,
                                             uint32_t data_length,
                                             uint32_t current_offset,
                                             uint32_t resource_size,
                                             void *callback_context);

typedef enum
{
    ADU_WORKFLOW_STATE_IDLE = 0,
    ADU_WORKFLOW_STATE_ACCEPT = 1,
    ADU_WORKFLOW_STATE_DOWNLOAD = 2,
    ADU_WORKFLOW_STATE_REPORT_SUCCESS = 3,
    ADU_WORKFLOW_STATE_FINISH = 4,
    ADU_WORKFLOW_STATE_REJECT = 5,
    ADU_WORKFLOW_STATE_CANCEL = 250,
    ADU_WORKFLOW_STATE_ACTION_UNKNOWN = 251,
    ADU_WORKFLOW_STATE_ERROR = 252
} azure_adu_workflow_state_t;

typedef struct
{
    azure_adu_workflow_t *context;
    AzureADUImage_t *image;
} download_callback_context_t;

struct azure_adu_workflow_t
{
    AzureIoTADUUpdateRequest_t update_request;
    azure_adu_context_t *adu_context;
    AzureIoTADUClientDeviceProperties_t *device_properties;
    azure_adu_workflow_decide_installation_callback_t callback_decide_installation;
    azure_adu_workflow_download_progress_callback_t callback_download_progress;
    azure_adu_workflow_update_finished_callback_t callback_update_finished;
    azure_adu_workflow_state_t state;
    uint32_t property_version;
};

azure_adu_workflow_t *azure_adu_workflow_create(azure_adu_context_t *adu_context)
{
    azure_adu_workflow_t *context = (azure_adu_workflow_t *)malloc(sizeof(azure_adu_workflow_t));

    memset(context, 0, sizeof(azure_adu_workflow_t));

    context->adu_context = adu_context;
    context->state = ADU_WORKFLOW_STATE_IDLE;
    context->property_version = 0;

    return context;
}

void azure_adu_workflow_set_callback_decide_installation(azure_adu_workflow_t *context,
                                                         azure_adu_workflow_decide_installation_callback_t callback)
{
    context->callback_decide_installation = callback;
}

void azure_adu_workflow_set_callback_download_progress(azure_adu_workflow_t *context,
                                                       azure_adu_workflow_download_progress_callback_t callback)
{
    context->callback_download_progress = callback;
}

void azure_adu_workflow_set_callback_update_finished(azure_adu_workflow_t *context,
                                                     azure_adu_workflow_update_finished_callback_t callback)
{
    context->callback_update_finished = callback;
}

AzureIoTResult_t azure_adu_workflow_init_agent(azure_adu_workflow_t *context, AzureIoTADUClientDeviceProperties_t *device_properties)
{
    context->device_properties = device_properties;

    return azure_adu_send_agent_state(context->adu_context,
                                      context->device_properties,
                                      NULL,
                                      eAzureIoTADUAgentStateIdle,
                                      NULL,
                                      STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                      CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                      NULL);
}

AzureIoTResult_t azure_adu_workflow_process_update_request_manifest(azure_adu_workflow_t *context,
                                                                    AzureIoTJSONReader_t *json_reader,
                                                                    uint32_t property_version)
{
    AzureIoTResult_t result = azure_adu_parse_request(context->adu_context, json_reader, &context->update_request);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure parsing request: %d", result);
        return result;
    }

    if (context->update_request.xWorkflow.xAction == eAzureIoTADUActionApplyDownload)
    {
        result = AzureIoTJWS_ManifestAuthenticate(context->update_request.pucUpdateManifest,
                                                  context->update_request.ulUpdateManifestLength,
                                                  context->update_request.pucUpdateManifestSignature,
                                                  context->update_request.ulUpdateManifestSignatureLength,
                                                  AZURE_ADU_ROOT_KEYS->keys,
                                                  AZURE_ADU_ROOT_KEYS->keys_count,
                                                  STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                  CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH);

        if (result != eAzureIoTSuccess)
        {
            CMP_LOGE(TAG_AZ_ADU_WKF, "failure validating manifest: %d", result);
            return result;
        }

        context->property_version = property_version;

        if (azure_adu_workflow_validate_installation_pre_requisites(context) == eAzureIoTADURequestDecisionAccept)
        {
            context->state = ADU_WORKFLOW_STATE_ACCEPT;
        }
        else
        {
            context->state = ADU_WORKFLOW_STATE_REJECT;
        }
    }
    else if (context->update_request.xWorkflow.xAction == eAzureIoTADUActionCancel)
    {
        context->state = ADU_WORKFLOW_STATE_CANCEL;

        CMP_LOGI(TAG_AZ_ADU_WKF, "update canceled");
    }
    else
    {
        context->state = ADU_WORKFLOW_STATE_ACTION_UNKNOWN;

        CMP_LOGW(TAG_AZ_ADU_WKF, "action unknown: %d", context->update_request.xWorkflow.xAction);
    }

    return result;
}

AzureIoTResult_t azure_adu_workflow_process_loop(azure_adu_workflow_t *context)
{
    if (context->state == ADU_WORKFLOW_STATE_IDLE)
    {
        return eAzureIoTSuccess;
    }

    if (context->state == ADU_WORKFLOW_STATE_ACCEPT)
    {
        return azure_adu_step_accept(context);
    }

    if (context->state == ADU_WORKFLOW_STATE_DOWNLOAD)
    {
        return azure_adu_step_download(context);
    }

    if (context->state == ADU_WORKFLOW_STATE_REPORT_SUCCESS)
    {
        return azure_adu_step_report_success(context);
    }

    if (context->state == ADU_WORKFLOW_STATE_FINISH || context->state == ADU_WORKFLOW_STATE_CANCEL || context->state == ADU_WORKFLOW_STATE_ACTION_UNKNOWN)
    {
        return azure_adu_step_finish(context);
    }

    if (context->state == ADU_WORKFLOW_STATE_REJECT)
    {
        return azure_adu_step_reject(context);
    }

    if (context->state == ADU_WORKFLOW_STATE_ERROR)
    {
        return azure_adu_step_error(context);
    }

    CMP_LOGW(TAG_AZ_ADU_WKF, "unhandled workflow state: %d", context->state);

    return eAzureIoTErrorFailed;
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
// STEPS
//

static AzureIoTResult_t azure_adu_step_accept(azure_adu_workflow_t *context)
{
    AzureIoTResult_t result = azure_adu_send_response(context->adu_context,
                                                      eAzureIoTADURequestDecisionAccept,
                                                      context->property_version,
                                                      STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                      CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                      NULL);

    context->state = ADU_WORKFLOW_STATE_DOWNLOAD;

    if (result != eAzureIoTSuccess)
    {
        context->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending accept response: %d", result);
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_reject(azure_adu_workflow_t *context)
{
    AzureIoTResult_t result = azure_adu_send_response(context->adu_context,
                                                      eAzureIoTADURequestDecisionReject,
                                                      context->property_version,
                                                      STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                      CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                      NULL);

    context->state = ADU_WORKFLOW_STATE_FINISH;

    if (result != eAzureIoTSuccess)
    {
        context->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending update rejected response");
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_download(azure_adu_workflow_t *context)
{
    AzureIoTResult_t result = azure_adu_send_agent_state(context->adu_context,
                                                         context->device_properties,
                                                         &context->update_request,
                                                         eAzureIoTADUAgentStateDeploymentInProgress,
                                                         NULL,
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    context->state = ADU_WORKFLOW_STATE_REPORT_SUCCESS;

    if (result != eAzureIoTSuccess)
    {
        context->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending in progress response: %d", result);
    }
    else
    {
        result = azure_adu_workflow_download_and_enable_image(context);

        if (result != eAzureIoTSuccess)
        {
            context->state = ADU_WORKFLOW_STATE_ERROR;

            CMP_LOGE(TAG_AZ_ADU_WKF, "failure downloading and enabling image: %d", result);
        }
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_report_success(azure_adu_workflow_t *context)
{
    AzureIoTResult_t result = azure_adu_workflow_send_update_results(context);

    // Do not go to ADU_WORKFLOW_STATE_FINISH.
    // Doing so will change the device update status to failed.
    // A device update will only succeed when a azure_adu_send_agent_state
    // with eAzureIoTADUAgentStateIdle status and the new version is sent
    // to the service. This will occur when the device restarts and
    // boot with the new image version.

    context->state = ADU_WORKFLOW_STATE_IDLE;

    if (result != eAzureIoTSuccess)
    {
        context->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure reporting success: %d", result);

        CALLBACK_UPDATE(context, false, ADU_WORKFLOW_STATE_REPORT_SUCCESS)
    }
    else
    {
        CALLBACK_UPDATE(context, true, 0)
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_finish(azure_adu_workflow_t *context)
{
    AzureIoTResult_t result = azure_adu_send_agent_state(context->adu_context,
                                                         context->device_properties,
                                                         &context->update_request,
                                                         eAzureIoTADUAgentStateIdle,
                                                         NULL,
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    context->state = ADU_WORKFLOW_STATE_IDLE;

    if (result != eAzureIoTSuccess)
    {
        context->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure finishing: %d", result);

        CALLBACK_UPDATE(context, false, ADU_WORKFLOW_STATE_FINISH)
    }
    else
    {
        CALLBACK_UPDATE(context, true, 0)
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_error(azure_adu_workflow_t *context)
{
    AzureIoTResult_t result = azure_adu_send_agent_state(context->adu_context,
                                                         context->device_properties,
                                                         NULL,
                                                         eAzureIoTADUAgentStateError,
                                                         NULL,
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    // We'll always go to idle to not loop infinitely on error.
    context->state = ADU_WORKFLOW_STATE_IDLE;

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending error: %d", result);

        CALLBACK_UPDATE(context, false, ADU_WORKFLOW_STATE_ERROR)
    }
    else
    {
        CALLBACK_UPDATE(context, true, 0)
    }

    return result;
}

//
// PRIVATE
//

static AzureIoTADURequestDecision_t azure_adu_workflow_validate_installation_pre_requisites(azure_adu_workflow_t *context)
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
                 "not flash space: has %d but needs %d",
                 (int32_t)free_space,
                 (int32_t)context->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes);

        return eAzureIoTADURequestDecisionReject;
    }

    if (context->callback_decide_installation != NULL && context->callback_decide_installation(&context->update_request) != eAzureIoTADURequestDecisionAccept)
    {
        CMP_LOGI(TAG_AZ_ADU_WKF, "installation rejected by the caller");
        return eAzureIoTADURequestDecisionReject;
    }

    return eAzureIoTADURequestDecisionAccept;
}

static AzureIoTResult_t azure_adu_workflow_download_and_enable_image(azure_adu_workflow_t *context)
{
    uint8_t *file_url = NULL;
    uint32_t file_url_length = 0;
    uint8_t *path = NULL;
    uint32_t path_length = 0;
    AzureADUImage_t image;

    if (AzureIoTPlatform_Init(&image) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure initializing flash platform");
        return eAzureIoTErrorFailed;
    }

    if (azure_adu_parse_file_url(&context->update_request.pxFileUrls[0],
                                 STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                 &file_url,
                                 &file_url_length,
                                 &path,
                                 &path_length) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure parsing file url");
        return eAzureIoTErrorFailed;
    }

    // Remote null-terminator from file_url and path.
    if (azure_adu_workflow_download_image(context,
                                          &image,
                                          file_url,
                                          file_url_length - 1,
                                          path,
                                          path_length - 1) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure downloading image");
        return eAzureIoTErrorFailed;
    }

    if (azure_adu_workflow_enable_image(context, &image) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure enabling image");
        return eAzureIoTErrorFailed;
    }

    return eAzureIoTSuccess;
}

static AzureIoTResult_t azure_adu_workflow_download_image(azure_adu_workflow_t *context,
                                                          AzureADUImage_t *image,
                                                          const uint8_t *file_url,
                                                          uint32_t file_url_length,
                                                          const uint8_t *path,
                                                          uint32_t path_length)
{
    azure_http_context_t *http = azure_http_create((const char *)file_url,
                                                   file_url_length,
                                                   (const char *)path,
                                                   path_length);

    if (azure_http_connect(http) != eAzureIoTHTTPSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure connecting to: %s", file_url);
        return eAzureIoTErrorFailed;
    }

    download_callback_context_t download_context = {
        .context = context,
        .image = image};

    AzureIoTResult_t result = azure_http_download_resource(http,
                                                           (char *)STATIC_MEMORY_ADU_DOWNLOAD,
                                                           CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_HTTP_DOWNLOAD,
                                                           CONFIG_ESP32_IOT_AZURE_DU_HTTP_DOWNLOAD_CHUNCK_SIZE,
                                                           &download_callback_write_to_flash,
                                                           &download_context,
                                                           &image->image_size) == eAzureIoTHTTPSuccess
                                  ? eAzureIoTSuccess
                                  : eAzureIoTErrorFailed;

    azure_http_disconnect(http);
    azure_http_free(http);

    return result;
}

static AzureIoTResult_t azure_adu_workflow_enable_image(const azure_adu_workflow_t *context, AzureADUImage_t *image)
{
    AzureIoTResult_t result = AzureIoTPlatform_VerifyImage(image,
                                                           context->update_request.xUpdateManifest.pxFiles[0].pxHashes[0].pucHash,
                                                           context->update_request.xUpdateManifest.pxFiles[0].pxHashes[0].ulHashLength);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure validating image: %d", result);
        return eAzureIoTErrorFailed;
    }

    if ((result = AzureIoTPlatform_EnableImage(image)) != eAzureIoTSuccess)
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
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending results: %d", result);
        return result;
    }

    return eAzureIoTSuccess;
}

static bool download_callback_write_to_flash(uint8_t *chunck,
                                             uint32_t chunck_length,
                                             uint32_t start_offset,
                                             uint32_t resource_size,
                                             void *callback_context)
{
    download_callback_context_t *context = (download_callback_context_t *)callback_context;

    if (AzureIoTPlatform_WriteBlock(context->image,
                                    start_offset,
                                    chunck,
                                    chunck_length) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure writing to flash");
        return false;
    }

    if (context->context->callback_download_progress != NULL)
    {
        context->context->callback_download_progress(start_offset + chunck_length, resource_size);
    }

    return true;
}
