#ifndef S24_CONTRACT_H
#define S24_CONTRACT_H

#include <stdint.h>
#include <stdbool.h>

/* ============================================
   HAL Contract - Matches S09 Hardware Layer
   ============================================ */

/* Return codes - MUST match S09's actual return types */
typedef enum {
    S24_OK = 0,
    S24_ERROR = -1,
    S24_BUSY = -2,
    S24_TIMEOUT = -3
} s24_status_t;

/* ========== GPIO Contract ========== */
typedef struct {
    void (*init)(uint32_t pin, bool output);
    void (*write)(uint32_t pin, bool state);
    bool (*read)(uint32_t pin);
    void (*toggle)(uint32_t pin);
} s24_gpio_t;

/* ========== SPI Contract ========== */
typedef struct {
    s24_status_t (*init)(uint32_t spi_id);
    s24_status_t (*transfer)(uint32_t spi_id, uint8_t *tx, uint8_t *rx, uint32_t len);
    s24_status_t (*cs_control)(uint32_t spi_id, bool enable);
} s24_spi_t;

/* ========== I2C Contract ========== */
typedef struct {
    s24_status_t (*init)(uint32_t i2c_id);
    s24_status_t (*write)(uint32_t i2c_id, uint8_t dev_addr, uint8_t *data, uint32_t len);
    s24_status_t (*read)(uint32_t i2c_id, uint8_t dev_addr, uint8_t *data, uint32_t len);
    s24_status_t (*write_read)(uint32_t i2c_id, uint8_t dev_addr, 
                               uint8_t *tx, uint32_t tx_len,
                               uint8_t *rx, uint32_t rx_len);
} s24_i2c_t;

/* ========== Timer Contract ========== */
typedef struct {
    void (*delay_ms)(uint32_t ms);
    void (*delay_us)(uint32_t us);
    uint32_t (*get_tick)(void);
    void (*reset_tick)(void);
} s24_timer_t;

/* ========== Global HAL Access ========== */
extern s24_gpio_t HAL_GPIO;
extern s24_spi_t HAL_SPI;
extern s24_i2c_t HAL_I2C;
extern s24_timer_t HAL_TIMER;

/* ========== Sensor Data Structures ========== */
typedef struct {
    float x, y, z;
    float temperature;
    uint64_t timestamp_ms;
    bool valid;
} s24_imu_data_t;

typedef struct {
    double latitude, longitude, altitude;
    float speed, heading;
    uint8_t fix_quality;
    uint64_t timestamp_ms;
    bool valid;
} s24_gps_data_t;

typedef struct {
    float distance;
    float intensity;
    uint16_t point_count;
    uint64_t timestamp_ms;
    bool valid;
} s24_lidar_data_t;

typedef struct {
    uint16_t width, height;
    uint8_t *pixel_data;  // Simulated - we'll use dummy data
    uint32_t data_size;
    uint64_t timestamp_ms;
    bool valid;
} s24_camera_data_t;

typedef struct {
    float voltage;
    float current;
    float charge_percent;
    float temperature;
    bool charging;
    uint64_t timestamp_ms;
    bool valid;
} s24_battery_data_t;

#endif // S24_CONTRACT_H