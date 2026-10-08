#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "display/oled_display.h"
#include "mcp_server.h"
#include "esp_http_client.h"
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
// Custom AudioCodec class: ReSpeaker I2S
// ==========================================
class ReSpeakerAudioCodec : public NoAudioCodecDuplex {
public:
    ReSpeakerAudioCodec()
        : NoAudioCodecDuplex(
              AUDIO_INPUT_SAMPLE_RATE,
              AUDIO_OUTPUT_SAMPLE_RATE,
              AUDIO_I2S_BCK_IO,
              AUDIO_I2S_WS_IO,
              AUDIO_I2S_DO_IO,
              AUDIO_I2S_DI_IO) {
        ESP_LOGI(
            TAG,
            "ReSpeaker I2S Driver Initialized (%dHz, 32bit)",
            AUDIO_INPUT_SAMPLE_RATE);
    }

    void SetOutputVolume(int volume) override {
        // Hardware volume is already configured in audio_bsp.c.
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
            ESP_LOGW(
                TAG,
                "Display I2C Init skipped: shared bus unavailable");
        }
    }

    void InitializeSsd1306Display() {
        if (!display_i2c_bus_) {
            return;
        }

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

        if (esp_lcd_new_panel_io_i2c_v2(
                display_i2c_bus_,
                &io_config,
                &panel_io_) != ESP_OK) {
            return;
        }

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = -1;
        panel_config.bits_per_pixel = 1;

        esp_lcd_panel_ssd1306_config_t ssd1306_config = {
            .height = static_cast<uint8_t>(DISPLAY_HEIGHT),
        };

        panel_config.vendor_config = &ssd1306_config;

        ESP_ERROR_CHECK(
            esp_lcd_new_panel_ssd1306(
                panel_io_,
                &panel_config,
                &panel_));

        ESP_ERROR_CHECK(
            esp_lcd_panel_reset(panel_));

        if (esp_lcd_panel_init(panel_) != ESP_OK) {
            display_ = new NoDisplay();
            return;
        }

        ESP_ERROR_CHECK(
            esp_lcd_panel_invert_color(panel_, false));

        ESP_ERROR_CHECK(
            esp_lcd_panel_disp_on_off(panel_, true));

        display_ = new OledDisplay(
            panel_io_,
            panel_,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            DISPLAY_MIRROR_X,
            DISPLAY_MIRROR_Y,
            {&font_puhui_14_1, &font_awesome_14_1});
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();

            if (app.GetDeviceState() == kDeviceStateStarting &&
                !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }

            app.ToggleChatState();
        });

        user_button_.OnClick([this]() {
            auto& app = Application::GetInstance();

            if (app.GetDeviceState() == kDeviceStateStarting &&
                !WifiStation::GetInstance().IsConnected()) {
                ResetWifiConfiguration();
            }

            app.ToggleChatState();
        });
    }

    void InitializeTools() {
        auto& mcp_server = McpServer::GetInstance();

        // =========================================================
        // Control a smart-home device
        // Supported devices: light, ac, fan, tv
        // Supported states: on, off
        // =========================================================
        mcp_server.AddTool(
            "self.home.device_control",
            "Control a home device. Supported devices are light, ac, fan, and tv. "
            "Set state to on or off.",
            PropertyList({
                Property("device", kPropertyTypeString),
                Property("state", kPropertyTypeString)
            }),
            [](const PropertyList& properties) -> ReturnValue {
                const std::string device =
                    properties["device"].value<std::string>();

                const std::string state =
                    properties["state"].value<std::string>();

                if (device != "light" &&
                    device != "ac" &&
                    device != "fan" &&
                    device != "tv") {
                    return "Invalid device. Use light, ac, fan, or tv.";
                }

                if (state != "on" && state != "off") {
                    return "Invalid state. Use on or off.";
                }

                std::string url =
                    "http://home-controller.local/api/device/" +
                    device + "/" + state;

                esp_http_client_config_t config = {};
                config.url = url.c_str();
                config.method = HTTP_METHOD_POST;
                config.timeout_ms = 2000;

                esp_http_client_handle_t client =
                    esp_http_client_init(&config);

                if (client == nullptr) {
                    return "Failed to initialize HTTP client.";
                }

                esp_err_t err =
                    esp_http_client_perform(client);

                if (err != ESP_OK) {
                    esp_http_client_cleanup(client);

                    return std::string(
                               "HTTP request failed: ") +
                           esp_err_to_name(err);
                }

                int status_code =
                    esp_http_client_get_status_code(client);

                esp_http_client_cleanup(client);

                if (status_code >= 200 &&
                    status_code < 300) {

                    std::string device_name;

                    if (device == "light") {
                        device_name = "light";
                    } else if (device == "ac") {
                        device_name = "air conditioner";
                    } else if (device == "fan") {
                        device_name = "fan";
                    } else {
                        device_name = "TV";
                    }

                    std::string state_name =
                        (state == "on") ? "on" : "off";

                    return "The " + device_name +
                           " has been turned " +
                           state_name + ".";
                }

                return std::string(
                           "ESP32-C6 returned HTTP status ") +
                       std::to_string(status_code);
            });

        // =========================================================
        // Get the actual state of a smart-home device
        // Supported devices: light, ac, fan, tv
        // =========================================================
        mcp_server.AddTool(
            "self.home.device_status",
            "Get the current actual state of a home device. "
            "Supported devices are light, ac, fan, and tv.",
            PropertyList({
                Property("device", kPropertyTypeString)
            }),
            [](const PropertyList& properties) -> ReturnValue {
                const std::string device =
                    properties["device"].value<std::string>();

                if (device != "light" &&
                    device != "ac" &&
                    device != "fan" &&
                    device != "tv") {
                    return "Invalid device. Use light, ac, fan, or tv.";
                }

                std::string url =
                    "http://home-controller.local/api/device/" +
                    device + "/status";

                esp_http_client_config_t config = {};
                config.url = url.c_str();
                config.timeout_ms = 2000;

                esp_http_client_handle_t client =
                    esp_http_client_init(&config);

                if (client == nullptr) {
                    return "Failed to initialize HTTP client.";
                }

                esp_err_t err =
                    esp_http_client_open(client, 0);

                if (err != ESP_OK) {
                    esp_http_client_cleanup(client);

                    return std::string(
                               "HTTP open failed: ") +
                           esp_err_to_name(err);
                }

                int content_length =
                    esp_http_client_fetch_headers(client);

                if (content_length < 0) {
                    esp_http_client_close(client);
                    esp_http_client_cleanup(client);

                    return "Failed to fetch HTTP response headers.";
                }

                int status_code =
                    esp_http_client_get_status_code(client);

                if (status_code != 200) {
                    esp_http_client_close(client);
                    esp_http_client_cleanup(client);

                    return std::string(
                               "ESP32-C6 returned HTTP status ") +
                           std::to_string(status_code);
                }

                char response[160] = {0};

                int len =
                    esp_http_client_read_response(
                        client,
                        response,
                        sizeof(response) - 1);

                if (len < 0) {
                    esp_http_client_close(client);
                    esp_http_client_cleanup(client);

                    return "Failed to read device status response.";
                }

                response[len] = '\0';

                ESP_LOGI(
                    TAG,
                    "Device status response: %s",
                    response);

                esp_http_client_close(client);
                esp_http_client_cleanup(client);

                const bool is_on =
                    (strstr(
                         response,
                         "\"state\":\"on\"") != nullptr);

                const bool is_off =
                    (strstr(
                         response,
                         "\"state\":\"off\"") != nullptr);

                std::string device_name;

                if (device == "light") {
                    device_name = "light";
                } else if (device == "ac") {
                    device_name = "air conditioner";
                } else if (device == "fan") {
                    device_name = "fan";
                } else {
                    device_name = "TV";
                }

                if (is_on) {
                    return "The " + device_name + " is turned on.";
                }

                if (is_off) {
                    return "The " + device_name + " is turned off.";
                }

                return std::string(
                           "Unexpected response from ESP32-C6: ") +
                       response;
            });
            
        // =========================================================
        // Get current home temperature and humidity
        // =========================================================
        mcp_server.AddTool(
            "self.home.environment",
            "Get the current temperature and humidity at home. "
            "This tool takes no parameters.",
            PropertyList(std::vector<Property>{}),
            [](const PropertyList& properties) -> ReturnValue {

                std::string url =
                    "http://home-controller.local/api/environment";

                esp_http_client_config_t config = {};
                config.url = url.c_str();
                config.method = HTTP_METHOD_GET;
                config.timeout_ms = 2000;

                esp_http_client_handle_t client =
                    esp_http_client_init(&config);

                if (client == nullptr) {
                    return "Failed to initialize HTTP client.";
                }

                esp_err_t err =
                    esp_http_client_open(client, 0);

                if (err != ESP_OK) {
                    esp_http_client_cleanup(client);

                    return std::string(
                               "HTTP open failed: ") +
                           esp_err_to_name(err);
                }

                int content_length =
                    esp_http_client_fetch_headers(client);

                if (content_length < 0) {
                    esp_http_client_close(client);
                    esp_http_client_cleanup(client);

                    return "Failed to fetch HTTP response headers.";
                }

                int status_code =
                    esp_http_client_get_status_code(client);

                if (status_code != 200) {
                    esp_http_client_close(client);
                    esp_http_client_cleanup(client);

                    return std::string(
                               "Home controller returned HTTP status ") +
                           std::to_string(status_code);
                }

                char response[256] = {0};

                int len =
                    esp_http_client_read_response(
                        client,
                        response,
                        sizeof(response) - 1);

                if (len < 0) {
                    esp_http_client_close(client);
                    esp_http_client_cleanup(client);

                    return "Failed to read environment response.";
                }

                response[len] = '\0';

                ESP_LOGI(
                    TAG,
                    "Environment response: %s",
                    response);

                esp_http_client_close(client);
                esp_http_client_cleanup(client);

                double temperature = 0.0;
                double humidity = 0.0;

                if (sscanf(
                        response,
                        "{\"success\":true,\"temperature\":%lf,\"humidity\":%lf}",
                        &temperature,
                        &humidity) == 2) {

                    char result[160];

                    snprintf(
                        result,
                        sizeof(result),
                        "The current temperature at home is %.2f degrees Celsius and the humidity is %.2f percent.",
                        temperature,
                        humidity);

                    return std::string(result);
                }

                return std::string(
                           "Unexpected environment response: ") +
                       response;
            });
            
    }

    void InitializeCamera() {
        camera_ = nullptr;
    }

public:
    XIAO_ESP32S3_Sense()
        : boot_button_(BOOT_BUTTON_GPIO),
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
