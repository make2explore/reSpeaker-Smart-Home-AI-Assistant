#ifndef HOME_CONTROL_H
#define HOME_CONTROL_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Wi-Fi network information returned by a scan.
 */
typedef struct {
    char ssid[33];
    bool secure;
} home_wifi_network_t;

/**
 * Scan for nearby Wi-Fi networks.
 *
 * @param networks    Array where scan results will be stored.
 * @param count       In: maximum number of results.
 *                    Out: number of results returned.
 *
 * @return true if the scan completed successfully.
 */
bool home_wifi_scan(home_wifi_network_t *networks,
                    uint16_t *count);

/**
 * Test Wi-Fi credentials and save them to NVS on success.
 *
 * @return true if the connection succeeds and credentials are saved.
 */
bool home_wifi_connect_and_save(const char *ssid,
                                const char *password);

/**
 * Check whether the Dashboard is currently in Wi-Fi provisioning mode.
 *
 * @return true when Wi-Fi setup mode is active.
 */
bool home_wifi_is_provisioning(void);


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
