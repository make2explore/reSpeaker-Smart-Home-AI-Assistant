#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// =============================================================
// 1. 基础硬件定义
// =============================================================
#define CONFIG_IDF_TARGET_ESP32S3 1

// XIAO ESP32S3 板载 LED (黄色)
#define BUILTIN_LED_GPIO        GPIO_NUM_21 

// 按钮定义
// Boot 按钮 (通常用于进入下载模式，也可复用)
#define BOOT_BUTTON_GPIO        GPIO_NUM_0  
// 用户按钮 (XIAO 扩展板上的 D1/D2 等位置可能有按钮，如果没有外接可忽略)
#define USER_BUTTON_GPIO        GPIO_NUM_NC 

// 触摸/音量按钮 (XIAO 上通常没有这些物理按键，设为 NC - Not Connected)
#define TOUCH_BUTTON_GPIO       GPIO_NUM_NC
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC
#define RESET_NVS_BUTTON_GPIO     GPIO_NUM_NC
#define RESET_FACTORY_BUTTON_GPIO GPIO_NUM_NC

// =============================================================
// 2. 音频系统配置 (核心修改部分)
// =============================================================

// --- 为什么改用 I2S? ---
// 原版代码使用 PDM (脉冲密度调制) 麦克风。
// XVF3800 是一个 DSP 音频处理器，它处理完声音后，通过 I2S (集成音频接口) 协议
// 将干净的 PCM 数据传给 ESP32。因此必须启用 I2S 输入和输出。
#define AUDIO_INPUT_I2S
#define AUDIO_OUTPUT_I2S

// --- 采样率设置 ---
// XVF3800 固件通常工作在 16kHz (语音识别标准)。
// 保持输入输出采样率一致非常重要，这有助于 XVF3800 进行回声消除 (AEC)。
#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 16000

// 小智项目内部使用的是这组宏名
#define AUDIO_PCM_SAMPLING_RATE  16000 
#define AUDIO_RESAMPLE_RATE      16000



// --- I2S 引脚定义 (对应 ReSpeaker 硬件连接) ---
// BCLK (位时钟): 对应 XIAO 的 D9
#define AUDIO_I2S_BCK_IO      GPIO_NUM_8  
// LRCK/WS (左右声道时钟): 对应 XIAO 的 D8
#define AUDIO_I2S_WS_IO       GPIO_NUM_7  
// DOUT (ESP32 发送给喇叭的数据): 对应 XIAO 的 D7
// 注意：GPIO 44 原本是 UART0 RX，现在被强制用于音频发送
#define AUDIO_I2S_DO_IO       GPIO_NUM_44 
// DIN (ESP32 接收麦克风的数据): 对应 XIAO 的 D6
// 注意：GPIO 43 原本是 UART0 TX，现在被强制用于音频接收
#define AUDIO_I2S_DI_IO       GPIO_NUM_43 

// I2S 传输位宽 (XVF3800 通常输出 32位数据)
#define AUDIO_I2S_BITS_PER_SAMPLE 32

// =============================================================
// 3. I2C 总线配置 (用于控制 XVF3800 音量)
// =============================================================
// I2C 引脚 (XIAO 的 D4 和 D5)
#define I2C_SDA_PIN           GPIO_NUM_5
#define I2C_SCL_PIN           GPIO_NUM_6

// XVF3800 的 I2C 地址 (你提供的)
#define XVF3800_I2C_ADDR      0x2C

// 是否有屏幕？
// 即使没有物理屏幕，保留定义也没坏处，只要它们使用相同的 I2C 引脚
#define DISPLAY_SDA_PIN       GPIO_NUM_5
#define DISPLAY_SCL_PIN       GPIO_NUM_6
#define DISPLAY_WIDTH         128
#define DISPLAY_HEIGHT        64
#define DISPLAY_MIRROR_X      true
#define DISPLAY_MIRROR_Y      true

// =============================================================
// 4. 功能开关与存储
// =============================================================

// 禁用 SD 卡 (你提到没有 SD 卡装置)
// 注意：这意味着 Wi-Fi 配置文件不能从 SD 卡读取。
// 你需要在代码中启用 NVS (内部存储) 或者硬编码 Wi-Fi 密码。
#undef USE_SD_CARD 
#define MOUNT_SD_CARD 0

// 禁用摄像头 (XVF3800 占用了必要的引脚，且你需要专注于音频)
// 将原有的 Camera Pin 定义全部注释或设为 -1，防止编译冲突
#define CAMERA_PIN_PWDN     -1
#define CAMERA_PIN_RESET    -1
#define CAMERA_PIN_XCLK     -1
#define CAMERA_PIN_SIOD     -1
#define CAMERA_PIN_SIOC     -1
#define CAMERA_PIN_D7       -1
#define CAMERA_PIN_D6       -1
#define CAMERA_PIN_D5       -1
#define CAMERA_PIN_D4       -1
#define CAMERA_PIN_D3       -1
#define CAMERA_PIN_D2       -1
#define CAMERA_PIN_D1       -1
#define CAMERA_PIN_D0       -1
#define CAMERA_PIN_VSYNC    -1
#define CAMERA_PIN_HREF     -1
#define CAMERA_PIN_PCLK     -1

#endif  // _BOARD_CONFIG_H_