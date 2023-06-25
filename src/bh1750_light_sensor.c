///
/// bh1750_light_sensor.c
///
/// Simple driver for interfacing with the 
/// BH1750 light sensor over I2C.
/// This interface assumes a I2C block & 
/// bus has been initialized already.
///
/// Created by Michael Hogue.
///

#include "bh1750_light_sensor.h"

#define _BH1750_I2C_ADDR 0x23       // Device's I2C address

const uint8_t _POWER_ON_C = 0x01;   // Power on command
const uint8_t _CONT_HRES_C = 0x10;  // Continuous high-res measurment command

/**
 * @brief Push one byte of data to TX FIFO.
 * 
 * @param i2c Initialized RP2040 I2C block.
 * @param byte Byte of data to push.
 */
void _i2c_write_byte(i2c_inst_t* i2c, uint8_t byte) {
    i2c_write_blocking(i2c, _BH1750_I2C_ADDR, &byte, 1, false);
}

/**
 * @brief Powers on the BH1750.
 * 
 * @param i2c Initialized RP2040 I2C block.
 */
void bh1750_power_on(i2c_inst_t* i2c) {
    _i2c_write_byte(i2c, _POWER_ON_C);
}

/**
 * @brief Get a measurement of ambient light from the BH1750.
 * 
 * @param i2c Initialized RP2040 I2C block.
 * @return uint16_t Measurement result (lux).
 */
uint16_t bh1750_read_measurement(i2c_inst_t* i2c) {
    // Send "Continuously H-resolution mode" instruction
    _i2c_write_byte(i2c, _CONT_HRES_C);

    // Wait at least 180 ms to complete measurement
    sleep_ms(200);

    uint8_t buff[2];

    i2c_read_blocking(i2c, _BH1750_I2C_ADDR, buff, 2, false);

    return (((uint16_t)buff[0] << 8) | buff[1]) / 1.2;
}