#include "shared_i2c_bus.h" // 包含头文件，声明了 shared_i2c_master_bus_get 函数
#include <esp_err.h>       // ESP-IDF 错误处理库 (esp_err_t, ESP_OK 等)
#include <esp_log.h>       // ESP-IDF 日志打印库 (ESP_LOGE 等)
#include "config.h"        // 包含引脚定义，如 I2C_SDA_PIN 和 I2C_SCL_PIN

static const char *TAG = "shared_i2c"; // 定义日志标签，方便在串口输出中识别来源

/**
 * static: 静态全局变量。
 * 作用：限制 s_bus 的作用域仅在此文件内，外部文件无法直接修改它。
 * 初始化为 NULL：表示总线尚未被创建。它充当“记忆”的作用，记录总线是否已初始化。
 */
static i2c_master_bus_handle_t s_bus = NULL; 

i2c_master_bus_handle_t shared_i2c_master_bus_get(void) {
    // 【判断】如果 s_bus 不为 NULL，说明之前已经初始化过了
    if (s_bus) {
        return s_bus; // 直接返回已经存在的句柄，不再执行下面的初始化代码
    }

    // 【配置】如果是第一次运行，则定义 I2C 总线的配置结构体
    i2c_master_bus_config_t bus_config = {
        .i2c_port = (i2c_port_t)0,        // 使用 ESP32 的 I2C 端口 0
        .sda_io_num = (gpio_num_t)I2C_SDA_PIN, // 数据线引脚 (通常是 17)
        .scl_io_num = (gpio_num_t)I2C_SCL_PIN, // 时钟线引脚 (通常是 18)
        .clk_source = I2C_CLK_SRC_DEFAULT,     // 使用默认时钟源
        .glitch_ignore_cnt = 7,                // 信号毛刺过滤，增强抗干扰能力
        .intr_priority = 0,                    // 中断优先级，0 为自动分配
        .trans_queue_depth = 0,                // 传输队列深度
        .flags = {
            .enable_internal_pullup = 1,       // 启用内部上拉电阻
        },
    };

    // 【执行】调用系统底层 API 创建新的 I2C 主机总线
    // 将生成的句柄地址存入静态变量 s_bus 中
    esp_err_t err = i2c_new_master_bus(&bus_config, &s_bus);

    // 【校验】如果创建失败（例如引脚被占用）
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(err));
        s_bus = NULL; // 确保失败后 s_bus 依然是 NULL
    }
    
    // 返回句柄（成功则返回有效句柄，失败则返回 NULL）
    return s_bus;
}