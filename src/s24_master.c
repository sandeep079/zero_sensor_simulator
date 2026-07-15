#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdbool.h>
#include <string.h>



// Forward declarations for sensor functions
extern s24_status_t s24_imu_init(void);
extern s24_status_t s24_imu_read(s24_imu_data_t *out);
extern s24_status_t s24_imu_deinit(void);
extern bool s24_imu_is_ready(void);

extern s24_status_t s24_gps_init(void);
extern s24_status_t s24_gps_read(s24_gps_data_t *out);
extern s24_status_t s24_gps_deinit(void);
extern bool s24_gps_is_ready(void);

extern s24_status_t s24_lidar_init(void);
extern s24_status_t s24_lidar_read(s24_lidar_data_t *out);
extern s24_status_t s24_lidar_deinit(void);
extern bool s24_lidar_is_ready(void);

extern s24_status_t s24_camera_init(void);
extern s24_status_t s24_camera_read(s24_camera_data_t *out);
extern s24_status_t s24_camera_deinit(void);
extern bool s24_camera_is_ready(void);

extern s24_status_t s24_battery_init(void);
extern s24_status_t s24_battery_read(s24_battery_data_t *out);
extern s24_status_t s24_battery_deinit(void);
extern bool s24_battery_is_ready(void);

// Rest of the existing s24_master.c code...


typedef enum {
    SENSOR_IMU = 0,
    SENSOR_GPS = 1,
    SENSOR_LIDAR = 2,
    SENSOR_CAMERA = 3,
    SENSOR_BATTERY = 4,
    SENSOR_COUNT = 5
} s24_sensor_id_t;

typedef struct {
    bool initialized;
    bool booted;
    uint32_t boot_time_ms;
    char *name;
    s24_status_t (*init)(void);
    s24_status_t (*deinit)(void);
    bool (*is_ready)(void);
} s24_sensor_entry_t;

static s24_sensor_entry_t sensors[SENSOR_COUNT] = {
    { .name = "IMU", .init = s24_imu_init, .deinit = s24_imu_deinit, .is_ready = s24_imu_is_ready },
    { .name = "GPS", .init = s24_gps_init, .deinit = s24_gps_deinit, .is_ready = s24_gps_is_ready },
    { .name = "LiDAR", .init = s24_lidar_init, .deinit = s24_lidar_deinit, .is_ready = s24_lidar_is_ready },
    { .name = "Camera", .init = s24_camera_init, .deinit = s24_camera_deinit, .is_ready = s24_camera_is_ready },
    { .name = "Battery", .init = s24_battery_init, .deinit = s24_battery_deinit, .is_ready = s24_battery_is_ready }
};

static bool master_initialized = false;
static uint32_t master_start_time = 0;

/* ========== Master Toggle Functions ========== */

s24_status_t s24_master_boot(void) {
    if (master_initialized) {
        LOG_WARN("Master already booted");
        return S24_OK;
    }
    
    LOG_INFO("=== S24 Zero Sensor Simulator Booting ===");
    LOG_INFO("Boot timestamp: %s", s24_get_timestamp());
    
    master_start_time = HAL_TIMER.get_tick();
    s24_status_t overall_status = S24_OK;
    
    // Initialize each sensor
    for (int i = 0; i < SENSOR_COUNT; i++) {
        LOG_INFO("Initializing %s...", sensors[i].name);
        
        s24_status_t status = sensors[i].init();
        if (status == S24_OK) {
            sensors[i].booted = true;
            sensors[i].boot_time_ms = HAL_TIMER.get_tick() - master_start_time;
            LOG_INFO("   %s booted in %dms", 
                     sensors[i].name, sensors[i].boot_time_ms);
        } else {
            sensors[i].booted = false;
            LOG_ERROR("   %s boot failed with code %d", 
                      sensors[i].name, status);
            overall_status = S24_ERROR;
        }
    }
    
    if (overall_status == S24_OK) {
        LOG_INFO("All sensors booted successfully in %dms",
                 HAL_TIMER.get_tick() - master_start_time);
    } else {
        LOG_WARN(" Some sensors failed to boot (see above)");
    }
    
    master_initialized = true;
    return overall_status;
}

void s24_master_shutdown(void) {
    if (!master_initialized) {
        LOG_WARN("Master not booted, nothing to shutdown");
        return;
    }
    
    LOG_INFO("=== S24 Zero Sensor Simulator Shutting Down ===");
    LOG_INFO("Shutdown timestamp: %s", s24_get_timestamp());
    
    uint32_t uptime = HAL_TIMER.get_tick() - master_start_time;
    LOG_INFO("Uptime: %dms", uptime);
    
    // Deinitialize each sensor (reverse order)
    for (int i = SENSOR_COUNT - 1; i >= 0; i--) {
        if (sensors[i].booted) {
            LOG_INFO("Deinitializing %s...", sensors[i].name);
            sensors[i].deinit();
            sensors[i].booted = false;
            LOG_INFO("   %s deinitialized", sensors[i].name);
        }
    }
    
    master_initialized = false;
    LOG_INFO("Shutdown complete");
}

bool s24_master_is_booted(void) {
    return master_initialized;
}

bool s24_sensor_is_booted(int sensor_id) {
    if (sensor_id < 0 || sensor_id >= SENSOR_COUNT) return false;
    return sensors[sensor_id].booted;
}

const char *s24_sensor_get_name(int sensor_id) {
    if (sensor_id < 0 || sensor_id >= SENSOR_COUNT) return "Unknown";
    return sensors[sensor_id].name;
}

/* ========== Convenience Read Functions ========== */

s24_status_t s24_read_all(void) {
    if (!master_initialized) {
        LOG_ERROR("Master not booted");
        return S24_ERROR;
    }
    
    LOG_INFO("Reading all sensors...");
    
    s24_imu_data_t imu_data;
    s24_gps_data_t gps_data;
    s24_lidar_data_t lidar_data;
    s24_camera_data_t camera_data;
    s24_battery_data_t battery_data;
    
    s24_status_t status = S24_OK;
    
    if (sensors[SENSOR_IMU].booted) {
        if (s24_imu_read(&imu_data) != S24_OK) status = S24_ERROR;
    }
    
    if (sensors[SENSOR_GPS].booted) {
        if (s24_gps_read(&gps_data) != S24_OK) status = S24_ERROR;
    }
    
    if (sensors[SENSOR_LIDAR].booted) {
        if (s24_lidar_read(&lidar_data) != S24_OK) status = S24_ERROR;
    }
    
    if (sensors[SENSOR_CAMERA].booted) {
        if (s24_camera_read(&camera_data) != S24_OK) status = S24_ERROR;
    }
    
    if (sensors[SENSOR_BATTERY].booted) {
        if (s24_battery_read(&battery_data) != S24_OK) status = S24_ERROR;
    }
    
    return status;
}