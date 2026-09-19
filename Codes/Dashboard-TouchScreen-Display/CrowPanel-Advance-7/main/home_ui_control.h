#ifndef HOME_UI_CONTROL_H
#define HOME_UI_CONTROL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HOME_DEVICE_LIGHT = 0,
    HOME_DEVICE_FAN,
    HOME_DEVICE_TV,
    HOME_DEVICE_AC,
    HOME_DEVICE_COUNT
} home_device_t;

/**
 * Initialize the UI integration layer.
 *
 * Starts synchronization between the CrowPanel UI and
 * the ESP32-C6 smart-home controller.
 */
void home_ui_control_init(void);

/**
 * Request a device state change.
 *
 * @param device Device to control.
 * @param on true = ON, false = OFF.
 *
 * @return true if the request was successfully sent.
 */
bool home_ui_set_device_state(home_device_t device, bool on);

/**
 * Bind the smart-home controls to the SquareLine UI.
 *
 * Must be called after ui_init().
 */
void home_ui_bind_controls(void);

/**
 * Get the latest cached state of a device.
 *
 * @param device Device to query.
 * @param on Pointer receiving the cached state.
 *
 * @return true if a valid cached state is available.
 */
bool home_ui_get_device_state(home_device_t device, bool *on);

/**
 * Synchronize the current cached device states with the LVGL UI.
 *
 * This function must be called from the LVGL/UI context.
 */
void home_ui_update_display(void);

/**
 * Mark whether the UI is being updated programmatically
 * from the home-control synchronization layer.
 */
void home_ui_set_syncing(bool syncing);

/**
 * Check whether the UI is currently being synchronized.
 */
bool home_ui_is_syncing(void);

bool home_ui_is_controller_online(void);

#ifdef __cplusplus
}
#endif

#endif // HOME_UI_CONTROL_H