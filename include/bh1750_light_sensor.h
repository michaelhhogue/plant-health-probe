#ifndef BH1750_LIGHT_SENSOR_H
#define BH1750_LIGHT_SENSOR_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

void _i2c_write_byte(i2c_inst_t* i2c, uint8_t byte); 

void bh1750_power_on(i2c_inst_t* i2c);

uint16_t bh1750_read_measurement(i2c_inst_t* i2c);

#endif