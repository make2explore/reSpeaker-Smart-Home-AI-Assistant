#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/i2c_master.h>

#include "shared_i2c_bus.h"
#include "config.h"

static const char *TAG = "AUDIO_BSP";

void audio_hardware_init(void) {
    i2c_master_bus_handle_t bus = shared_i2c_master_bus_get();
    if (!bus) {
        ESP_LOGW(TAG, "Shared I2C bus unavailable; skip audio hw init");
        return;
    }

#if defined(XVF3800_I2C_ADDR)
    esp_err_t ret = i2c_master_probe(bus, XVF3800_I2C_ADDR, pdMS_TO_TICKS(200));
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "XVF3800 detected at 0x%02X", (unsigned)XVF3800_I2C_ADDR);
    } else {
        ESP_LOGW(TAG, "XVF3800 probe failed at 0x%02X: %s", (unsigned)XVF3800_I2C_ADDR, esp_err_to_name(ret));
    }
#else
    ESP_LOGI(TAG, "XVF3800_I2C_ADDR not defined; nothing to probe");
#endif
}
