#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_event.h>
#include "application.h"

#define TAG "main"

// 声明 C 语言编写的初始化函数（非 Wi‑Fi）
extern "C" {
    void audio_hardware_init(void);
}

extern "C" void app_main(void)
{
    // 1. 初始化事件循环
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 2. 唤醒 ReSpeaker XVF3800 硬件 (必须在 Start 之前)
    audio_hardware_init();

    // 3. 初始化 NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 4. 启动小智应用（Wi‑Fi 在 Board::StartNetwork() 中初始化）
    Application::GetInstance().Start();
}