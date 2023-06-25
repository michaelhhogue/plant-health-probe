///
/// Simple driver for reading moisture level from Adafruit soil
/// moisture sensor over I2C from RP2040 over (pre-initialized) I2C.
/// Performs bare-minimum functionality to communicate with the 
/// seesaw device.
/// (https://learn.adafruit.com/adafruit-stemma-soil-sensor-i2c-capacitive-moisture-sensor/overview)
///
/// Created by Michael Hogue.

#include "soil_moisture_seesaw.h"
#include "pico/stdlib.h"
#include "soil_moisture_seesaw.h"

#define I2C_ADDR 0x36   // I2C address of the device

const uint8_t TOUCH_BASE = 0x0F;            // Moisture sensor register base address
const uint8_t TOUCH_CHANNEL_OFFSET = 0x10;  // Moisture sensor register offset

/**
 * @brief Perform software reset on the soil moisture sensor.
 * 
 * @param i2c Initialized I2C block on RP2040.
 */
void seesaw_sw_reset(i2c_inst_t* i2c) {
    uint8_t reset_bytes[3] = {0x00, 0xFF, 0xFF};

    i2c_write_blocking(i2c, I2C_ADDR, reset_bytes, 3, false);
}

/**
 * @brief Reads moisture data from seesaw device.
 * 
 * @param i2c Initialized I2C block on RP2040.
 * @return uint16_t Moisture level: 200 (very dry) to 2000 (very wet)
 */
uint16_t seesaw_read_moisture(i2c_inst_t* i2c) {
    // Request data from moisture sensor registers
    uint8_t write_bytes[2] = {TOUCH_BASE, TOUCH_CHANNEL_OFFSET};
    i2c_write_blocking(i2c, I2C_ADDR, write_bytes, 2, false);

    sleep_ms(200);

    uint8_t buff[2];

    // Read soil moisture data
    i2c_read_blocking(i2c, I2C_ADDR, buff, 2, false);

    return ((uint16_t)buff[0] << 8) | buff[1];
}