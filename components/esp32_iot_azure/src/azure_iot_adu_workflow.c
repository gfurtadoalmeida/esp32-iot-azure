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

static AzureIoTResult_t azure_adu_step_accept(azure_adu_workflow_t *workflow);
static AzureIoTResult_t azure_adu_step_reject(azure_adu_workflow_t *workflow);
static AzureIoTResult_t azure_adu_step_download(azure_adu_workflow_t *workflow);
static AzureIoTResult_t azure_adu_step_report_success(azure_adu_workflow_t *workflow);
static AzureIoTResult_t azure_adu_step_finish(azure_adu_workflow_t *workflow);
static AzureIoTResult_t azure_adu_step_error(azure_adu_workflow_t *workflow);

static AzureIoTADURequestDecision_t azure_adu_workflow_validate_installation_pre_requisites(azure_adu_workflow_t *workflow);
static AzureIoTResult_t azure_adu_workflow_download_and_enable_image(azure_adu_workflow_t *workflow);
static bool download_callback(uint8_t *data,
                              uint32_t data_length,
                              uint32_t current_offset,
                              uint32_t resource_size,
                              void *callback_context);
static AzureIoTResult_t azure_adu_workflow_enable_image(const azure_adu_workflow_t *workflow, AzureADUImage_t *image);
static AzureIoTResult_t azure_adu_workflow_send_update_results(const azure_adu_workflow_t *workflow);

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
    azure_adu_workflow_t *workflow;
    AzureADUImage_t *image;
} download_callback_context_t;

struct azure_adu_workflow_t
{
    AzureIoTADUUpdateRequest_t update_request;
    azure_adu_context_t *adu_context;
    AzureIoTADUClientDeviceProperties_t *device_properties;
    azure_adu_workflow_should_install_callback_t should_install_callback;
    azure_adu_workflow_download_progress_callback_t download_progress_callback;
    azure_adu_workflow_state_t state;
    uint32_t property_version;
};

azure_adu_workflow_t *azure_adu_workflow_create(azure_adu_context_t *adu_context)
{
    azure_adu_workflow_t *workflow = (azure_adu_workflow_t *)malloc(sizeof(azure_adu_workflow_t));

    memset(workflow, 0, sizeof(azure_adu_workflow_t));

    workflow->adu_context = adu_context;
    workflow->state = ADU_WORKFLOW_STATE_IDLE;
    workflow->property_version = 0;

    return workflow;
}

AzureIoTResult_t azure_adu_workflow_init_agent(azure_adu_workflow_t *workflow, AzureIoTADUClientDeviceProperties_t *device_properties)
{
    workflow->device_properties = device_properties;

    return azure_adu_send_agent_state(workflow->adu_context,
                                      workflow->device_properties,
                                      NULL,
                                      eAzureIoTADUAgentStateIdle,
                                      NULL,
                                      STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                      CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                      NULL);
}

AzureIoTResult_t azure_adu_workflow_process_update_request_manifest(azure_adu_workflow_t *workflow,
                                                                    AzureIoTJSONReader_t *json_reader,
                                                                    uint32_t property_version)
{
    AzureIoTResult_t result = azure_adu_parse_request(workflow->adu_context, json_reader, &workflow->update_request);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure parsing request: %d", result);
        return result;
    }

    if (workflow->update_request.xWorkflow.xAction == eAzureIoTADUActionApplyDownload)
    {
        result = AzureIoTJWS_ManifestAuthenticate(workflow->update_request.pucUpdateManifest,
                                                  workflow->update_request.ulUpdateManifestLength,
                                                  workflow->update_request.pucUpdateManifestSignature,
                                                  workflow->update_request.ulUpdateManifestSignatureLength,
                                                  AZURE_ADU_ROOT_KEYS->keys,
                                                  AZURE_ADU_ROOT_KEYS->keys_count,
                                                  STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                  CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH);

        if (result != eAzureIoTSuccess)
        {
            CMP_LOGE(TAG_AZ_ADU_WKF, "failure validating manifest: %d", result);
            return result;
        }

        workflow->property_version = property_version;

        if (azure_adu_workflow_validate_installation_pre_requisites(workflow) == eAzureIoTADURequestDecisionAccept)
        {
            workflow->state = ADU_WORKFLOW_STATE_ACCEPT;
        }
        else
        {
            workflow->state = ADU_WORKFLOW_STATE_REJECT;
        }
    }
    else if (workflow->update_request.xWorkflow.xAction == eAzureIoTADUActionCancel)
    {
        workflow->state = ADU_WORKFLOW_STATE_CANCEL;

        CMP_LOGI(TAG_AZ_ADU_WKF, "update canceled");
    }
    else
    {
        workflow->state = ADU_WORKFLOW_STATE_ACTION_UNKNOWN;

        CMP_LOGW(TAG_AZ_ADU_WKF, "action unknown: %d", workflow->update_request.xWorkflow.xAction);
    }

    return result;
}

AzureIoTResult_t azure_adu_workflow_process(azure_adu_workflow_t *workflow)
{
    if (workflow->state == ADU_WORKFLOW_STATE_IDLE)
    {
        return eAzureIoTSuccess;
    }

    if (workflow->state == ADU_WORKFLOW_STATE_ACCEPT)
    {
        return azure_adu_step_accept(workflow);
    }

    if (workflow->state == ADU_WORKFLOW_STATE_DOWNLOAD)
    {
        return azure_adu_step_download(workflow);
    }

    if (workflow->state == ADU_WORKFLOW_STATE_REPORT_SUCCESS)
    {
        return azure_adu_step_report_success(workflow);
    }

    if (workflow->state == ADU_WORKFLOW_STATE_FINISH || workflow->state == ADU_WORKFLOW_STATE_CANCEL || workflow->state == ADU_WORKFLOW_STATE_ACTION_UNKNOWN)
    {
        return azure_adu_step_finish(workflow);
    }

    if (workflow->state == ADU_WORKFLOW_STATE_REJECT)
    {
        return azure_adu_step_reject(workflow);
    }

    if (workflow->state == ADU_WORKFLOW_STATE_ERROR)
    {
        return azure_adu_step_error(workflow);
    }

    CMP_LOGW(TAG_AZ_ADU_WKF, "unhandled workflow state: %d", workflow->state);

    return eAzureIoTErrorFailed;
}

void azure_adu_workflow_free(azure_adu_workflow_t *workflow)
{
    free(workflow);
}

//
// STEPS
//

static AzureIoTResult_t azure_adu_step_accept(azure_adu_workflow_t *workflow)
{
    AzureIoTResult_t result = azure_adu_send_response(workflow->adu_context,
                                                      eAzureIoTADURequestDecisionAccept,
                                                      workflow->property_version,
                                                      STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                      CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                      NULL);

    workflow->state = ADU_WORKFLOW_STATE_DOWNLOAD;

    if (result != eAzureIoTSuccess)
    {
        workflow->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending accept response: %d", result);
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_reject(azure_adu_workflow_t *workflow)
{
    AzureIoTResult_t result = azure_adu_send_response(workflow->adu_context,
                                                      eAzureIoTADURequestDecisionReject,
                                                      workflow->property_version,
                                                      STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                      CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                      NULL);

    workflow->state = ADU_WORKFLOW_STATE_FINISH;

    if (result != eAzureIoTSuccess)
    {
        workflow->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending update rejected response");
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_download(azure_adu_workflow_t *workflow)
{
    AzureIoTResult_t result = azure_adu_send_agent_state(workflow->adu_context,
                                                         workflow->device_properties,
                                                         &workflow->update_request,
                                                         eAzureIoTADUAgentStateDeploymentInProgress,
                                                         NULL,
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    workflow->state = ADU_WORKFLOW_STATE_REPORT_SUCCESS;

    if (result != eAzureIoTSuccess)
    {
        workflow->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending in progress response: %d", result);
    }
    else
    {
        result = azure_adu_workflow_download_and_enable_image(workflow);

        if (result != eAzureIoTSuccess)
        {
            workflow->state = ADU_WORKFLOW_STATE_ERROR;

            CMP_LOGE(TAG_AZ_ADU_WKF, "failure downloading and enabling image: %d", result);
        }
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_report_success(azure_adu_workflow_t *workflow)
{
    AzureIoTResult_t result = azure_adu_workflow_send_update_results(workflow);

    // Do not go to ADU_WORKFLOW_STATE_FINISH.
    // Doing so will change the device update status to failed.
    // A device update will only succeed when a azure_adu_send_agent_state
    // with eAzureIoTADUAgentStateIdle status and the new version is sent
    // to the service. This will occur when the device restarts and
    // boot with the new image version.

    workflow->state = ADU_WORKFLOW_STATE_IDLE;

    if (result != eAzureIoTSuccess)
    {
        workflow->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure reporting success: %d", result);
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_finish(azure_adu_workflow_t *workflow)
{
    AzureIoTResult_t result = azure_adu_send_agent_state(workflow->adu_context,
                                                         workflow->device_properties,
                                                         &workflow->update_request,
                                                         eAzureIoTADUAgentStateIdle,
                                                         NULL,
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    workflow->state = ADU_WORKFLOW_STATE_IDLE;

    if (result != eAzureIoTSuccess)
    {
        workflow->state = ADU_WORKFLOW_STATE_ERROR;

        CMP_LOGE(TAG_AZ_ADU_WKF, "failure finishing: %d", result);
    }

    return result;
}

static AzureIoTResult_t azure_adu_step_error(azure_adu_workflow_t *workflow)
{
    AzureIoTResult_t result = azure_adu_send_agent_state(workflow->adu_context,
                                                         workflow->device_properties,
                                                         NULL,
                                                         eAzureIoTADUAgentStateError,
                                                         NULL,
                                                         STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                                         CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_SCRATCH,
                                                         NULL);

    // We'll always go to idle to not loop infinitely on error.
    workflow->state = ADU_WORKFLOW_STATE_IDLE;

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure sending error: %d", result);
    }

    return result;
}

//
// PRIVATE
//

static AzureIoTADURequestDecision_t azure_adu_workflow_validate_installation_pre_requisites(azure_adu_workflow_t *workflow)
{
    if (workflow->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes < 0)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "invalid image: no content");
        return eAzureIoTADURequestDecisionReject;
    }

    int64_t free_space = AzureIoTPlatform_GetSingleFlashBootBankSize();

    if (free_space < workflow->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF,
                 "not flash space: has %d but needs %d",
                 (int32_t)free_space,
                 (int32_t)workflow->update_request.xUpdateManifest.pxFiles[0].llSizeInBytes);

        return eAzureIoTADURequestDecisionReject;
    }

    if (workflow->should_install_callback != NULL && !workflow->should_install_callback(&workflow->update_request))
    {
        CMP_LOGI(TAG_AZ_ADU_WKF, "installation rejected by the user");
        return eAzureIoTADURequestDecisionReject;
    }

    return eAzureIoTADURequestDecisionAccept;
}

static AzureIoTResult_t azure_adu_workflow_download_and_enable_image(azure_adu_workflow_t *workflow)
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

    if (azure_adu_parse_file_url(&workflow->update_request.pxFileUrls[0],
                                 STATIC_MEMORY_ADU_SCRATCH_BUFFER,
                                 &file_url,
                                 &file_url_length,
                                 &path,
                                 &path_length) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure parsing file url");
        return eAzureIoTErrorFailed;
    }

    azure_http_context_t *http = azure_http_create((const char *)file_url,
                                                   file_url_length - 1, // Remove null-terminator.
                                                   (const char *)path,
                                                   path_length - 1); // Remove null-terminator.

    if (azure_http_connect(http) != eAzureIoTHTTPSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure connecting to: %s", file_url);
        return eAzureIoTErrorFailed;
    }

    download_callback_context_t download_context = {
        .workflow = workflow,
        .image = &image};

    AzureIoTResult_t result = azure_http_download_resource(http,
                                                           (char *)STATIC_MEMORY_ADU_DOWNLOAD,
                                                           CONFIG_ESP32_IOT_AZURE_DU_BUFFER_SIZE_HTTP_DOWNLOAD,
                                                           CONFIG_ESP32_IOT_AZURE_DU_HTTP_DOWNLOAD_CHUNCK_SIZE,
                                                           &download_callback,
                                                           &download_context,
                                                           &image.image_size) == eAzureIoTHTTPSuccess
                                  ? eAzureIoTSuccess
                                  : eAzureIoTErrorFailed;

    azure_http_disconnect(http);
    azure_http_free(http);

    if (result != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure downloading image");
        return result;
    }

    if ((result = azure_adu_workflow_enable_image(workflow, &image)) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure enabling image");
    }

    return result;
}

static bool download_callback(uint8_t *data,
                              uint32_t data_length,
                              uint32_t current_offset,
                              uint32_t resource_size,
                              void *callback_context)
{
    download_callback_context_t *context = (download_callback_context_t *)callback_context;

    if (AzureIoTPlatform_WriteBlock(context->image,
                                    current_offset,
                                    data,
                                    data_length) != eAzureIoTSuccess)
    {
        CMP_LOGE(TAG_AZ_ADU_WKF, "failure writing to flash");
        return false;
    }

    current_offset += data_length;

    if (context->workflow->download_progress_callback != NULL)
    {
        context->workflow->download_progress_callback(resource_size, current_offset);
    }

    return true;
}

static AzureIoTResult_t azure_adu_workflow_enable_image(const azure_adu_workflow_t *workflow, AzureADUImage_t *image)
{
    AzureIoTResult_t result = AzureIoTPlatform_VerifyImage(image,
                                                           workflow->update_request.xUpdateManifest.pxFiles[0].pxHashes[0].pucHash,
                                                           workflow->update_request.xUpdateManifest.pxFiles[0].pxHashes[0].ulHashLength);

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

static AzureIoTResult_t azure_adu_workflow_send_update_results(const azure_adu_workflow_t *workflow)
{
    AzureIoTADUClientInstallResult_t update_results =
        {
            .lResultCode = 200,
            .lExtendedResultCode = 200,
            .pucResultDetails = (uint8_t *)"device updated",
            .ulResultDetailsLength = sizeof("device updated") - 1,
            .ulStepResultsCount = workflow->update_request.xUpdateManifest.xInstructions.ulStepsCount};

    // The order of the step results must match the order of the steps
    // in the the update manifest instructions.
    for (int32_t step_index = 0; step_index < update_results.ulStepResultsCount; step_index++)
    {
        update_results.pxStepResults[step_index].ulResultCode = 200;
        update_results.pxStepResults[step_index].ulExtendedResultCode = 200;
        update_results.pxStepResults[step_index].pucResultDetails = (uint8_t *)"device updated";
        update_results.pxStepResults[step_index].ulResultDetailsLength = sizeof("device updated") - 1;
    }

    AzureIoTResult_t result = azure_adu_send_agent_state(workflow->adu_context,
                                                         workflow->device_properties,
                                                         &workflow->update_request,
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