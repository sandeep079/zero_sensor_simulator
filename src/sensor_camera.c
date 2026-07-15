#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    bool initialized;
    s24_camera_data_t data;
    uint32_t frame_count;
} s24_camera_state_t;

static s24_camera_state_t camera = {0};

s24_status_t s24_camera_init(void) {
    LOG_INFO("Initializing Camera (OV2640)");
    
    HAL_TIMER.delay_ms(30);
    
    camera.initialized = true;
    camera.frame_count = 0;
    memset(&camera.data, 0, sizeof(s24_camera_data_t));
    camera.data.width = 800;
    camera.data.height = 600;
    camera.data.data_size = 800 * 600 * 2;  // RGB565
    
    LOG_INFO("Camera initialized (%dx%d)", 
             camera.data.width, camera.data.height);
    return S24_OK;
}

s24_status_t s24_camera_read(s24_camera_data_t *out) {
    if (!camera.initialized) {
        LOG_ERROR("Camera not initialized");
        return S24_ERROR;
    }
    
    camera.frame_count++;
    
    // Simulate camera frame
    camera.data.width = 800;
    camera.data.height = 600;
    camera.data.data_size = 800 * 600 * 2;
    camera.data.timestamp_ms = HAL_TIMER.get_tick();
    camera.data.valid = true;
    
    // We don't allocate real pixel data to avoid memory leaks
    // In production, this would be DMA buffer
    
    memcpy(out, &camera.data, sizeof(s24_camera_data_t));
    
    LOG_DEBUG("Camera read: frame=%d, %dx%d, size=%d bytes",
              camera.frame_count, camera.data.width, 
              camera.data.height, camera.data.data_size);
    
    return S24_OK;
}

s24_status_t s24_camera_deinit(void) {
    camera.initialized = false;
    LOG_INFO("Camera deinitialized");
    return S24_OK;
}

bool s24_camera_is_ready(void) {
    return camera.initialized;
}