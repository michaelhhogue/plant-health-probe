#ifndef SOIL_MOISTURE_SEESAW_H
#define SOIL_MOISTURE_SEESAW_H

#include "hardware/i2c.h"

void seesaw_sw_reset(i2c_inst_t* i2c);

uint16_t seesaw_read_moisture(i2c_inst_t* i2c);

#endif