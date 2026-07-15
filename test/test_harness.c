#include "../include/s24_contract.h"
#include "../include/s24_logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ========== Mock HAL Implementation ========== */

// Mock GPIO
static void mock_gpio_init(uint32_t pin, bool output) {
    (void)pin; (void)output;
    printf("Mock GPIO: init\n");
}
static void mock_gpio_write(uint32_t pin, bool state) {
    (void)pin; (void)state;
    printf("Mock GPIO: write\n");
}
static bool mock_gpio_read(uint32_t pin) { 
    (void)pin;
    return 0; 
}
static void mock_gpio_toggle(uint32_t pin) {
    (void)pin;
    printf("Mock GPIO: toggle\n");
}

s24_gpio_t HAL_GPIO = {
    .init = mock_gpio_init,
    .write = mock_gpio_write,
    .read = mock_gpio_read,
    .toggle = mock_gpio_toggle
};

// Mock SPI
static s24_status_t mock_spi_init(uint32_t spi_id) {
    (void)spi_id;
    printf("Mock SPI: init\n");
    return S24_OK;
}
static s24_status_t mock_spi_transfer(uint32_t spi_id, uint8_t *tx, 
                                       uint8_t *rx, uint32_t len) {
    (void)spi_id; (void)len;
    static uint8_t counter = 0;
    counter++;
    if (tx && rx && tx[0] == 0x47) {
        rx[0] = 0x47;
    }
    return S24_OK;
}
static s24_status_t mock_spi_cs_control(uint32_t spi_id, bool enable) {
    (void)spi_id; (void)enable;
    return S24_OK;
}

s24_spi_t HAL_SPI = {
    .init = mock_spi_init,
    .transfer = mock_spi_transfer,
    .cs_control = mock_spi_cs_control
};

// Mock I2C
static s24_status_t mock_i2c_init(uint32_t i2c_id) {
    (void)i2c_id;
    printf("Mock I2C: init\n");
    return S24_OK;
}
static s24_status_t mock_i2c_write(uint32_t i2c_id, uint8_t dev_addr, 
                                    uint8_t *data, uint32_t len) {
    (void)i2c_id; (void)dev_addr; (void)data; (void)len;
    return S24_OK;
}
static s24_status_t mock_i2c_read(uint32_t i2c_id, uint8_t dev_addr, 
                                   uint8_t *data, uint32_t len) {
    (void)i2c_id; (void)dev_addr; (void)data; (void)len;
    return S24_OK;
}
static s24_status_t mock_i2c_write_read(uint32_t i2c_id, uint8_t dev_addr,
                                         uint8_t *tx, uint32_t tx_len,
                                         uint8_t *rx, uint32_t rx_len) {
    (void)i2c_id; (void)dev_addr; (void)tx; (void)tx_len; (void)rx; (void)rx_len;
    return S24_OK;
}

s24_i2c_t HAL_I2C = {
    .init = mock_i2c_init,
    .write = mock_i2c_write,
    .read = mock_i2c_read,
    .write_read = mock_i2c_write_read
};

// Mock Timer
static uint32_t mock_tick = 0;
static void mock_delay_ms(uint32_t ms) {
    mock_tick += ms;
}
static void mock_delay_us(uint32_t us) {
    mock_tick += us / 1000;
}
static uint32_t mock_get_tick(void) { return mock_tick; }
static void mock_reset_tick(void) { mock_tick = 0; }

s24_timer_t HAL_TIMER = {
    .delay_ms = mock_delay_ms,
    .delay_us = mock_delay_us,
    .get_tick = mock_get_tick,
    .reset_tick = mock_reset_tick
};

/* ========== External declarations ========== */
// These functions are defined in s24_master.c and sensor files
extern s24_status_t s24_master_boot(void);
extern void s24_master_shutdown(void);
extern bool s24_master_is_booted(void);

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

/* ========== Test Functions ========== */

int test_boot_sequence(void) {
    printf("\n========== TEST: Boot Sequence ==========\n");
    
    // Initialize logger
    s24_logger_init(NULL);
    
    s24_status_t status = s24_master_boot();
    
    if (status == S24_OK && s24_master_is_booted()) {
        printf("✅ Boot sequence PASSED\n");
        return 0;
    } else {
        printf("❌ Boot sequence FAILED\n");
        return 1;
    }
}

int test_sensor_reads(void) {
    printf("\n========== TEST: Sensor Reads ==========\n");
    
    if (!s24_master_is_booted()) {
        printf("❌ Master not booted\n");
        return 1;
    }
    
    s24_imu_data_t imu;
    s24_gps_data_t gps;
    s24_lidar_data_t lidar;
    s24_camera_data_t camera;
    s24_battery_data_t battery;
    
    int errors = 0;
    
    if (s24_imu_read(&imu) == S24_OK && imu.valid) {
        printf("✅ IMU: ax=%.2f, ay=%.2f, az=%.2f\n", imu.x, imu.y, imu.z);
    } else {
        printf("❌ IMU read failed\n");
        errors++;
    }
    
    if (s24_gps_read(&gps) == S24_OK && gps.valid) {
        printf("✅ GPS: lat=%.6f, lon=%.6f, alt=%.1f\n", 
               gps.latitude, gps.longitude, gps.altitude);
    } else {
        printf("❌ GPS read failed\n");
        errors++;
    }
    
    if (s24_lidar_read(&lidar) == S24_OK && lidar.valid) {
        printf("✅ LiDAR: dist=%.2fm, intensity=%.1f\n", 
               lidar.distance, lidar.intensity);
    } else {
        printf("❌ LiDAR read failed\n");
        errors++;
    }
    
    if (s24_camera_read(&camera) == S24_OK && camera.valid) {
        printf("✅ Camera: %dx%d, size=%d bytes\n", 
               camera.width, camera.height, camera.data_size);
    } else {
        printf("❌ Camera read failed\n");
        errors++;
    }
    
    if (s24_battery_read(&battery) == S24_OK && battery.valid) {
        printf("✅ Battery: %.1fV, %.1f%%, %.1f°C\n", 
               battery.voltage, battery.charge_percent, battery.temperature);
    } else {
        printf("❌ Battery read failed\n");
        errors++;
    }
    
    if (errors == 0) {
        printf("✅ All sensor reads PASSED\n");
        return 0;
    } else {
        printf("❌ %d sensor reads FAILED\n", errors);
        return 1;
    }
}

int test_extended_cycles(void) {
    printf("\n========== TEST: Extended Cycles ==========\n");
    printf("Running 100 cycles...\n");
    
    for (int i = 0; i < 100; i++) {
        s24_imu_data_t imu;
        s24_lidar_data_t lidar;
        
        if (s24_imu_read(&imu) != S24_OK || !imu.valid) {
            printf("❌ IMU read failed at cycle %d\n", i);
            return 1;
        }
        
        if (s24_lidar_read(&lidar) != S24_OK || !lidar.valid) {
            printf("❌ LiDAR read failed at cycle %d\n", i);
            return 1;
        }
        
        HAL_TIMER.delay_ms(10);
    }
    
    printf("✅ All 100 cycles PASSED\n");
    return 0;
}

int test_shutdown_sequence(void) {
    printf("\n========== TEST: Shutdown Sequence ==========\n");
    
    s24_master_shutdown();
    
    if (!s24_master_is_booted()) {
        printf("✅ Shutdown sequence PASSED\n");
        return 0;
    } else {
        printf("❌ Shutdown sequence FAILED\n");
        return 1;
    }
}

/* ========== Main Test Runner ========== */

int main(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║       S24 Zero Sensor Simulator - Test Suite        ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    int total_tests = 0;
    int passed_tests = 0;
    
    // Test 1: Boot
    total_tests++;
    if (test_boot_sequence() == 0) passed_tests++;
    
    // Test 2: Sensor Reads
    total_tests++;
    if (test_sensor_reads() == 0) passed_tests++;
    
    // Test 3: Extended Cycles
    total_tests++;
    if (test_extended_cycles() == 0) passed_tests++;
    
    // Test 4: Shutdown
    total_tests++;
    if (test_shutdown_sequence() == 0) passed_tests++;
    
    printf("\n========================================================\n");
    printf("Tests Passed: %d/%d\n", passed_tests, total_tests);
    
    if (passed_tests == total_tests) {
        printf("\n✅ ALL TESTS PASSED - S24 Zero Sensor Simulator Ready\n");
        return 0;
    } else {
        printf("\n❌ %d TEST(S) FAILED\n", total_tests - passed_tests);
        return 1;
    }
}