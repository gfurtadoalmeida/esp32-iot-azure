#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "example_iot_hub.h"
#include "esp32_iot_azure/azure_iot_hub.h"
#include "esp32_iot_azure/extension/azure_iot_hub_extension.h"
#include "esp32_iot_azure/extension/azure_iot_hub_client_properties_extension.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"
#include "esp32_iot_azure/extension/azure_iot_json_reader_extension.h"
#include "dtdl/temperaturecontroller.h"

typedef struct
{
    azure_iot_hub_context_t *iot_hub;
    utf8_string_t scratch_buffer;
    temperature_controller_status_t device_status;
    uint8_t display_brightness;
    bool restart_command_called;
} example_context_t;

static const char TAG_EX_IOT[] = "EX_IOT_HUB";

static example_context_t EXAMPLE_CONTEXT = {
    .iot_hub = NULL,
    .scratch_buffer = UTF8_STRING_WITH_FIXED_LENGTH(700),
    .device_status = TEMP_CONTROLLER_STATUS_NORMAL,
    .display_brightness = 50,
    .restart_command_called = false};

static void callback_cloud_to_device_subscription(const AzureIoTHubClientCloudToDeviceMessageRequest_t *message, const void *callback_context);
static void callback_cloud_command_subscription(const AzureIoTHubClientCommandRequest_t *message, void *callback_context);
static void callback_cloud_properties_subscription(const AzureIoTHubClientPropertiesResponse_t *message, void *callback_context);

static AzureIoTResult_t device_report_initial_state(example_context_t *context);
static AzureIoTResult_t device_change_state(example_context_t *context, const AzureIoTHubClientPropertiesResponse_t *message, uint32_t *version);
static AzureIoTResult_t device_report_state_changed(example_context_t *context, uint32_t version);

bool example_iot_hub_run(const utf8_string_t *iot_hub_hostname,
                         const utf8_string_t *device_id,
                         const utf8_string_t *device_symmetric_key)
{
    bool success = false;
    AzureIoTHubClientOptions_t *iot_client_options = NULL;
    buffer_t buffer = {
        .length = 4096,
        .buffer = (uint8_t *)malloc(4096)};

    azure_iot_hub_context_t *iot = azure_iot_hub_create(&buffer);

    example_context_t *example_context = &EXAMPLE_CONTEXT;
    example_context->iot_hub = iot;

    if (azure_iot_hub_options_init(iot, &iot_client_options) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure initializing options");
        goto CLEAN_UP;
    }

    iot_client_options->pxComponentList = (AzureIoTHubClientComponent_t[2]){
        azureiothubCREATE_COMPONENT(TEMP_CTRL_CMP_DISPLAY_NAME),
        azureiothubCREATE_COMPONENT(TEMP_CTRL_CMP_THERMOSTAT_NAME)};
    iot_client_options->ulComponentListLength = 2;

    if (azure_iot_hub_init(iot,
                           iot_hub_hostname->buffer,
                           (uint16_t)iot_hub_hostname->length,
                           device_id->buffer,
                           (uint16_t)device_id->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure initializing");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_auth_set_symmetric_key(iot, device_symmetric_key->buffer, device_symmetric_key->length) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure setting symmetric key");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_connect(iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure connecting");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_cloud_to_device_message(iot, &callback_cloud_to_device_subscription, example_context) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure subscribing to cloud-to-device messages");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_command(iot, &callback_cloud_command_subscription, example_context) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure subscribing to commands");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_properties(iot, &callback_cloud_properties_subscription, example_context) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure subscribing to properties");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_request_properties_async(iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure requesting device properties document");
        goto CLEAN_UP;
    }

    if (device_report_initial_state(example_context) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure reporting initial state");
        goto CLEAN_UP;
    }

    utf8_string_t telemetry_payload = UTF8_STRING_WITH_FIXED_LENGTH(15);

    while (!example_context->restart_command_called)
    {
        telemetry_payload.length = sprintf((char *)telemetry_payload.buffer,
                                           "{\"" TEMP_CTRL_CMP_THERMOSTAT_PRP_TLY_TEMPERATURE_NAME "\":%d}",
                                           rand() % (28 + 1 - 18) + 18);

        if (azure_iot_hub_send_telemetry_from_component(iot,
                                                        (uint8_t *)TEMP_CTRL_CMP_THERMOSTAT_PRP_TLY_TEMPERATURE_NAME,
                                                        sizeof_l(TEMP_CTRL_CMP_THERMOSTAT_PRP_TLY_TEMPERATURE_NAME),
                                                        telemetry_payload.buffer,
                                                        telemetry_payload.length,
                                                        eAzureIoTHubMessageQoS1,
                                                        NULL) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_IOT, "failure sending telemetry");
        }

        if (azure_iot_hub_process_loop(iot) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_IOT, "failure processing loop");
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    azure_iot_hub_unsubscribe_cloud_to_device_message(iot);
    azure_iot_hub_unsubscribe_command(iot);
    azure_iot_hub_unsubscribe_properties(iot);

    success = true;
CLEAN_UP:
    azure_iot_hub_disconnect(iot);
    azure_iot_hub_deinit(iot);
    azure_iot_hub_free(iot);

    return success;
}

//
// SUBSCRIPTION CALLBACKS
//

static void callback_cloud_to_device_subscription(const AzureIoTHubClientCloudToDeviceMessageRequest_t *message, const void *callback_context)
{
    ESP_LOGI(TAG_EX_IOT, "cloud-to-device message: %.*s", message->ulPayloadLength, (const char *)message->pvMessagePayload);
}

static void callback_cloud_command_subscription(const AzureIoTHubClientCommandRequest_t *message, void *callback_context)
{
    ESP_LOGI(TAG_EX_IOT, "command message: %.*s", message->usCommandNameLength, (const char *)message->pucCommandName);

    example_context_t *context = (example_context_t *)callback_context;

    // The "restart" command belongs to the root interface (TemperatureController),
    // therefore no component name must be sent.
    if (message->usComponentNameLength == 0 && strncasecmp(TEMP_CTRL_CMD_RESTART_NAME, (const char *)message->pucCommandName, sizeof_l(TEMP_CTRL_CMD_RESTART_NAME)) == 0)
    {
        ESP_LOGW(TAG_EX_IOT, "restarting device");

        if (azure_iot_hub_send_command_response(context->iot_hub, message, NULL, 0, 200) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_IOT, "failure sending command response");
        }

        context->restart_command_called = true;

        return;
    }

    utf8_string_t error_payload = UTF8_STRING_FROM_LITERAL("{\"message\":\"command unknown\"}");

    if (azure_iot_hub_send_command_response(context->iot_hub, message, error_payload.buffer, error_payload.length, 404) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure sending command response");
    }
}

static void callback_cloud_properties_subscription(const AzureIoTHubClientPropertiesResponse_t *message, void *callback_context)
{
    example_context_t *context = (example_context_t *)callback_context;
    uint32_t version = 0;

    ESP_LOGI(TAG_EX_IOT, "message status: %d", message->xMessageStatus);
    ESP_LOGI(TAG_EX_IOT, "message length: %d", message->ulPayloadLength);

    switch (message->xMessageType)
    {
    case eAzureIoTHubPropertiesRequestedMessage:
        ESP_LOGI(TAG_EX_IOT, "azure_iot_hub_request_properties_async response = property document (desired + reported) sent by the server");

        if (device_change_state(context, message, &version) == eAzureIoTSuccess)
        {
            ESP_LOGI(TAG_EX_IOT, "device state changed");
        }
        else
        {
            ESP_LOGE(TAG_EX_IOT, "failure changing device state");
        }

        break;

    case eAzureIoTHubPropertiesWritablePropertyMessage:
        ESP_LOGI(TAG_EX_IOT, "server wants to change a device property");

        if (device_change_state(context, message, &version) == eAzureIoTSuccess)
        {
            ESP_LOGI(TAG_EX_IOT, "device state changed");

            if (device_report_state_changed(context, version) == eAzureIoTSuccess)
            {
                ESP_LOGI(TAG_EX_IOT, "state change reported");
            }
            else
            {
                ESP_LOGE(TAG_EX_IOT, "failure reporting state change");
            }
        }
        else
        {
            ESP_LOGE(TAG_EX_IOT, "failure changing device state");
        }

        break;

    case eAzureIoTHubPropertiesReportedResponseMessage:
        ESP_LOGI(TAG_EX_IOT, "azure_iot_hub_send_properties_reported response = properties value stored at the server or error.");

        if (message->xMessageStatus != eAzureIoTStatusNoContent)
        {
            ESP_LOGW(TAG_EX_IOT, "failure reporting properties: %.*s", message->ulPayloadLength, (const char *)message->pvMessagePayload);
        }
        break;

    default:
        ESP_LOGE(TAG_EX_IOT, "unknown property message");
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
        ESP_LOGE(TAG_EX_IOT, "failure getting property version");
    }
    else
    {
        const uint8_t *component_name = NULL;
        uint32_t component_name_length = 0;

        ESP_LOGI(TAG_EX_IOT, "desired property version: %d", *version);

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
                ESP_LOGW(TAG_EX_IOT, "unknown root property (we expect a component): %.*s", component_name_length, component_name);

                AZ_CHECK(AzureIoTJSONReader_SkipPropertyAndValue(&json_reader));

                continue;
            }

            // We're expecting a "display" component.
            // We have to skip over the root property and value to continue iterating.
            if (strncasecmp(TEMP_CTRL_CMP_DISPLAY_NAME, (const char *)component_name, sizeof_l(TEMP_CTRL_CMP_DISPLAY_NAME)) != 0)
            {
                ESP_LOGW(TAG_EX_IOT, "unknown component: %.*s", component_name_length, component_name);

                AZ_CHECK(AzureIoTJSONReader_SkipPropertyAndValue(&json_reader));

                continue;
            }

            if (AzureIoTJSONReader_TokenIsTextEqual(&json_reader,
                                                    (uint8_t *)TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME,
                                                    sizeof_l(TEMP_CTRL_CMP_DISPLAY_PRP_BRIGHTNESS_NAME)))
            {
                AZ_CHECK(AzureIoTJSONReader_NextToken(&json_reader))
                AZ_CHECK(AzureIoTJSONReader_GetTokenUInt8(&json_reader, &context->display_brightness))
                AZ_CHECK(AzureIoTJSONReader_NextToken(&json_reader))

                ESP_LOGI(TAG_EX_IOT, "display.brightness received: %d", context->display_brightness);
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
        ESP_LOGE(TAG_EX_IOT, "failure reporting properties: 0x%08x", AZ_CHECK_RESULT_VAR);
    }

    AZ_CHECK_RETURN_LAST()
}