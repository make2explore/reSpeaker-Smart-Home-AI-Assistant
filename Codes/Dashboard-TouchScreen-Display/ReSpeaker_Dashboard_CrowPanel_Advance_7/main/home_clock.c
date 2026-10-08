#include "home_clock.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ui.h"

#define TAG "HOME_CLOCK"

#define TIME_ZONE "IST-5:30"
#define NTP_SERVER "pool.ntp.org"

static bool time_synced = false;
static bool sntp_started = false;

static bool home_clock_wifi_has_ip(void)
{
    esp_netif_t *netif =
        esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");

    if (netif == NULL) {
        return false;
    }

    esp_netif_ip_info_t ip_info;

    if (esp_netif_get_ip_info(netif, &ip_info) != ESP_OK) {
        return false;
    }

    return ip_info.ip.addr != 0;
}

static void home_clock_sntp_task(void *arg)
{
    /*
     * Wait until the Dashboard has actually connected to Wi-Fi.
     * This is important because home_clock_init() can run before
     * the Wi-Fi provisioning/connection task has completed.
     */
    while (!home_clock_wifi_has_ip()) {
        ESP_LOGI(TAG, "Waiting for Wi-Fi connection before starting SNTP...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    if (!sntp_started) {
        setenv("TZ", TIME_ZONE, 1);
        tzset();

        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, NTP_SERVER);
        esp_sntp_init();

        sntp_started = true;

        ESP_LOGI(
            TAG,
            "SNTP started after Wi-Fi connection (timezone: %s)",
            TIME_ZONE);
    }

    /*
     * Wait for a valid time. The display updater can continue running
     * independently; it simply ignores the clock until the year is valid.
     */
    while (!time_synced) {
        time_t now;
        struct tm timeinfo;

        time(&now);
        localtime_r(&now, &timeinfo);

        if (timeinfo.tm_year >= (2025 - 1900)) {
            time_synced = true;

            ESP_LOGI(
                TAG,
                "Time synchronized: %04d-%02d-%02d %02d:%02d:%02d",
                timeinfo.tm_year + 1900,
                timeinfo.tm_mon + 1,
                timeinfo.tm_mday,
                timeinfo.tm_hour,
                timeinfo.tm_min,
                timeinfo.tm_sec);

            break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    vTaskDelete(NULL);
}

void home_clock_init(void)
{
    /*
     * Set the timezone immediately so localtime_r() uses IST even
     * while SNTP is waiting for the network.
     */
    setenv("TZ", TIME_ZONE, 1);
    tzset();

    xTaskCreate(
        home_clock_sntp_task,
        "home_clock_sntp",
        4096,
        NULL,
        5,
        NULL);
}

void home_clock_update_display(void)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    /*
     * Before NTP synchronization, the system clock may not be valid.
     * Keep the existing SquareLine placeholder text until a sensible
     * year is available.
     */
    if (timeinfo.tm_year < (2025 - 1900)) {
        return;
    }

    if (!time_synced) {
        time_synced = true;

        ESP_LOGI(
            TAG,
            "Time synchronized: %04d-%02d-%02d %02d:%02d:%02d",
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday,
            timeinfo.tm_hour,
            timeinfo.tm_min,
            timeinfo.tm_sec);
    }

    char time_text[16];
    char day_text[16];
    char date_text[32];

    strftime(
        time_text,
        sizeof(time_text),
        "%H:%M:%S",
        &timeinfo);

    strftime(
        day_text,
        sizeof(day_text),
        "%A",
        &timeinfo);

    strftime(
        date_text,
        sizeof(date_text),
        "%d %B %Y",
        &timeinfo);

    lv_label_set_text(ui_Label1, time_text);
    lv_label_set_text(ui_Label2, day_text);
    lv_label_set_text(ui_Label3, date_text);
}
