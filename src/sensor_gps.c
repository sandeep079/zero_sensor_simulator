#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <math.h>

typedef struct {
    bool initialized;
    s24_gps_data_t data;
} s24_gps_state_t;

static s24_gps_state_t gps = {0};

s24_status_t s24_gps_init(void) {
    LOG_INFO("Initializing GPS (u-blox NEO-M9N)");
    
    HAL_TIMER.delay_ms(100);  // GPS takes longer to acquire
    
    // Simulate fix acquisition
    gps.initialized = true;
    memset(&gps.data, 0, sizeof(s24_gps_data_t));
    
    LOG_INFO("GPS initialized with baseline fix");
    return S24_OK;
}

s24_status_t s24_gps_read(s24_gps_data_t *out) {
    if (!gps.initialized) {
        LOG_ERROR("GPS not initialized");
        return S24_ERROR;
    }
    
    // Baseline position (drone home position)
    gps.data.latitude = 37.7749;   // San Francisco
    gps.data.longitude = -122.4194;
    gps.data.altitude = 50.0;
    gps.data.speed = 0.0;
    gps.data.heading = 0.0;
    gps.data.fix_quality = 3;  // 3D fix
    gps.data.timestamp_ms = HAL_TIMER.get_tick();
    gps.data.valid = true;
    
    memcpy(out, &gps.data, sizeof(s24_gps_data_t));
    
    LOG_DEBUG("GPS read: lat=%.6f, lon=%.6f, alt=%.1fm, fix=%d",
              gps.data.latitude, gps.data.longitude, 
              gps.data.altitude, gps.data.fix_quality);
    
    return S24_OK;
}

s24_status_t s24_gps_deinit(void) {
    gps.initialized = false;
    LOG_INFO("GPS deinitialized");
    return S24_OK;
}

bool s24_gps_is_ready(void) {
    return gps.initialized;
}