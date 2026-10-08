
file(GLOB_RECURSE SRC_UI ${CMAKE_SOURCE_DIR} "ui/*.c")



idf_component_register(
    SRCS 
    "waveshare_rgb_lcd_port.c" 
    "main.c" 
    "home_control.c"
    "home_ui_control.c"
    "home_clock.c"
    "lvgl_port.c"
    ${SRC_UI}
    INCLUDE_DIRS 
    "."
    "./ui"
    REQUIRES
        esp_http_server
        esp_http_client
        esp_netif
        esp_event
        esp_wifi
        nvs_flash)

idf_component_get_property(lvgl_lib lvgl__lvgl COMPONENT_LIB)
target_compile_options(${lvgl_lib} PRIVATE -Wno-format)
