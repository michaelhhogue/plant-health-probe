#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "pico/stdlib.h"

void graphics_init(void);

void clear_current_view(void);

void show_loading_view(void);

void show_critical_error_view(void);

void show_dual_view(uint16_t moisture, uint16_t lux, int8_t temperature);

void show_soil_view(uint16_t moisture, int8_t temperature);

void show_light_view(uint16_t lux, int8_t temperature);

#endif