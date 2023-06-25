/*

Handles displaying views based on the sensor data to the LCD.

Created by Michael Hogue

*/

#include "graphics.h"
#include <stdio.h>
#include "lcd.h"

/**
 * @brief Displays the top header showing the current temperature
 * and active view mode.
 * 
 * @param temperature The current temperature
 * @param mode_label The active view mode
 */
void _display_header(int8_t temperature, char mode_label[5]) {
    // Draw header underline
    lcd_draw_rect(0, 9, 83, 9, true);

    lcd_set_cursor(0, 0);

    char text_line[11];
    sprintf(text_line, "%3dF %5s", temperature, mode_label);
    lcd_print_str(text_line, false);

    // Ensure text is not written on this header
    // by shifting the cursor down a line.
    lcd_set_cursor(0, 1);
}

/**
 * @brief Displays a percentage bar spanning the full width of the
 * display at the given y position. The bar is 8px tall.
 * 
 * @param percentage Portion of the bar to fill
 * @param top_y Top y-position of the bar
 */
void _display_percentage_bar(float percentage, uint8_t top_y) {
    // Draw outline
    lcd_draw_rect(0, top_y, 83, top_y + 7, false);

    // Max percentage to 100%
    percentage = MIN(percentage, 1);

    // Draw fill
    uint8_t to_x = percentage * 81;
    lcd_draw_rect(2, top_y + 2, to_x, top_y + 5, true);
}

/**
 * @brief Initializes the LCD
 * 
 */
void graphics_init(void) {
    // Initialize LCD
    lcd_init();

    // Show the custom splash screen
    show_splashscreen();
}

/**
 * @brief Clears the LCD
 * 
 */
void clear_current_view(void) {
    clear_lcd();
}

/**
 * @brief Shows a view that says "LOADING..."
 * 
 */
void show_loading_view(void) {
    clear_lcd();
    lcd_set_cursor(0, 2);
    lcd_print_str("LOADING...", true);
}

/**
 * @brief Shows a view that says "CRITICAL ERROR!"
 * 
 */
void show_critical_error_view(void) {
    clear_lcd();
    lcd_set_cursor(0, 2);
    lcd_print_str("  CRITICAL  ERROR!", true);
}

/**
 * @brief Shows the dual view which shows percentage bar and title
 * for both moisture and light sensor data.
 * Also includes the given temperature in the header.
 * 
 * @param moisture Soil moisture value
 * @param lux Ambient light value
 * @param temperature Current temperature
 */
void show_dual_view(uint16_t moisture, uint16_t lux, int8_t temperature) {
    clear_lcd();

    _display_header(temperature, "DUAL");

    lcd_newline();

    lcd_print_str("MOISTURE: ", false);

    _display_percentage_bar((((float)moisture) - 200) / 1000, 24);

    lcd_set_cursor(0, 4);

    lcd_print_str("LIGHT: ", false);

    _display_percentage_bar((float)lux / 32000, 40);

    flush_lcd_buffer();
}

/**
 * @brief Shows details for the soil moisture data.
 * Also includes the given temperature in the header.
 * 
 * @param moisture Soil moisture value
 * @param temperature Current temperature
 */
void show_soil_view(uint16_t moisture, int8_t temperature) {  
    clear_lcd();

    _display_header(temperature, "SOIL");

    lcd_newline();

    lcd_print_str("MOISTURE: ", false);

    float moisture_percentage = (((float)moisture) - 200) / 1000;

    _display_percentage_bar(moisture_percentage, 32);

    lcd_set_cursor(0, 5);

    if (moisture_percentage < 0.2) {
        lcd_print_str("    DRY   ", false);
    } else if (moisture_percentage < 0.5) {
        lcd_print_str("   MOIST  ", false);
    } else if (moisture_percentage < 0.8) {
        lcd_print_str("    WET   ", false);
    } else {
        lcd_print_str(" VERY WET ", false);
    }

    flush_lcd_buffer();
}

/**
 * @brief Shows details for the ambient light data.
 * Also includes the given temperature in the header.
 * 
 * @param lux Ambient light value
 * @param temperature Current temperature
 */
void show_light_view(uint16_t lux, int8_t temperature) {
    clear_lcd();

    _display_header(temperature, "LIGHT");

    lcd_newline();

    lcd_print_str("QUALITY: ", false);

    _display_percentage_bar((float)lux / 32000, 32);

    lcd_set_cursor(0, 5);

    if (lux > 32000) {
        lcd_print_str(" SUNLIGHT ", false);
    } else if (lux > 10000) {
        lcd_print_str("   SHADE  ", false);
    } else {
        lcd_print_str("  TOO DIM ", false);
    }

    flush_lcd_buffer();
}