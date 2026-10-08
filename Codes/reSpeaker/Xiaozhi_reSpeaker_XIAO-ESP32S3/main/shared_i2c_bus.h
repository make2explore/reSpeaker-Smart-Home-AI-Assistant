#pragma once

#include <driver/i2c_master.h>

#ifdef __cplusplus
extern "C" {
#endif

i2c_master_bus_handle_t shared_i2c_master_bus_get(void);

#ifdef __cplusplus
}
#endif
