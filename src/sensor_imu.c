#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    bool initialized;
    s24_imu_data_t data;
} s24_imu_state_t;

static s24_imu_state_t imu = {0};

/* ========== IMU Driver Functions ========== */

s24_status_t s24_imu_init(void) {
    LOG_INFO("Initializing IMU (ICM-42688-P)");
    
    // Simulate hardware init
    HAL_TIMER.delay_ms(10);
    
    // Read WHO_AM_I (simulated)
    uint8_t who_am_i = 0x47;  // Expected value per S24 doc
    
    if (who_am_i != 0x47) {
        LOG_ERROR("IMU WHO_AM_I mismatch: got 0x%02X", who_am_i);
        return S24_ERROR;
    }
    
    imu.initialized = true;
    memset(&imu.data, 0, sizeof(s24_imu_data_t));
    imu.data.temperature = 25.0f;  // Baseline temp
    
    LOG_INFO("IMU initialized successfully");
    return S24_OK;
}

s24_status_t s24_imu_read(s24_imu_data_t *out) {
    if (!imu.initialized) {
        LOG_ERROR("IMU not initialized");
        return S24_ERROR;
    }
    
    // Generate baseline data (zero/static)
    imu.data.x = 0.0f;
    imu.data.y = 0.0f;
    imu.data.z = 9.81f;  // Gravity on Z
    imu.data.temperature = 25.0f + (rand() % 10) / 10.0f;  // ±0.5°C variation
    imu.data.timestamp_ms = HAL_TIMER.get_tick();
    imu.data.valid = true;
    
    memcpy(out, &imu.data, sizeof(s24_imu_data_t));
    
    LOG_DEBUG("IMU read: ax=%.2f, ay=%.2f, az=%.2f, temp=%.1f°C",
              imu.data.x, imu.data.y, imu.data.z, imu.data.temperature);
    
    return S24_OK;
}

s24_status_t s24_imu_deinit(void) {
    if (!imu.initialized) return S24_OK;
    
    imu.initialized = false;
    LOG_INFO("IMU deinitialized");
    return S24_OK;
}

bool s24_imu_is_ready(void) {
    return imu.initialized;
}