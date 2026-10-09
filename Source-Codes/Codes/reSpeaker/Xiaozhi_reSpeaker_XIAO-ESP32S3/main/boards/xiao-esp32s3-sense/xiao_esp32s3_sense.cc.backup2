#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h" 
#include "display/oled_display.h"
#include "mcp_server.h"
#include "lamp_controller.h"
#include "esp32_camera.h"

#include "shared_i2c_bus.h"

#include "led/gpio_led.h"
#include <wifi_station.h>
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/gpio.h>
#include <driver/i2s_std.h> 

#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>

#define TAG "XIAO_S3_Sense"

// ==========================================
// 自定义 AudioCodec 类：适配 ReSpeaker I2S
// ==========================================
class ReSpeakerAudioCodec : public NoAudioCodecDuplex {
public:
    ReSpeakerAudioCodec()
        : NoAudioCodecDuplex(AUDIO_INPUT_SAMPLE_RATE,
              AUDIO_OUTPUT_SAMPLE_RATE,
              AUDIO_I2S_BCK_IO,
              AUDIO_I2S_WS_IO,
              AUDIO_I2S_DO_IO,
              AUDIO_I2S_DI_IO) {
        ESP_LOGI(TAG, "ReSpeaker I2S Driver Initialized (%dHz, 32bit)", AUDIO_INPUT_SAMPLE_RATE);
    }

    void SetOutputVolume(int volume) override {
        // 硬件音量已在 audio_bsp.c 设置，此处留空
        (void)volume;
    }
};


LV_FONT_DECLARE(font_puhui_14_1);
LV_FONT_DECLARE(font_awesome_14_1);

class XIAO_ESP32S3_Sense : public WifiBoard {
 private:
    Button boot_button_;
    Button user_button_;
    Esp32Camera* camera_ = nullptr;
    i2c_master_bus_handle_t display_i2c_bus_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    Display* display_ = nullptr;

    void InitializeDisplayI2c() {
        display_i2c_bus_ = shared_i2c_master_bus_get();
        if (!display_i2c_bus_) {
            ESP_LOGW(TAG, "Display I2C Init skipped: shared bus unavailable");
        }
    }

    void InitializeSsd1306Display() {
        if (!display_i2c_bus_) return;

        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = 0x3C,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
            .control_phase_bytes = 1,
            .dc_bit_offset = 6,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .flags = {
                .dc_low_on_data = 0,
                .disable_control_phase = 0,
            },
            .scl_speed_hz = 400 * 1000,
        };

        if (esp_lcd_new_panel_io_i2c_v2(display_i2c_bus_, &io_config, &panel_io_) != ESP_OK) return;

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = -1;
        panel_config.bits_per_pixel = 1;

        esp_lcd_panel_ssd1306_config_t ssd1306_config = {
            .height = static_cast<uint8_t>(DISPLAY_HEIGHT),
        };
        panel_config.vendor_config = &ssd1306_config;

        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(panel_io_, &panel_config, &panel_));
        
        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_));
        if (esp_lcd_panel_init(panel_) != ESP_OK) {
            display_ = new NoDisplay();
            return;
        }
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_, false));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_, true));

        display_ = new OledDisplay(panel_io_, panel_, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y,
            {&font_puhui_14_1, &font_awesome_14_1});
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });

        user_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }
            app.ToggleChatState();
        });
    }

    void InitializeTools() {
    }

    void InitializeCamera() {
        camera_ = nullptr;
    }

 public:
    XIAO_ESP32S3_Sense() :
        boot_button_(BOOT_BUTTON_GPIO),
        user_button_(USER_BUTTON_GPIO) {
        InitializeDisplayI2c();
        InitializeSsd1306Display();
        InitializeButtons();
        InitializeTools();
        InitializeCamera();
    }

    virtual Led* GetLed() override {
        static GpioLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
        static ReSpeakerAudioCodec audio_codec;
        return &audio_codec;
    }

    virtual Camera* GetCamera() override {
        return camera_;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
};

DECLARE_BOARD(XIAO_ESP32S3_Sense);