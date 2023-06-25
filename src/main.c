/*

Plant Health Probe - main.c
Core functionality of the Plant Health Probe.

Created by Michael Hogue.

*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/sync.h"
#include "hardware/pio.h"
#include "viewmode_select.h"
#include "bh1750_light_sensor.h"
#include "ds18b20.h"
#include "soil_moisture_seesaw.h"
#include "graphics.h"

#define I2C_INSTANCE i2c1
#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7

#define MODE_SELECT_PIN 8

#define PIO_INSTANCE pio0
#define ONE_WIRE_PIN 9

// Stores sensor data
typedef struct {
    int16_t temperature;
    uint16_t lux;
    uint16_t moisture;

} sensor_data_t;

// Flag set to true when the delay timer ISR has fired
static volatile bool cycle_timer_flag = false;

// This variable is shared between the two cores
// and stores the sensor readings sampled
// in the background from Core1.
// Core0: READ ONLY from this value.
// Core1: WRITE ONLY to this value
// NOTE: temperature value <= -100 indicates no 
// readings have been stored here yet.
static volatile sensor_data_t shared_sensor_data = {0, 0, -100};

// PIO state machine instance set at initialization
static int8_t pio_sm = -1;

/**
 * @brief ISR for the delay cycle timer.
 * 
 * @return int64_t Additional delay time. Should always return 0.
 */
int64_t cycle_timer_isr() {
    cycle_timer_flag = true;

    return 0;
}

/**
 * @brief Entry point for core1. This processor is responsible for
 * sampling data from each sensor in a round-robin fashion. This allows
 * core0 to handle user input/output fast.
 * 
 */
void core1_entry() {
    // Initialize temperature sensor
    gpio_init(ONE_WIRE_PIN);
    gpio_pull_up(ONE_WIRE_PIN);
    pio_sm = ds18b20_init(PIO_INSTANCE, ONE_WIRE_PIN);

    // Print message if a SM could not be initialized
    if (pio_sm == -1) {
        puts("A PIO state machine could not be initialized.");
    }

    // Initialize I2C block
    i2c_init(I2C_INSTANCE, 100000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

    // Initialize ambient light sensor
    bh1750_power_on(I2C_INSTANCE);

    // Initialize soil moisture sensor
    seesaw_sw_reset(I2C_INSTANCE);

    // Repeatedly sample sensor data. 
    // ds18b20 sampling includes 1s delay.
    while(1) {
        int8_t temperature = ds18b20_get_temperature(PIO_INSTANCE, pio_sm, true);

        uint16_t lux = bh1750_read_measurement(I2C_INSTANCE);

        uint16_t moisture = seesaw_read_moisture(I2C_INSTANCE);

        shared_sensor_data.temperature = temperature;
        shared_sensor_data.lux = lux;
        shared_sensor_data.moisture = moisture;
    }
}

/**
 * @brief Performs initialization of I/O, as well as starting
 * sensor sampling on Core1.
 * 
 */
void initialize(void) {
    // Setup GPIO IRQ for mode select button
    setup_viewmodeselect_irq(MODE_SELECT_PIN);

    // Start sensor sampling on Core1
    multicore_reset_core1();
    sleep_ms(100);
    multicore_launch_core1(core1_entry);

    // Initialize graphics
    graphics_init();
}

/**
 * @brief Performs a power-efficient 0.5s delay. This delay will be 
 * cut short if the view-mode switch button is pressed.
 * 
 * @param prev_view_mode The view-mode which was set before starting\
 * the delay.
 */
void delay_until_alarm_or_modeswitch(view_mode_t prev_view_mode) {
    // Create 0.5 second run cycle delay
    alarm_id_t cycle_alarm = add_alarm_in_ms(500, cycle_timer_isr, NULL, true);

    // Wait for an interrupt.
    // If it was cycle alarm
    // or mode switch, continue.
    while(!cycle_timer_flag && prev_view_mode == get_viewmode()) {
        __wfi();
    }

    // Reset cycle alarm
    cancel_alarm(cycle_alarm);
    cycle_timer_flag = false;
}

/**
 * @brief Shows sensor-data view on LCD based on the current view mode.
 * 
 * @param view_mode The current view-mode.
 */
void output_data(view_mode_t view_mode) {
    // // Get local copy of shared sensor data onto the stack.
     sensor_data_t local_sensor_data = shared_sensor_data;

    // // If no sensor data has yet been stored, display message.
    if (local_sensor_data.temperature <= -100) {
        show_loading_view();
        return;
    }

    switch (view_mode) {
        case DUAL:
            show_dual_view( 
                local_sensor_data.moisture,
                local_sensor_data.lux,
                local_sensor_data.temperature
            );
        break;
        case SOIL:
            show_soil_view(
                local_sensor_data.moisture, 
                local_sensor_data.temperature
            );
        break;
        case LIGHT:
            show_light_view(
                local_sensor_data.lux, 
                local_sensor_data.temperature
            );
        break;
        default:
            show_critical_error_view();
    }
}

/**
 * @brief Entry point of system. Contains the run-loop.
 *
 */
int main(void) {
    // Perform initialization of all components
    initialize();    

    /* -- Run Loop -- */

    // Keep track of plant mode index from previous cycle
    view_mode_t prev_view_mode = SOIL;
    while (1) {
        // Get the current active plant mode and the plant data
        // parameters associated with that mode.
        view_mode_t view_mode = get_viewmode();
        
        // Output data
        output_data(view_mode);

        // Delay for 0.5s or until mode switch button is pressed
        delay_until_alarm_or_modeswitch(prev_view_mode);

        // Update the previous mode index
        prev_view_mode = view_mode;
    }

    /* -- End Run Loop -- */

    return 0;
}
