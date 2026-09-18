#include "home_clock.h"

#include <stdio.h>
#include <time.h>

#include "esp_log.h"
#include "esp_sntp.h"

#include "ui.h"

#define TAG "HOME_CLOCK"

#define TIME_ZONE "IST-5:30"

static bool time_synced = false;

void home_clock_init(void)
{
    setenv("TZ", TIME_ZONE, 1);
    tzset();

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");

    esp_sntp_init();

    ESP_LOGI(TAG, "SNTP started");
}

void home_clock_update_display(void)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    /*
     * Before NTP synchronization, the system clock may not be valid.
     * Wait for a sensible year before displaying the date/time.
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