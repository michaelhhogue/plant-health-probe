/*

Responsible for the interrupt caused by pressing the view-mode switch button.

Created by Michael Hogue.

*/

#include "viewmode_select.h"
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#define _MODE_HIGHEST 2
#define _DEBOUNCE_TIME 200000

// For button debouncing
static volatile absolute_time_t _last_interrupt_time = {0};

// Stores state of the current view mode
static volatile view_mode_t _view_mode = DUAL;

/**
 * @brief ISR for interrupt triggered by mode select button press.
 * 
 * @param gpio GPIO that caused the interrupt.
 * @param event_mask Active interrupt(s).
 */
void _viewmodeselect_isr(uint gpio, uint32_t event_mask) {
    absolute_time_t current_interrupt_time = get_absolute_time();
    int64_t time_diff = absolute_time_diff_us(_last_interrupt_time, current_interrupt_time);

    if (time_diff > _DEBOUNCE_TIME) {
        irq_set_mask_enabled(event_mask, false);

        // Rotate the current view mode
        if (_view_mode == LIGHT) {
            _view_mode = DUAL;
        } else {
            _view_mode++;
        }

        irq_set_mask_enabled(event_mask, true);
    }

    _last_interrupt_time = current_interrupt_time;
}

/**
 * @brief Sets up the interrupt for view-mode select button press.
 * 
 * @param gpio Pin for the view-mode select button
 */
void setup_viewmodeselect_irq(uint gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
    gpio_pull_down(gpio);

    gpio_set_irq_enabled_with_callback(gpio, GPIO_IRQ_EDGE_FALL, true, _viewmodeselect_isr); 
}

/**
 * @brief Get the active view-mode
 * 
 * @return view_mode_t The active view-mode
 */
view_mode_t get_viewmode(void) {
    return _view_mode;
}