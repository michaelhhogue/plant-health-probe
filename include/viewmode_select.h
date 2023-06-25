#ifndef VIEWMODE_SELECT_H
#define VIEWMODE_SELECT_H

#include "pico/stdlib.h"

typedef enum {DUAL, SOIL, LIGHT} view_mode_t;

void setup_viewmodeselect_irq(uint gpio);

view_mode_t get_viewmode(void);

#endif