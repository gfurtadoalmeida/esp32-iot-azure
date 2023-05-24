#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "examples/adu/example_adu.h"
#include "esp32_iot_azure/azure_iot_hub.h"
#include "esp32_iot_azure/azure_iot_adu.h"
#include "esp32_iot_azure/azure_iot_adu_workflow.h"
#include "esp32_iot_azure/extension/azure_iot_hub_extension.h"
#include "esp32_iot_azure/extension/azure_iot_hub_client_properties_extension.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"
#include "esp32_iot_azure/extension/azure_iot_json_reader_extension.h"
#include "dtdl/temperaturecontroller.h"

typedef struct
{
    azure_iot_hub_context_t *iot_hub;
    azure_adu_context_t *adu;
    azure_adu_workflow_t *adu_workflow;
    utf8_string_t scratch_buffer;
    temperature_controller_status_t device_status;
    uint8_t display_brightness;
    bool restart_command_called;
} example_context_t;

static const char TAG_EX_ADU[] = "EX_IOT_ADU";

static example_context_t EXAMPLE_CONTEXT = {
    .adu = NULL,
    .iot_hub = NULL,
    .scratch_buffer = UTF8_STRING_WITH_FIXED_LENGTH(700),
    .device_status = TEMP_CONTROLLER_STATUS_NORMAL,
    .display_brightness = 50,
    .restart_command_called = false};
;

static void callback_cloud_properties_subscription(const AzureIoTHubClientPropertiesResponse_t *message, void *callback_context);

static AzureIoTResult_t device_report_initial_state(example_context_t *context);
static AzureIoTResult_t device_change_state(example_context_t *context, const AzureIoTHubClientPropertiesResponse_t *message, uint32_t *version);
static AzureIoTResult_t device_report_state_changed(example_context_t *context, uint32_t version);

bool example_adu_run(const utf8_string_t *iot_hub_hostname,
                     const utf8_string_t *device_id,
                     const utf8_string_t *device_symmetric_key)
{
    bool success = false;
    AzureIoTHubClientOptions_t *iot_client_options = NULL;
    AzureIoTADUClientOptions_t *adu_client_options = NULL;
    AzureIoTADUClientDeviceProperties_t adu_client_device_properties = {0};

    azure_iot_hub_context_t *iot = azure_iot_hub_create();
    azure_adu_context_t *adu = azure_adu_create(iot);
    azure_adu_workflow_t *adu_workflow = azure_adu_workflow_create(adu);

    example_context_t *example_context = &EXAMPLE_CONTEXT;
    example_context->adu = adu;
    example_context->adu_workflow = adu_workflow;
    example_context->iot_hub = iot;

    if (azure_iot_hub_options_init(iot, &iot_client_options) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure initializing IoT Hub options");
        goto CLEAN_UP;
    }

    iot_client_options->pxComponentList = (AzureIoTHubClientComponent_t[3]){
        azureiothubCREATE_COMPONENT(AZ_IOT_ADU_CLIENT_PROPERTIES_COMPONENT_NAME),
        azureiothubCREATE_COMPONENT(TEMP_CTRL_CMP_DISPLAY_NAME),
        azureiothubCREATE_COMPONENT(TEMP_CTRL_CMP_THERMOSTAT_NAME)};
    iot_client_options->ulComponentListLength = 3;

    if (azure_iot_hub_init(iot,
                           iot_hub_hostname->buffer,
                           (uint16_t)iot_hub_hostname->length,
                           device_id->buffer,
                           (uint16_t)device_id->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure initializing IoT Hub");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_auth_set_symmetric_key(iot, device_symmetric_key->buffer, device_symmetric_key->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure setting IoT Hub symmetric key");
        goto CLEAN_UP;
    }

    if (azure_adu_options_init(adu, &adu_client_options) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure initializing Device Update options");
        goto CLEAN_UP;
    }

    if (azure_adu_init(adu) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure initializing Device Update");
        goto CLEAN_UP;
    }

    if (azure_adu_device_properties_init(&adu_client_device_properties) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure initializing Device Update device properties");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_connect(iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure connecting to IoT Hub");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_properties(iot, &callback_cloud_properties_subscription, example_context) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure subscribing to properties");
        goto CLEAN_UP;
    }

    if (azure_adu_workflow_init_agent(adu_workflow, &adu_client_device_properties) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure initializing Device Update Workflow");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_request_properties_async(iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure requesting device properties document");
        goto CLEAN_UP;
    }

    if (device_report_initial_state(example_context) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure reporting initial state");
        goto CLEAN_UP;
    }

    utf8_string_t telemetry_payload = UTF8_STRING_FROM_LITERAL("{\"temp\":25}");

    while (true)
    {
        if (azure_iot_hub_send_telemetry_from_component(iot,
                                                        (uint8_t *)TEMP_CTRL_CMP_THERMOSTAT_PRP_TLY_TEMPERATURE_NAME,
                                                        sizeof_l(TEMP_CTRL_CMP_THERMOSTAT_PRP_TLY_TEMPERATURE_NAME),
                                                        telemetry_payload.buffer,
                                                        telemetry_payload.length,
                                                        eAzureIoTHubMessageQoS1,
                                                        NULL) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_ADU, "failure sending telemetry");
        }

        if (azure_iot_hub_process_loop(iot) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_ADU, "failure processing loop");
        }

        if (azure_adu_workflow_process_loop(adu_workflow) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_ADU, "failure processing workflow");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    azure_iot_hub_unsubscribe_properties(iot);

    success = true;
CLEAN_UP:
    azure_adu_workflow_free(adu_workflow);
    azure_adu_free(adu);
    azure_iot_hub_disconnect(iot);
    azure_iot_hub_deinit(iot);
    azure_iot_hub_free(iot);

    return success;
}

//
// SUBSCRIPTION CALLBACKS
//

static void callback_cloud_properties_subscription(const AzureIoTHubClientPropertiesResponse_t *message, void *callback_context)
{
    example_context_t *context = (example_context_t *)callback_context;
    uint32_t version = 0;

    ESP_LOGI(TAG_EX_ADU, "message status: %d", message->xMessageStatus);
    ESP_LOGI(TAG_EX_ADU, "message length: %d", message->ulPayloadLength);

    switch (message->xMessageType)
    {
    case eAzureIoTHubPropertiesRequestedMessage:
        ESP_LOGI(TAG_EX_ADU, "azure_iot_hub_request_properties_async response = property document (desired + reported) sent by the server");

        if (device_change_state(context, message, &version) == eAzureIoTSuccess)
        {
            ESP_LOGI(TAG_EX_ADU, "device state changed");
        }
        else
        {
            ESP_LOGE(TAG_EX_ADU, "failure changing device state");
        }

        break;

    case eAzureIoTHubPropertiesWritablePropertyMessage:
        ESP_LOGI(TAG_EX_ADU, "server wants to change a device property");

        if (device_change_state(context, message, &version) == eAzureIoTSuccess)
        {
            ESP_LOGI(TAG_EX_ADU, "device state changed");

            if (device_report_state_changed(context, version) == eAzureIoTSuccess)
            {
                ESP_LOGI(TAG_EX_ADU, "state change reported");
            }
            else
            {
                ESP_LOGE(TAG_EX_ADU, "failure reporting state change");
            }
        }
        else
        {
            ESP_LOGE(TAG_EX_ADU, "failure changing device state");
        }

        break;

    case eAzureIoTHubPropertiesReportedResponseMessage:
        ESP_LOGI(TAG_EX_ADU, "azure_iot_hub_send_properties_reported response = properties value stored at the server or error.");

        if (message->xMessageStatus != eAzureIoTStatusNoContent)
        {
            ESP_LOGW(TAG_EX_ADU, "failure reporting properties: %.*s", message->ulPayloadLength, (const char *)message->pvMessagePayload);
        }
        break;

    default:
        ESP_LOGE(TAG_EX_ADU, "unknown property message");
    }
}

//
// PROPERTY HANDLE
//

static AzureIoTResult_t device_report_initial_state(example_context_t *context)
{
    AzureIoTJSONWriter_t json_writer;
    AzureIoTHubClient_t *iot_client = azure_iot_hub_get_iot_client(context->iot_hub);

    AZ_CHECK_BEGIN()
    AZ_CHECK(AzureIoTJSONWriter_Init(&json_writer, context->scratch_buffer.buffer, context->scratch_buffer.length))
    AZ_CHECK(AzureIoTJSONWriter_AppendBeginObject(&json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithInt32Value(&json_writer,
                                                             (uint8_t *)TEMP_CTRL_PRP_DEVICE_STATUS_NAME,
                                                             sizeof_l(TEMP_CTRL_PRP_DEVICE_STATUS_NAME),
                                                             context->device_status))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginComponent(iot_client,
                                                               &json_writer,
                                                               (uint8_t *)TEMP_CTRL_CMP_DISPLAY_NAME,
                                                               sizeof_l(TEMP_CTRL_CMP_DISPLAY_NAME)))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithInt32Value(&json_writer,
                                                             (uint8_t *)TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME,
                                                             sizeof_l(TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME),
                                                             context->display_brightness))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndComponent(iot_client, &json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendEndObject(&json_writer))

    int32_t payload_length = AzureIoTJSONWriter_GetBytesUsed(&json_writer);

    AZ_CHECK(azure_iot_hub_send_properties_reported(context->iot_hub, context->scratch_buffer.buffer, payload_length, NULL))

    AZ_CHECK_RETURN_LAST()
}

static AzureIoTResult_t device_change_state(example_context_t *context,
                                            const AzureIoTHubClientPropertiesResponse_t *message,
                                            uint32_t *version)
{
    AzureIoTJSONReader_t json_reader;
    AzureIoTHubClient_t *iot_client = azure_iot_hub_get_iot_client(context->iot_hub);

    AZ_CHECK_BEGIN()
    AZ_CHECK(AzureIoTJSONReader_Init(&json_reader, message->pvMessagePayload, message->ulPayloadLength))

    if (AzureIoTHubClientProperties_GetPropertiesVersion(iot_client, &json_reader, message->xMessageType, version))
    {
        ESP_LOGE(TAG_EX_ADU, "failure getting property version");
    }
    else
    {
        const uint8_t *component_name = NULL;
        uint32_t component_name_length = 0;

        ESP_LOGI(TAG_EX_ADU, "desired property version: %d", *version);

        // Reset JSON reader to the beginning.
        AZ_CHECK(AzureIoTJSONReader_Init(&json_reader, message->pvMessagePayload, message->ulPayloadLength))

        while (AzureIoTHubClientProperties_GetNextComponentProperty(iot_client,
                                                                    &json_reader,
                                                                    message->xMessageType,
                                                                    eAzureIoTHubClientPropertyWritable,
                                                                    &component_name,
                                                                    &component_name_length) == eAzureIoTSuccess)
        {
            // We're expecting properties from a component.
            // We have to skip over the root property and value to continue iterating.
            if (component_name_length == 0)
            {
                ESP_LOGW(TAG_EX_ADU, "unknown root property (we expect a component): %.*s", component_name_length, component_name);

                AZ_CHECK(AzureIoTJSONReader_SkipPropertyAndValue(&json_reader))

                continue;
            }

            if (azure_adu_is_adu_component(context->adu, component_name, component_name_length))
            {
                AZ_CHECK(azure_adu_workflow_process_update_request(context->adu_workflow,
                                                                   &json_reader,
                                                                   *version))

                continue;
            }

            // We're expecting a "display" component.
            // We have to skip over the root property and value to continue iterating.
            if (strncasecmp(TEMP_CTRL_CMP_DISPLAY_NAME, (const char *)component_name, sizeof_l(TEMP_CTRL_CMP_DISPLAY_NAME)) != 0)
            {
                ESP_LOGW(TAG_EX_ADU, "unknown component: %.*s", component_name_length, component_name);

                AZ_CHECK(AzureIoTJSONReader_SkipPropertyAndValue(&json_reader))

                continue;
            }

            if (AzureIoTJSONReader_TokenIsTextEqual(&json_reader,
                                                    (uint8_t *)TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME,
                                                    sizeof_l(TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME)))
            {
                AZ_CHECK(AzureIoTJSONReader_NextToken(&json_reader))
                AZ_CHECK(AzureIoTJSONReader_GetTokenUInt8(&json_reader, &context->display_brightness))
                AZ_CHECK(AzureIoTJSONReader_NextToken(&json_reader))

                ESP_LOGI(TAG_EX_ADU, "display.brightness received: %d", context->display_brightness);
            }
        }
    }

    AZ_CHECK_RETURN_LAST()
}

static AzureIoTResult_t device_report_state_changed(example_context_t *context, uint32_t version)
{
    AzureIoTJSONWriter_t json_writer;
    AzureIoTHubClient_t *iot_client = azure_iot_hub_get_iot_client(context->iot_hub);

    AZ_CHECK_BEGIN()

    AZ_CHECK(AzureIoTJSONWriter_Init(&json_writer, context->scratch_buffer.buffer, context->scratch_buffer.length))
    AZ_CHECK(AzureIoTJSONWriter_AppendBeginObject(&json_writer))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginComponent(iot_client,
                                                               &json_writer,
                                                               (uint8_t *)TEMP_CTRL_CMP_DISPLAY_NAME,
                                                               sizeof_l(TEMP_CTRL_CMP_DISPLAY_NAME)))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginResponseStatus(iot_client,
                                                                    &json_writer,
                                                                    (uint8_t *)TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME,
                                                                    sizeof_l(TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME),
                                                                    200,
                                                                    version,
                                                                    (uint8_t *)"success",
                                                                    sizeof("success") - 1))
    AZ_CHECK(AzureIoTJSONWriter_AppendInt32(&json_writer, context->display_brightness))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndResponseStatus(iot_client, &json_writer))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndComponent(iot_client, &json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendEndObject(&json_writer))

    int32_t payload_length = AzureIoTJSONWriter_GetBytesUsed(&json_writer);

    if ((AZ_CHECK_RESULT_VAR = azure_iot_hub_send_properties_reported(context->iot_hub,
                                                                      context->scratch_buffer.buffer,
                                                                      payload_length,
                                                                      NULL)) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_ADU, "failure reporting properties: 0x%08x", AZ_CHECK_RESULT_VAR);
    }

    AZ_CHECK_RETURN_LAST()
}