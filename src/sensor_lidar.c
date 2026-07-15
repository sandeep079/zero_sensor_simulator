#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <math.h>

typedef struct {
    bool initialized;
    s24_lidar_data_t data;
    uint32_t cycle_count;
} s24_lidar_state_t;

static s24_lidar_state_t lidar = {0};

s24_status_t s24_lidar_init(void) {
    LOG_INFO("Initializing LiDAR (Velodyne VLP-16)");
    
    HAL_TIMER.delay_ms(50);
    
    lidar.initialized = true;
    lidar.cycle_count = 0;
    memset(&lidar.data, 0, sizeof(s24_lidar_data_t));
    lidar.data.point_count = 16;  // 16 channels
    
    LOG_INFO("LiDAR initialized");
    return S24_OK;
}

s24_status_t s24_lidar_read(s24_lidar_data_t *out) {
    if (!lidar.initialized) {
        LOG_ERROR("LiDAR not initialized");
        return S24_ERROR;
    }
    
    lidar.cycle_count++;
    
    // Simulate LiDAR scan: distance varies slightly each cycle
    float base_distance = 5.0f;
    float variation = sinf(lidar.cycle_count / 100.0f) * 0.5f;
    
    lidar.data.distance = base_distance + variation;
    lidar.data.intensity = 80.0f + (rand() % 40);
    lidar.data.point_count = 16 + (rand() % 10);  // 16-26 points
    lidar.data.timestamp_ms = HAL_TIMER.get_tick();
    lidar.data.valid = true;
    
    memcpy(out, &lidar.data, sizeof(s24_lidar_data_t));
    
    LOG_DEBUG("LiDAR read: dist=%.2fm, intensity=%.1f, points=%d",
              lidar.data.distance, lidar.data.intensity, 
              lidar.data.point_count);
    
    return S24_OK;
}

s24_status_t s24_lidar_deinit(void) {
    lidar.initialized = false;
    LOG_INFO("LiDAR deinitialized");
    return S24_OK;
}

bool s24_lidar_is_ready(void) {
    return lidar.initialized;
}