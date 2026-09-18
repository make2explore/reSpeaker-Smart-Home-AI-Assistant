#include "home_control.h"

#include <stdio.h>
#include <string.h>

#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define TAG "HOME_CONTROL"

#define WIFI_CONNECTED_BIT BIT0

static EventGroupHandle_t wifi_event_group;
static char controller_base_url[128];

static bool cached_light_state = false;
static bool cached_light_state_valid = false;

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();

    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {

        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGW(TAG, "Wi-Fi disconnected, retrying...");
        esp_wifi_connect();

    } else if (event_base == IP_EVENT &&
               event_id == IP_EVENT_STA_GOT_IP) {

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

        ESP_LOGI(TAG,
                 "Wi-Fi connected, IP: " IPSTR,
                 IP2STR(&event->ip_info.ip));

        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static bool wait_for_wifi(uint32_t timeout_ms)
{
    EventBits_t bits = xEventGroupWaitBits(
        wifi_event_group,
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        pdMS_TO_TICKS(timeout_ms));

    return (bits & WIFI_CONNECTED_BIT) != 0;
}

static bool http_request(const char *path)
{
    if (!wait_for_wifi(10000)) {
        ESP_LOGE(TAG, "Wi-Fi is not connected");
        return false;
    }

    char url[160];

    snprintf(url,
         sizeof(url),
         "%s%s",
         controller_base_url,
         path);
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
        ESP_LOGE(TAG,
                 "HTTP request failed: %s",
                 esp_err_to_name(err));

        esp_http_client_cleanup(client);
        return false;
    }

    int status_code =
        esp_http_client_get_status_code(client);

    esp_http_client_cleanup(client);

    return (status_code >= 200 && status_code < 300);
}

static void home_status_poll_task(void *arg)
{
    while (true) {
        bool state = false;

        if (home_light_get_status(&state)) {
            cached_light_state = state;
            cached_light_state_valid = true;
        } else {
            ESP_LOGW(TAG, "Failed to update light status");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void home_control_init(void)
{
    snprintf(controller_base_url,
             sizeof(controller_base_url),
             "http://%s",
             CONFIG_HOME_CONTROLLER_HOST);

    ESP_LOGI(TAG,
             "Home controller: %s",
             controller_base_url);

    wifi_event_group = xEventGroupCreate();

    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);

    if (wifi_event_group == NULL) {
        ESP_LOGE(TAG, "Failed to create Wi-Fi event group");
        return;
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifi_init_cfg =
        WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(
        esp_wifi_init(&wifi_init_cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL));

    ESP_ERROR_CHECK(
        esp_event_handler_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold = {
                .authmode = WIFI_AUTH_WPA2_PSK,
            },
        },
    };

    strncpy(
        (char *)wifi_config.sta.ssid,
        CONFIG_HOME_WIFI_SSID,
        sizeof(wifi_config.sta.ssid));

    strncpy(
        (char *)wifi_config.sta.password,
        CONFIG_HOME_WIFI_PASSWORD,
        sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(
        esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(
        esp_wifi_set_config(
            WIFI_IF_STA,
            &wifi_config));

    ESP_ERROR_CHECK(
        esp_wifi_start());

    ESP_LOGI(TAG, "Connecting to Wi-Fi...");

}

bool home_light_set(bool on)
{
    if (on) {
        return http_request("/api/light/on");
    }

    return http_request("/api/light/off");
}

bool home_light_get_status(bool *on)
{
    if (on == NULL) {
        return false;
    }

    if (!wait_for_wifi(10000)) {
        ESP_LOGE(TAG, "Wi-Fi is not connected");
        return false;
    }

    char url[160];

    snprintf(url,
             sizeof(url),
             "%s/api/light/status",
             controller_base_url);

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
        ESP_LOGE(TAG,
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
        ESP_LOGE(TAG,
                 "Unexpected HTTP status: %d",
                 status_code);

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    char response[128] = {0};

    int len =
        esp_http_client_read_response(
            client,
            response,
            sizeof(response) - 1);

    if (len < 0) {
        ESP_LOGE(TAG, "Failed to read HTTP response");

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    response[len] = '\0';

    *on =
        (strstr(response, "\"state\":\"on\"") != NULL);

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    return true;
}

bool home_light_get_cached_status(bool *on)
{
    if (on == NULL || !cached_light_state_valid) {
        return false;
    }

    *on = cached_light_state;
    return true;
}

bool home_environment_get(float *temperature, float *humidity)
{
    if (temperature == NULL || humidity == NULL) {
        return false;
    }

    if (!wait_for_wifi(10000)) {
        ESP_LOGE(TAG, "Wi-Fi is not connected");
        return false;
    }

    char url[160];

    snprintf(
        url,
        sizeof(url),
        "%s/api/environment",
        controller_base_url);

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
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
            "Environment HTTP open failed: %s",
            esp_err_to_name(err));

        esp_http_client_cleanup(client);
        return false;
    }

    int content_length =
        esp_http_client_fetch_headers(client);

    if (content_length < 0) {
        ESP_LOGE(
            TAG,
            "Failed to fetch environment HTTP headers");

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    int status_code =
        esp_http_client_get_status_code(client);

    if (status_code != 200) {
        ESP_LOGE(
            TAG,
            "Unexpected environment HTTP status: %d",
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
        ESP_LOGE(
            TAG,
            "Failed to read environment response");

        esp_http_client_close(client);
        esp_http_client_cleanup(client);
        return false;
    }

    response[len] = '\0';

    esp_http_client_close(client);
    esp_http_client_cleanup(client);

    ESP_LOGI(
        TAG,
        "Environment response: %s",
        response);

    float temp = 0.0f;
    float hum = 0.0f;

    if (sscanf(
            response,
            "{\"success\":true,\"temperature\":%f,\"humidity\":%f}",
            &temp,
            &hum) == 2) {

        *temperature = temp;
        *humidity = hum;

        return true;
    }

    ESP_LOGE(
        TAG,
        "Failed to parse environment response");

    return false;
}