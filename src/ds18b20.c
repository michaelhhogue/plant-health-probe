#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "hardware/pio.h"
#include "ds18b20.pio.h"
#include "ds18b20.h"

/**
 * @brief Writes given set of bytes to the DS18B20 from the
 * specified SM on the specified PIO.
 * 
 * @param pio PIO block containing the SM interfacing with
 * the DS18B20.
 * @param sm State Machine interfacing with the DS18B20.
 * @param bytes Array of bytes to write to the SM's FIFO.
 * @param len Number of bytes to write.
 */
void _writeBytes(PIO pio, uint sm, uint8_t bytes[], int len) {
    pio_sm_put_blocking(pio, sm, 250);
    pio_sm_put_blocking(pio, sm, len - 1);

    for (int i = 0; i < len; i++) {
        pio_sm_put_blocking(pio, sm, bytes[i]);
    }
}

/**
 * @brief Reads bytes from the DS18B20 of count 'len'.
 * 
 * @param pio PIO block containing the SM interfacing with
 * the DS18B20.
 * @param sm State Machine interfacing with the DS18B20.
 * @param bytes Array of bytes to read data into.
 * @param len Number of bytes to read.
 */
void _readBytes(PIO pio, uint sm, uint8_t bytes[], int len) {
    pio_sm_put_blocking(pio, sm, 0);
    pio_sm_put_blocking(pio, sm, len - 1);

    for (int i = 0; i < len; i++) {
        bytes[i] = pio_sm_get_blocking(pio, sm) >> 24;
    }
}

/**
 * @brief Gets the current temperature from the DS18B20.
 * 
 * @param pio PIO block containing the SM interfacing with
 * the DS18B20.
 * @param sm State Machine interfacing with the DS18B20.
 * @param in_fahrenheit True if temperature should be 
 * converted to fahrenheit. Otherwise, it will be left
 * in Celsius.
 * @return int8_t Temperature measured from the DS18B20
 * as a signed 8-bit integer.
 */
int8_t ds18b20_get_temperature(PIO pio, uint sm, bool in_fahrenheit) {
    _writeBytes(pio, sm, (uint8_t[]){0xCC, 0x44}, 2);

    sleep_ms(1000);

    _writeBytes(pio, sm, (uint8_t[]){0xCC, 0xBE}, 2);

    uint8_t data[2];

    _readBytes(pio, sm, data, 2);

    volatile int8_t temperature = (data[1] << 4 | data[0] >> 4);

    return in_fahrenheit ? (temperature * 9.0/5.0) + 32 : temperature; 
}

/**
 * @brief Initializes the PIO State Machine needed to
 * interface with the DS18B20 over the 1-wire bus.
 * 
 * @param pio Which PIO the SM should be requested from.
 * @param gpio The GPIO pin connecting to the 1-wire bus.
 * @return int index of the SM being used, or -1 if one
 * could not be accquired.
 */
int ds18b20_init(PIO pio, int gpio) {
    uint offset = pio_add_program(pio, &ds18b20_program);
    int sm = pio_claim_unused_sm(pio, true);

    if (sm == -1) {
        printf("ds18b20_init: Could not get an unused state machine.\n");

        return sm;
    }

    pio_gpio_init(pio, gpio);

    pio_sm_config c = ds18b20_program_get_default_config(offset);

    sm_config_set_clkdiv_int_frac(&c, 266, 0);
    sm_config_set_set_pins(&c, gpio, 1);
    sm_config_set_out_pins(&c, gpio, 1);
    sm_config_set_in_pins(&c, gpio);
    sm_config_set_in_shift(&c, true, true, 8);
    pio_sm_init(pio0, sm, offset, &c);
    pio_sm_set_enabled(pio0, sm, true);

    return sm;
}