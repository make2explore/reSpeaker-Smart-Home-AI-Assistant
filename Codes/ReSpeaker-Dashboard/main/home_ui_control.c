#include "home_ui_control.h"

#include <string.h>
#include <stdint.h>

#include "esp_http_client.h"
#include "esp_log.h"

#include "home_control.h"

#include "ui.h"

#define TAG "HOME_UI_CONTROL"

static bool device_state[HOME_DEVICE_COUNT] = {false};
static bool device_state_valid[HOME_DEVICE_COUNT] = {false};
static bool ui_syncing = false;
static bool fan_animation_running = false;
static lv_anim_t *fan_animation = NULL;
static bool controller_online = false;

static float environment_temperature = 0.0f;
static float environment_humidity = 0.0f;
static bool environment_valid = false;

static int controller_failure_count = 0;

static void home_ui_switch_event_cb(lv_event_t *e);
static void home_ui_fan_start(void);
static void home_ui_fan_stop(void);

static const char *device_name(home_device_t device)
{
    switch (device) {
        case HOME_DEVICE_LIGHT:
            return "light";

        case HOME_DEVICE_FAN:
            return "fan";

        case HOME_DEVICE_TV:
            return "tv";

        case HOME_DEVICE_AC:
            return "ac";

        default:
            return "unknown";
    }
}

static bool parse_device_state(const char *response,
                               home_device_t device,
                               bool *on)
{
    if (response == NULL || on == NULL) {
        return false;
    }

    const char *name = device_name(device);

    char on_pattern[32];
    char off_pattern[32];

    snprintf(
        on_pattern,
        sizeof(on_pattern),
        "\"%s\":\"on\"",
        name);

    snprintf(
        off_pattern,
        sizeof(off_pattern),
        "\"%s\":\"off\"",
        name);

    if (strstr(response, on_pattern) != NULL) {
        *on = true;
        return true;
    }

    if (strstr(response, off_pattern) != NULL) {
        *on = false;
        return true;
    }

    return false;
}

static bool request_device_state(home_device_t device,
                                 bool on)
{
    char url[160];

    snprintf(
        url,
        sizeof(url),
        "http://home-controller.local/api/device/%s/%s",
        device_name(device),
        on ? "on" : "off");

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 3000,
    };

    esp_http_client_handle_t client =
        esp_http_client_init(&config);

    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return false;
    }

    esp_err_t err =
        esp_http_client_perform(client);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "HTTP request failed: %s",
            esp_err_to_name(err));

        esp_http_client_cleanup(client);
        return false;
    }

    int status_code =
        esp_http_client_get_status_code(client);

    esp_http_client_cleanup(client);

    if (status_code < 200 || status_code >= 300) {
        ESP_LOGE(
            TAG,
            "Unexpected HTTP status: %d",
            status_code);
        return false;
    }

    device_state[device] = on;
    device_state_valid[device] = true;

    return true;
}

static bool request_all_device_status(void)
{
    const char *url =
        "http://home-controller.local/api/devices/status";

    esp_http_client_config_t config = {
        .url = url,
        .timeout_ms = 3000,
    };

    esp_http_client_handle_t client =
        esp_http_client_init(&config);

    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return false;
    }

    esp_err_t err =
        esp_http_client_open(client, 0);

    if (err != ESP_OK) {
        ESP_LOGE(
            TAG,
            "HTTP open failed: %s",
            esp_err_to_name(err));

        esp_http_client_cleanup(client);
        return false;
    }

    int content_length =
        esp_http_client_fetch_headers(client);

    if (content_length < 0) {
        ESP_LOGE(TAG, "Failed to fetch HTTP headers");

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    int status_code =
        esp_http_client_get_status_code(client);

    if (status_code != 200) {
        ESP_LOGE(
            TAG,
            "Unexpected HTTP status: %d",
            status_code);

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    char response[256] = {0};

    int len =
        esp_http_client_read_response(
            client,
            response,
            sizeof(response) - 1);

    if (len < 0) {
        ESP_LOGE(TAG, "Failed to read device status response");

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    response[len] = '\0';

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    for (int i = 0; i < HOME_DEVICE_COUNT; i++) {
        bool state = false;

        if (parse_device_state(
                response,
                (home_device_t)i,
                &state)) {

            device_state[i] = state;
            device_state_valid[i] = true;
        }
    }

    return true;
}

static void home_ui_status_task(void *arg)
{
    while (true) {

        if (request_all_device_status()) {
            controller_failure_count = 0;
            controller_online = true;
        } else {
            controller_failure_count++;

            ESP_LOGW(
                TAG,
                "Controller status request failed (%d consecutive failure%s)",
                controller_failure_count,
                controller_failure_count == 1 ? "" : "s");

            if (controller_failure_count >= 3) {
                controller_online = false;
            }
        }

        float temperature = 0.0f;
        float humidity = 0.0f;

        if (home_environment_get(
                &temperature,
                &humidity)) {

            environment_temperature = temperature;
            environment_humidity = humidity;
            environment_valid = true;
        } else {
            ESP_LOGW(
                TAG,
                "Failed to update environment data");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void home_ui_control_init(void)
{
    xTaskCreate(
        home_ui_status_task,
        "home_ui_status",
        4096,
        NULL,
        5,
        NULL);
}

bool home_ui_set_device_state(home_device_t device,
                              bool on)
{
    if (device >= HOME_DEVICE_COUNT) {
        return false;
    }

    return request_device_state(device, on);
}

bool home_ui_get_device_state(home_device_t device,
                              bool *on)
{
    if (device >= HOME_DEVICE_COUNT ||
        on == NULL ||
        !device_state_valid[device]) {
        return false;
    }

    *on = device_state[device];
    return true;
}

/**
 * Synchronize the current cached device states with the LVGL UI.
 *
 * This function must be called from the LVGL/UI context.
 */
void home_ui_update_display(void)
{
    bool state = false;

    bool controller_online_state =
        home_ui_is_controller_online();

    if (controller_online_state) {
        lv_obj_clear_state(
            uic_ControllerIndicator,
            LV_STATE_USER_1);

        lv_label_set_text(
            uic_ControllerLabel,
            "Online");

        lv_obj_set_style_text_color(
            uic_ControllerLabel,
            lv_color_hex(0x00FF00),
            LV_PART_MAIN | LV_STATE_DEFAULT);

    } else {
        lv_obj_add_state(
            uic_ControllerIndicator,
            LV_STATE_USER_1);

        lv_label_set_text(
            uic_ControllerLabel,
            "Offline");

        lv_obj_set_style_text_color(
            uic_ControllerLabel,
            lv_color_hex(0xFF0000),
            LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    home_ui_set_syncing(true);

    if (home_ui_get_device_state(HOME_DEVICE_LIGHT, &state)) {
        bool current =
            lv_obj_has_state(ui_Switch1, LV_STATE_CHECKED);

        if (current != state) {
            if (state) {
                lv_obj_add_state(ui_Switch1, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(ui_Switch1, LV_STATE_CHECKED);
            }
        }
    }

    if (home_ui_get_device_state(HOME_DEVICE_FAN, &state)) {
        bool current =
            lv_obj_has_state(ui_Switch2, LV_STATE_CHECKED);

        if (current != state) {
            if (state) {
                lv_obj_add_state(
                    ui_Switch2,
                    LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(
                    ui_Switch2,
                    LV_STATE_CHECKED);
            }
        }

        if (state) {
            home_ui_fan_start();
        } else {
            home_ui_fan_stop();
        }
    }

    if (home_ui_get_device_state(HOME_DEVICE_TV, &state)) {
        bool current =
            lv_obj_has_state(ui_Switch3, LV_STATE_CHECKED);

        if (current != state) {
            if (state) {
                lv_obj_add_state(ui_Switch3, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(ui_Switch3, LV_STATE_CHECKED);
            }
        }
    }

    if (home_ui_get_device_state(HOME_DEVICE_AC, &state)) {
        bool current =
            lv_obj_has_state(ui_Switch4, LV_STATE_CHECKED);

        if (current != state) {
            if (state) {
                lv_obj_add_state(ui_Switch4, LV_STATE_CHECKED);
            } else {
                lv_obj_clear_state(ui_Switch4, LV_STATE_CHECKED);
            }
        }
    }

        if (environment_valid) {

        char temperature_text[32];
        char humidity_text[32];

        snprintf(
            temperature_text,
            sizeof(temperature_text),
            "%.2f C",
            environment_temperature);

        snprintf(
            humidity_text,
            sizeof(humidity_text),
            "%.2f %%",
            environment_humidity);

        lv_label_set_text(
            uic_temp,
            temperature_text);

        lv_label_set_text(
            uic_humi,
            humidity_text);

        /*
         * Temperature arc:
         * 0-50 °C mapped to 0-100.
         */
        int temperature_arc =
            (int)((environment_temperature / 50.0f) * 100.0f);

        if (temperature_arc < 0) {
            temperature_arc = 0;
        }

        if (temperature_arc > 100) {
            temperature_arc = 100;
        }

        lv_arc_set_value(
            uic_tempArc,
            temperature_arc);

        /*
         * Humidity is naturally 0-100%.
         */
        int humidity_arc =
            (int)environment_humidity;

        if (humidity_arc < 0) {
            humidity_arc = 0;
        }

        if (humidity_arc > 100) {
            humidity_arc = 100;
        }

        lv_arc_set_value(
            uic_humiArc,
            humidity_arc);
    }

    home_ui_set_syncing(false);
}

void home_ui_set_syncing(bool syncing)
{
    ui_syncing = syncing;
}

bool home_ui_is_syncing(void)
{
    return ui_syncing;
}

void home_ui_bind_controls(void)
{
    /*
     * Register our application callback for all four switches.
     *
     * We leave SquareLine's generated callbacks in place for now.
     * The application callback is registered after them and handles
     * the actual smart-home control.
     */
    lv_obj_add_event_cb(
        ui_Switch1,
        home_ui_switch_event_cb,
        LV_EVENT_VALUE_CHANGED,
        (void *)(intptr_t)HOME_DEVICE_LIGHT);

    lv_obj_add_event_cb(
        ui_Switch2,
        home_ui_switch_event_cb,
        LV_EVENT_VALUE_CHANGED,
        (void *)(intptr_t)HOME_DEVICE_FAN);

    lv_obj_add_event_cb(
        ui_Switch3,
        home_ui_switch_event_cb,
        LV_EVENT_VALUE_CHANGED,
        (void *)(intptr_t)HOME_DEVICE_TV);

    lv_obj_add_event_cb(
        ui_Switch4,
        home_ui_switch_event_cb,
        LV_EVENT_VALUE_CHANGED,
        (void *)(intptr_t)HOME_DEVICE_AC);
}

static void home_ui_switch_event_cb(lv_event_t *e)
{
    if (home_ui_is_syncing()) {
        return;
    }

    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code != LV_EVENT_VALUE_CHANGED) {
        return;
    }

    lv_obj_t *target = lv_event_get_target(e);

    home_device_t device =
        (home_device_t)(intptr_t)lv_event_get_user_data(e);

    bool on =
        lv_obj_has_state(target, LV_STATE_CHECKED);

    if (!home_ui_set_device_state(device, on)) {

        /*
         * Restore the previous state if the HTTP request fails.
         */
        home_ui_set_syncing(true);

        if (on) {
            lv_obj_clear_state(target, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(target, LV_STATE_CHECKED);
        }

        home_ui_set_syncing(false);

        return;
    }

    /*
     * Preserve the Light visual effect that SquareLine generated.
     */
    if (device == HOME_DEVICE_LIGHT) {

        if (on) {
            _ui_state_modify(
                ui_Panel2,
                LV_STATE_USER_1,
                _UI_MODIFY_STATE_TOGGLE);
        } else {
            _ui_state_modify(
                ui_Panel2,
                LV_STATE_USER_1,
                _UI_MODIFY_STATE_REMOVE);
        }
    }

    /*
     * Update the fan animation immediately after a successful
     * touchscreen command. The normal synchronization timer will
     * continue to keep the UI aligned with the actual C6 state.
     */
    if (device == HOME_DEVICE_FAN) {
        if (on) {
            home_ui_fan_start();
        } else {
            home_ui_fan_stop();
        }
    }
}

static void home_ui_fan_start(void)
{
    if (fan_animation_running) {
        return;
    }

    fan_animation =
        rotateFan_Animation(ui_Image3, 0);

    fan_animation_running = true;
}

static void home_ui_fan_stop(void)
{
    if (!fan_animation_running) {
        return;
    }

    if (fan_animation != NULL) {
        lv_anim_custom_del(
            fan_animation,
            NULL);

        fan_animation = NULL;
    }

    fan_animation_running = false;

    lv_img_set_angle(ui_Image3, 0);
}

bool home_ui_is_controller_online(void)
{
    return controller_online;
}