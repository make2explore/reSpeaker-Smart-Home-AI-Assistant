#ifndef HOME_CONTROL_H
#define HOME_CONTROL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the Wi-Fi/HTTP-based home-control module.
 *
 * This function prepares the module for communication with
 * the ESP32-C6 smart-home controller.
 */
void home_control_init(void);

/**
 * Request the light to turn ON or OFF.
 *
 * @param on true  = turn light ON
 *           false = turn light OFF
 *
 * @return true if the HTTP request was successfully sent and
 *         the ESP32-C6 returned a successful HTTP response.
 */
bool home_light_set(bool on);

/**
 * Get the current light state from the ESP32-C6.
 *
 * @param on Pointer where the current state will be stored:
 *           true  = ON
 *           false = OFF
 *
 * @return true if the status was successfully retrieved.
 */
bool home_light_get_status(bool *on);

/**
 * Get the most recently known light state.
 *
 * This function does not perform a network request.
 * It only returns the state cached by the background
 * home-control polling task.
 *
 * @param on Pointer where the cached state will be stored.
 *
 * @return true if a valid state has been received from
 *         the ESP32-C6 at least once.
 */
bool home_light_get_cached_status(bool *on);

/**
 * Get the current home temperature and humidity.
 *
 * @param temperature Pointer where temperature in Celsius is stored.
 * @param humidity Pointer where relative humidity in percent is stored.
 *
 * @return true if the environment data was successfully retrieved.
 */
bool home_environment_get(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

#endif // HOME_CONTROL_H