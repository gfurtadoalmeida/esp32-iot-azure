#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "example_iot_hub.h"
#include "esp32_iot_azure/azure_iot_hub.h"
#include "esp32_iot_azure/extension/azure_iot_hub_extension.h"
#include "esp32_iot_azure/extension/azure_iot_hub_client_properties_extension.h"
#include "esp32_iot_azure/extension/azure_iot_json_writer_extension.h"
#include "esp32_iot_azure/extension/azure_iot_json_reader_extension.h"

static const char TAG_EX_IOT[] = "EX_IOT_HUB";

static volatile bool s_turn_off_command_called = false;

static void callback_cloud_to_device_subscription(const AzureIoTHubClientCloudToDeviceMessageRequest_t *message, const void *callback_context);
static void callback_cloud_command_subscription(const AzureIoTHubClientCommandRequest_t *message, void *callback_context);
static void callback_cloud_properties_subscription(const AzureIoTHubClientPropertiesResponse_t *message, void *callback_context);

static AzureIoTResult_t device_report_initial_state(azure_iot_hub_context_t *iot, uint8_t temperature, uint8_t display_brigthness);
static AzureIoTResult_t device_change_state(azure_iot_hub_context_t *iot, const AzureIoTHubClientPropertiesResponse_t *message, uint32_t *version, uint16_t *new_display_brightness);
static AzureIoTResult_t device_report_state_changed(azure_iot_hub_context_t *iot, uint32_t version, uint16_t display_brightness);

bool example_iot_hub_run(const utf8_string_t *iot_hub_hostname,
                         const utf8_string_t *device_id,
                         const utf8_string_t *device_symmetric_key)
{
    bool success = false;
    AzureIoTHubClientOptions_t *iot_client_options = NULL;

    azure_iot_hub_context_t *iot = azure_iot_hub_create();

    if (azure_iot_hub_options_init(iot, &iot_client_options) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure initializing options");
        goto CLEAN_UP;
    }

    iot_client_options->pxComponentList = (AzureIoTHubClientComponent_t[2]){azureiothubCREATE_COMPONENT("temperatureSensor"), azureiothubCREATE_COMPONENT("display")};
    iot_client_options->ulComponentListLength = 2;

    if (azure_iot_hub_init(iot, iot_hub_hostname, device_id) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure initializing");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_auth_set_symmetric_key(iot, device_symmetric_key) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure setting symmetric key");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_connect(iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure connecting");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_cloud_to_device_message(iot, &callback_cloud_to_device_subscription, NULL) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure subscribing to cloud-to-device messages");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_command(iot, &callback_cloud_command_subscription, iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure subscribing to commands");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_subscribe_properties(iot, &callback_cloud_properties_subscription, iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure subscribing to properties");
        goto CLEAN_UP;
    }

    if (azure_iot_hub_request_properties_async(iot) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure requesting device properties document");
        goto CLEAN_UP;
    }

    if (device_report_initial_state(iot, 25, 95) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure reporting initial state");
        goto CLEAN_UP;
    }

    utf8_string_t telemetry_payload = UTF8_STRING_WITH_FIXED_LENGTH(20);
    utf8_string_t component_name = UTF8_STRING_FROM_LITERAL("temperatureSensor");

    while (!s_turn_off_command_called)
    {
        telemetry_payload.length = sprintf((char *)telemetry_payload.buffer, "{\"temp\":%d}", rand() % (28 + 1 - 18) + 18);

        azure_iot_hub_send_telemetry_from_component(iot, &component_name, &telemetry_payload, eAzureIoTHubMessageQoS1, NULL);

        azure_iot_hub_process_loop(iot);

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

    azure_iot_hub_context_t *iot = (azure_iot_hub_context_t *)callback_context;

    if (strncasecmp("restart", (const char *)message->pucCommandName, sizeof("restart") - 1) == 0)
    {
        ESP_LOGW(TAG_EX_IOT, "restarting device");

        if (azure_iot_hub_send_command_response(iot, message, NULL, 200) != eAzureIoTSuccess)
        {
            ESP_LOGE(TAG_EX_IOT, "failure sending command response");
        }

        s_turn_off_command_called = true;

        return;
    }

    utf8_string_t error_payload = UTF8_STRING_FROM_LITERAL("{\"message\":\"command unknown\"}");

    if (azure_iot_hub_send_command_response(iot, message, &error_payload, 404) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure sending command response");
    }
}

static void callback_cloud_properties_subscription(const AzureIoTHubClientPropertiesResponse_t *message, void *callback_context)
{
    azure_iot_hub_context_t *iot = (azure_iot_hub_context_t *)callback_context;
    uint32_t version = 0;
    uint16_t new_value = 0;

    ESP_LOGI(TAG_EX_IOT, "property message: %.*s", message->ulPayloadLength, (const char *)message->pvMessagePayload);

    switch (message->xMessageType)
    {
    case eAzureIoTHubPropertiesRequestedMessage:
        ESP_LOGI(TAG_EX_IOT, "azure_iot_hub_request_properties_async response = property document (desired + reported) sent by the server");

        if (device_change_state(iot, message, &version, &new_value) == eAzureIoTSuccess)
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

        if (device_change_state(iot, message, &version, &new_value) == eAzureIoTSuccess)
        {
            ESP_LOGI(TAG_EX_IOT, "device state changed");

            if (device_report_state_changed(iot, version, new_value) == eAzureIoTSuccess)
            {
                ESP_LOGI(TAG_EX_IOT, "response status sent");
            }
            else
            {
                ESP_LOGE(TAG_EX_IOT, "failure sending response status");
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

static AzureIoTResult_t device_report_initial_state(azure_iot_hub_context_t *iot,
                                                    uint8_t temperature,
                                                    uint8_t display_brigthness)
{
    utf8_string_t payload = UTF8_STRING_WITH_FIXED_LENGTH(AZURE_JSON_OBJECT_MIN_NEEDED_BYTES + AZURE_JSON_CALC_PROPERTY_NEEDED_BYTES("deviceStatus", 10) + AZURE_JSON_CALC_CLIENT_PROPERTY_MIN_COMPONENT_OBJECT_NEEDED_BYTES("temperatureSensor") + AZURE_JSON_CALC_PROPERTY_NEEDED_BYTES("temp", 2) + AZURE_JSON_CALC_CLIENT_PROPERTY_MIN_COMPONENT_OBJECT_NEEDED_BYTES("display") + AZURE_JSON_CALC_PROPERTY_NEEDED_BYTES("brightness", 3));

    AzureIoTJSONWriter_t json_writer;
    AzureIoTHubClient_t *iot_client = azure_iot_hub_get_context_client(iot);

    AZ_CHECK_BEGIN()
    AZ_CHECK(AzureIoTJSONWriter_Init(&json_writer, payload.buffer, payload.length))
    AZ_CHECK(AzureIoTJSONWriter_AppendBeginObject(&json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithStringValue(&json_writer,
                                                              (uint8_t *)"deviceStatus",
                                                              sizeof("deviceStatus") - 1,
                                                              (uint8_t *)"active",
                                                              sizeof("active")))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginComponent(iot_client,
                                                               &json_writer,
                                                               (uint8_t *)"temperatureSensor",
                                                               sizeof("temperatureSensor") - 1))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithInt32Value(&json_writer,
                                                             (uint8_t *)"temp",
                                                             sizeof("temp") - 1,
                                                             temperature))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndComponent(iot_client, &json_writer))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginComponent(iot_client,
                                                               &json_writer,
                                                               (uint8_t *)"display",
                                                               sizeof("display") - 1))
    AZ_CHECK(AzureIoTJSONWriter_AppendPropertyWithInt32Value(&json_writer,
                                                             (uint8_t *)"brightness",
                                                             sizeof("brightness") - 1,
                                                             display_brigthness))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndComponent(iot_client, &json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendEndObject(&json_writer))

    payload.length = AzureIoTJSONWriter_GetBytesUsed(&json_writer);

    AZ_CHECK(azure_iot_hub_send_properties_reported(iot, &payload, NULL))

    AZ_CHECK_RETURN_LAST()
}

static AzureIoTResult_t device_change_state(azure_iot_hub_context_t *iot,
                                            const AzureIoTHubClientPropertiesResponse_t *message,
                                            uint32_t *version,
                                            uint16_t *new_display_brightness)
{
    AzureIoTJSONReader_t json_reader;
    AzureIoTHubClient_t *iot_client = azure_iot_hub_get_context_client(iot);
    const uint8_t *component_name = NULL;
    uint32_t component_name_length = 0;

    AZ_CHECK_BEGIN()
    AZ_CHECK(AzureIoTJSONReader_Init(&json_reader, message->pvMessagePayload, message->ulPayloadLength))

    if (AzureIoTHubClientProperties_GetPropertiesVersion(iot_client, &json_reader, message->xMessageType, version))
    {
        ESP_LOGE(TAG_EX_IOT, "failure getting property version");
    }
    else
    {
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
            if (component_name_length == 0)
            {
                ESP_LOGW(TAG_EX_IOT, "unknown root property (we expect a component): %.*s", component_name_length, component_name);

                // We're expecting properties from component.
                // We have to skip over the property and value
                // to continue iterating.
                AzureIoTJSONReader_SkipPropertyAndValue(&json_reader);

                continue;
            }

            if (strncasecmp("display", (const char *)component_name, sizeof("display") - 1) != 0)
            {
                ESP_LOGW(TAG_EX_IOT, "unknown component: %.*s", component_name_length, component_name);

                // Unknown component name, we have to skip over the property
                // and value to continue iterating.
                AzureIoTJSONReader_SkipPropertyAndValue(&json_reader);

                continue;
            }

            if (AzureIoTJSONReader_TokenIsTextEqual(&json_reader,
                                                    (uint8_t *)"brightness",
                                                    sizeof("brightness") - 1))
            {
                AZ_CHECK(AzureIoTJSONReader_NextToken(&json_reader))
                AZ_CHECK(AzureIoTJSONReader_GetTokenUInt16(&json_reader, new_display_brightness))
                AZ_CHECK(AzureIoTJSONReader_NextToken(&json_reader))

                ESP_LOGI(TAG_EX_IOT, "display.brightness received: %d", *new_display_brightness);
            }
        }
    }

    AZ_CHECK_RETURN_LAST()
}

static AzureIoTResult_t device_report_state_changed(azure_iot_hub_context_t *iot, uint32_t version, uint16_t display_brightness)
{
    utf8_string_t payload = UTF8_STRING_WITH_FIXED_LENGTH(AZURE_JSON_OBJECT_MIN_NEEDED_BYTES + AZURE_JSON_CALC_CLIENT_PROPERTY_MIN_COMPONENT_OBJECT_NEEDED_BYTES("display") + AZURE_JSON_CALC_CLIENT_PROPERTY_MIN_RESPONSE_STATUS_PROPERTY_NEEDED_BYTES("brightness", NULL, 3));

    AzureIoTJSONWriter_t json_writer;
    AzureIoTHubClient_t *iot_client = azure_iot_hub_get_context_client(iot);

    AZ_CHECK_BEGIN()

    AZ_CHECK(AzureIoTJSONWriter_Init(&json_writer, payload.buffer, payload.length))
    AZ_CHECK(AzureIoTJSONWriter_AppendBeginObject(&json_writer))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginComponent(iot_client, &json_writer, (uint8_t *)"display", sizeof("display") - 1))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderBeginResponseStatus(iot_client,
                                                                    &json_writer,
                                                                    (uint8_t *)"brightness",
                                                                    sizeof("brightness") - 1,
                                                                    200,
                                                                    version,
                                                                    (uint8_t *)"success",
                                                                    sizeof("success") - 1))
    AZ_CHECK(AzureIoTJSONWriter_AppendInt32(&json_writer, display_brightness))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndResponseStatus(iot_client, &json_writer))
    AZ_CHECK(AzureIoTHubClientProperties_BuilderEndComponent(iot_client, &json_writer))
    AZ_CHECK(AzureIoTJSONWriter_AppendEndObject(&json_writer))

    payload.length = AzureIoTJSONWriter_GetBytesUsed(&json_writer);

    if ((AZ_CHECK_RESULT_VAR = azure_iot_hub_send_properties_reported(iot_client, &payload, NULL)) != eAzureIoTSuccess)
    {
        ESP_LOGE(TAG_EX_IOT, "failure reporting properties: 0x%08x", AZ_CHECK_RESULT_VAR);
    }

    AZ_CHECK_RETURN_LAST()
}