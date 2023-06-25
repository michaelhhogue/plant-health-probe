#ifndef DS18B20_H
#define DS18B20_H

 #include "hardware/pio.h"

void _writeBytes(PIO pio, uint sm, uint8_t bytes[], int len);

void _readBytes(PIO pio, uint sm, uint8_t bytes[], int len);

int8_t ds18b20_get_temperature(PIO pio, uint sm, bool in_fahrenheit);

int ds18b20_init(PIO pio, int gpio);

#endif