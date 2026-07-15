#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    bool initialized;
    s24_battery_data_t data;
    float discharge_rate;  // % per second
} s24_battery_state_t;

static s24_battery_state_t battery = {0};

s24_status_t s24_battery_init(void) {
    LOG_INFO("Initializing Battery Monitor (MAX17048)");
    
    HAL_TIMER.delay_ms(10);
    
    battery.initialized = true;
    memset(&battery.data, 0, sizeof(s24_battery_data_t));
    battery.data.charge_percent = 100.0f;
    battery.data.voltage = 12.6f;  // 3S LiPo fully charged
    battery.data.current = 0.0f;
    battery.data.temperature = 25.0f;
    battery.data.charging = false;
    battery.discharge_rate = 0.0f;  // Static for baseline
    
    LOG_INFO("Battery monitor initialized: %.1f%%", 
             battery.data.charge_percent);
    return S24_OK;
}

s24_status_t s24_battery_read(s24_battery_data_t *out) {
    if (!battery.initialized) {
        LOG_ERROR("Battery not initialized");
        return S24_ERROR;
    }
    
    // Baseline battery data (static for zero simulator)
    battery.data.voltage = 12.4f + (rand() % 20) / 100.0f;  // 12.4-12.6V
    battery.data.current = 0.5f + (rand() % 10) / 100.0f;   // 0.5-0.6A
    battery.data.charge_percent = 98.0f + (rand() % 20) / 10.0f; // 98-100%
    battery.data.temperature = 28.0f + (rand() % 10) / 10.0f;
    battery.data.charging = false;
    battery.data.timestamp_ms = HAL_TIMER.get_tick();
    battery.data.valid = true;
    
    memcpy(out, &battery.data, sizeof(s24_battery_data_t));
    
    LOG_DEBUG("Battery read: V=%.2fV, I=%.2fA, charge=%.1f%%, temp=%.1f°C",
              battery.data.voltage, battery.data.current,
              battery.data.charge_percent, battery.data.temperature);
    
    return S24_OK;
}

s24_status_t s24_battery_deinit(void) {
    battery.initialized = false;
    LOG_INFO("Battery monitor deinitialized");
    return S24_OK;
}

bool s24_battery_is_ready(void) {
    return battery.initialized;
}