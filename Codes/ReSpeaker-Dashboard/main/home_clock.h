#ifndef HOME_CLOCK_H
#define HOME_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the dashboard clock.
 *
 * Configures the system time to Asia/Kolkata and
 * starts SNTP synchronization.
 */
void home_clock_init(void);

/**
 * Update the time, day and date labels on the dashboard.
 *
 * Must be called from the LVGL/UI context.
 */
void home_clock_update_display(void);

#ifdef __cplusplus
}
#endif

#endif // HOME_CLOCK_H