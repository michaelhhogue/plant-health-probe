/*

Driver and simple graphics library for the NOKIA 5110 monochrome LCD.

Created by Michael Hogue

*/

#include "lcd.h"
#include "hardware/spi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LCD_COMMAND 0
#define LCD_DATA 1

#define SPI_INST spi0
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19
#define PIN_DC   20
#define PIN_RST  21

#define LCD_BUF_SIZE (48 * 84) / 8

// Simple font table. Based on BBC-Micro font.
const uint8_t _FONT_TABLE[][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // 32 ' '
    {0x00, 0x00, 0x00, 0xde, 0xde, 0x00, 0x00, 0x00}, // 33 '!'
    {0x00, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x00}, // 34 '"'
    {0x00, 0x24, 0x7e, 0x24, 0x24, 0x7e, 0x24, 0x00}, // 35 '#'
    {0x00, 0x00, 0x58, 0x54, 0xfe, 0x54, 0x34, 0x00}, // 36 '$'
    {0x00, 0xc6, 0x66, 0x30, 0x18, 0xcc, 0xc6, 0x00}, // 37 '%'
    {0x00, 0x6c, 0x92, 0x92, 0xba, 0x44, 0xa0, 0x00}, // 38 '&'
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00}, // 39 '''
    {0x00, 0x00, 0x00, 0x7c, 0xc6, 0x82, 0x00, 0x00}, // 40 '('
    {0x00, 0x00, 0x82, 0xc6, 0x7c, 0x00, 0x00, 0x00}, // 41 ')'
    {0x24, 0x3c, 0x18, 0x7e, 0x7e, 0x18, 0x3c, 0x24}, // 42 '*'
    {0x00, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x00}, // 43 '+'
    {0x00, 0x00, 0x80, 0xe0, 0x60, 0x00, 0x00, 0x00}, // 44 ','
    {0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00}, // 45 '-'
    {0x00, 0x00, 0x00, 0xc0, 0xc0, 0x00, 0x00, 0x00}, // 46 '.'
    {0x00, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x00}, // 47 '/'
    {0x00, 0x7c, 0xfe, 0xb2, 0x9a, 0xfe, 0x7c, 0x00}, // 48 '0'
    {0x00, 0x80, 0x84, 0xfe, 0xfe, 0x80, 0x80, 0x00}, // 49 '1'
    {0x00, 0x84, 0xc6, 0xe2, 0xb2, 0x9e, 0x8c, 0x00}, // 50 '2'
    {0x00, 0x44, 0xc6, 0x92, 0x92, 0xfe, 0x6c, 0x00}, // 51 '3'
    {0x00, 0x30, 0x38, 0x2c, 0xfe, 0xfe, 0x20, 0x00}, // 52 '4'
    {0x00, 0x4e, 0xce, 0x8a, 0x8a, 0xfa, 0x72, 0x00}, // 53 '5'
    {0x00, 0x78, 0xfc, 0x96, 0x92, 0xf2, 0x60, 0x00}, // 54 '6'
    {0x00, 0x02, 0xe2, 0xf2, 0x1a, 0x0e, 0x06, 0x00}, // 55 '7'
    {0x00, 0x6c, 0xfe, 0x92, 0x92, 0xfe, 0x6c, 0x00}, // 56 '8'
    {0x00, 0x0c, 0x9e, 0x92, 0xd2, 0x7e, 0x3c, 0x00}, // 57 '9'
    {0x00, 0x00, 0x00, 0x6c, 0x6c, 0x00, 0x00, 0x00}, // 58 ':'
    {0x00, 0x00, 0x80, 0xec, 0x6c, 0x00, 0x00, 0x00}, // 59 ';'
    {0x00, 0x10, 0x10, 0x28, 0x28, 0x44, 0x44, 0x00}, // 60 '<'
    {0x00, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x00}, // 61 '='
    {0x00, 0x44, 0x44, 0x28, 0x28, 0x10, 0x10, 0x00}, // 62 '>'
    {0x00, 0x04, 0x06, 0x52, 0x52, 0x0e, 0x0c, 0x00}, // 63 '?'
    {0x00, 0x38, 0x44, 0xb2, 0xaa, 0xba, 0x24, 0x18}, // 64 '@'
    {0x00, 0xfc, 0xfe, 0x12, 0x12, 0xfe, 0xfc, 0x00}, // 65 'A'
    {0x00, 0xfe, 0xfe, 0x92, 0x92, 0xfe, 0x6c, 0x00}, // 66 'B'
    {0x00, 0x7c, 0xfe, 0x82, 0x82, 0xc6, 0x44, 0x00}, // 67 'C'
    {0x00, 0xfe, 0xfe, 0x82, 0xc6, 0x7c, 0x38, 0x00}, // 68 'D'
    {0x00, 0xfe, 0xfe, 0x92, 0x92, 0x92, 0x82, 0x00}, // 69 'E'
    {0x00, 0xfe, 0xfe, 0x12, 0x12, 0x12, 0x02, 0x00}, // 70 'F'
    {0x00, 0x7c, 0xfe, 0x82, 0x92, 0xf6, 0x64, 0x00}, // 71 'G'
    {0x00, 0xfe, 0xfe, 0x10, 0x10, 0xfe, 0xfe, 0x00}, // 72 'H'
    {0x00, 0x82, 0x82, 0xfe, 0xfe, 0x82, 0x82, 0x00}, // 73 'I'
    {0x00, 0x40, 0xc2, 0x82, 0xfe, 0x7e, 0x02, 0x00}, // 74 'J'
    {0x00, 0xfe, 0xfe, 0x38, 0x6c, 0xc6, 0x82, 0x00}, // 75 'K'
    {0x00, 0xfe, 0xfe, 0x80, 0x80, 0x80, 0x80, 0x00}, // 76 'L'
    {0x00, 0xfe, 0xfe, 0x0c, 0x38, 0x0c, 0xfe, 0xfe}, // 77 'M'
    {0x00, 0xfe, 0xfe, 0x18, 0x30, 0xfe, 0xfe, 0x00}, // 78 'N'
    {0x00, 0x7c, 0xfe, 0x82, 0x82, 0xfe, 0x7c, 0x00}, // 79 'O'
    {0x00, 0xfe, 0xfe, 0x12, 0x12, 0x1e, 0x0c, 0x00}, // 80 'P'
    {0x00, 0x7c, 0xfe, 0x82, 0x62, 0xde, 0xbc, 0x00}, // 81 'Q'
    {0x00, 0xfe, 0xfe, 0x12, 0x32, 0xfe, 0xcc, 0x00}, // 82 'R'
    {0x00, 0x4c, 0xde, 0x92, 0x92, 0xf6, 0x64, 0x00}, // 83 'S'
    {0x00, 0x02, 0x02, 0xfe, 0xfe, 0x02, 0x02, 0x00}, // 84 'T'
    {0x00, 0x7e, 0xfe, 0x80, 0x80, 0xfe, 0x7e, 0x00}, // 85 'U'
    {0x00, 0x3e, 0x7e, 0xc0, 0xc0, 0x7e, 0x3e, 0x00}, // 86 'V'
    {0x00, 0xfe, 0xfe, 0x60, 0x38, 0x60, 0xfe, 0xfe}, // 87 'W'
    {0x00, 0xc6, 0xee, 0x38, 0x38, 0xee, 0xc6, 0x00}, // 88 'X'
    {0x00, 0x06, 0x0e, 0xf8, 0xf8, 0x0e, 0x06, 0x00}, // 89 'Y'
    {0x00, 0xc2, 0xe2, 0xb2, 0x9a, 0x8e, 0x86, 0x00}, // 90 'Z'
};

// Memory buffer for the LCD
static uint8_t display_buffer[LCD_BUF_SIZE];

// Current x/y position of the text cursor
static uint8_t cursor_x_pos = 0; // Max: 9
static uint8_t cursor_y_pos = 0; // Max: 5

/**
 * @brief Send the given command byte to the LCD.
 * 
 * @param cmd Command to send
 */
void _lcd_cmd(uint8_t cmd) {
    gpio_put(PIN_DC, LCD_COMMAND);
    spi_write_blocking(SPI_INST, &cmd, 1);
}

/**
 * @brief Send data of 'len' bytes to LCD memory.
 * 
 * @param data Bytes to send to LCD memory
 * @param len Number of bytes to send
 */
void _lcd_data(uint8_t *data, size_t len) {
    gpio_put(PIN_DC, LCD_DATA);
    spi_write_blocking(SPI_INST, data, len);
}

/**
 * @brief Set the display buffer to all zeros.
 * 
 */
void _clear_buffer(void) {
    memset(display_buffer, 0, LCD_BUF_SIZE);
}

/**
 * @brief Move the text cursor forward one position.
 * If it has reached the end of the line, it will 
 * wrap around to the next.
 * 
 */
void _increment_cursor(void) {
    if (cursor_x_pos >= 9) {
        cursor_x_pos = 0;
        cursor_y_pos++;
    } else {
        cursor_x_pos++;
    }

    if (cursor_y_pos >= 6) {
        cursor_y_pos = 0;
    }
}

/**
 * @brief Initialize the LCD and display buffer.
 * 
 */
void lcd_init(void) {
    spi_init(SPI_INST, 4000000);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(PIN_DC);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_RST, GPIO_OUT);

    // Reset LCD
    gpio_put(PIN_RST, 0);
    sleep_ms(100);
    gpio_put(PIN_RST, 1);

    // Initialize LCD
    _lcd_cmd(0x21);  // Extended instruction set
    _lcd_cmd(0x90);  // Set V_op to +16 x b[V]
    _lcd_cmd(0x20);  // Basic instruction set
    _lcd_cmd(0x0C);  // Normal mode
    _lcd_cmd(0x80);  // Set X-address of RAM to 0
    _lcd_cmd(0x40);  // Set Y-address of RAM to 0

    _clear_buffer();
    _lcd_data(display_buffer, LCD_BUF_SIZE);

    cursor_x_pos = 0;
    cursor_y_pos = 0;
}

/**
 * @brief Send the contents of the entire buffer to
 * LCD memory.
 * 
 */
void flush_lcd_buffer(void) {
    _lcd_data(display_buffer, LCD_BUF_SIZE);
}

/**
 * @brief Clears the entire LCD and resets the text cursor.
 * 
 */
void clear_lcd(void) {
    _clear_buffer();
    flush_lcd_buffer();
    _lcd_cmd(0x80);  // Set X-address of RAM to 0
    _lcd_cmd(0x40);  // Set Y-address of RAM to 0
}

/**
 * @brief Shows the "HOGUE" splashscreen for 3 seconds then
 * clears the screen.
 * 
 */
void show_splashscreen(void) {
    const uint8_t heading[] =
    {
        0xc0, 0xe0, 0xc0, 0xf0, 0xc0, 0xe0, 0xf0, 0xc0, 0x80, 0xe0, 
        0x80, 0xf0, 0xc0, 0xc0, 0xe0, 0xc0, 0xf0, 0xe0, 0xc0, 0xe0, 
        0xc0, 0xf8, 0xc0, 0xe0, 0xf0, 0xc0, 0xf0, 0xe0, 0xc0, 0xf0, 
        0xe0, 0xc0, 0xe0
    };
    const uint8_t splash_top[] =
    {
        0xff, 0x00, 0xfc, 0x80, 0x80, 0x80, 0xfc, 0x00, 0xfc, 0x04, 
        0x04, 0x04, 0xfc, 0x00, 0xfc, 0x04, 0x04, 0x04, 0x1c, 0x00, 
        0xfc, 0x00, 0x00, 0x00, 0xfc, 0x00, 0xfc, 0x84, 0x84, 0x84, 
        0x04, 0x00, 0xff
    };

    const uint8_t splash_bottom[] =
    {
        0xff, 0x80, 0xbf, 0x80, 0x80, 0x80, 0xbf, 0x80, 0xbf, 0xa0,
        0xa0, 0xa0, 0xbf, 0x80, 0xbf, 0xa0, 0xa1, 0xa1, 0xbf, 0x80,
        0xbf, 0xa0, 0xa0, 0xa0, 0xbf, 0x80, 0xbf, 0xa0, 0xa0, 0xa0,
        0xa0, 0x80, 0xff
    };

    memcpy(display_buffer + 109, heading, 33);
    memcpy(display_buffer + 193, splash_top, 33);
    memcpy(display_buffer + 277, splash_bottom, 33);

    flush_lcd_buffer();

    sleep_ms(3000);

    clear_lcd();
}

/**
 * @brief Moves the cursor down a line or back to the first line
 * if already at the bottom.
 * 
 */
void lcd_newline(void) {
    if (cursor_y_pos >= 6) {
        cursor_y_pos = 0;
    } else {
        cursor_y_pos++;
    }

    cursor_x_pos = 0;
}

/**
 * @brief Sets the text cursor to specified x/y position.
 * 
 * @param x X position of cursor
 * @param y Y position of cursor
 * @return uint8 1 if attempting to set cursor out of bounds.
 * 0 if cursor was set successfully.
 */
int lcd_set_cursor(uint8_t x, uint8_t y) {
    if (x > 9 || y > 5) {
        puts("(LCD) Attempting to set cursor coordinates out of bounds.");
        return 1;
    }

    cursor_x_pos = x;
    cursor_y_pos = y;

    return 0;
}

/**
 * @brief Gets the current position of the text cursor.
 * 
 * @param x Pointer to the X position value to set.
 * @param y Pointer to the Y position value to set.
 */
void lcd_get_cursor_position(uint8_t* x, uint8_t* y) {
    *x = cursor_x_pos;
    *y = cursor_y_pos;
}

/**
 * @brief Draws a 1px-wide column at x position between two y values.
 * 
 * @param for_x X position of the column
 * @param y1 Top of column
 * @param y2 Bottom of column
 */
void _rect_draw_column(uint16_t for_x, uint8_t y1, uint8_t y2) {
    for (uint16_t y = y1; y <= y2; y++) {
        uint16_t index = ((y / 8) * 84) + for_x;
        uint8_t bit = 1 << (y % 8);
        display_buffer[index] |= bit;
    }
}

/**
 * @brief Draws a rectangle from a top-left point to a bottom-right point
 * Note: The buffer is NOT flushed upon completion.
 * 
 * @param x1 X-coordinate of top-left point 
 * @param y1 Y-coordinate of top-left point 
 * @param x2 X-coordinate of bottom-right point 
 * @param y2 Y-coordinate of bottom-right point 
 * @param fill Fill rect if true. Otherwise it will be a 1px outline.
 * @return uint8_t 1 if any point is out of bounds. Otherwise, 0.
 */
uint8_t lcd_draw_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool fill) {
    if (x1 > 83 || y1 > 47 || x2 > 83 || y2 > 47) {
        printf("(LCD) lcd_draw_rect: Coordinates out of bounds.\n");
        return 1;
    }

    if (!fill) {
        for (uint16_t x = x1; x <= x2; x++) {
            if (x != x1 && x != x2) {
                uint16_t top_index = ((y1 / 8) * 84) + x;
                uint16_t bottom_index = ((y2 / 8) * 84) + x;
                uint8_t top_bit = 1 << (y1 % 8);
                uint8_t bottom_bit = 1 << (y2 % 8);
                display_buffer[top_index] |= top_bit;
                display_buffer[bottom_index] |= bottom_bit;
            } else {
                _rect_draw_column(x, y1, y2);
            }
        }

        return 0;
    }

    for (uint16_t x = x1; x <= x2; x++) {
        _rect_draw_column(x, y1, y2);
    }

    return 0;
}

/**
 * @brief Draws a 8x8 pixel bitmap at point representing its top-left corner.
 * Note: The buffer is NOT flushed upon completion.
 * 
 * @param bitmap Bitmap to draw
 * @param x Top-left x position (Max: 83)
 * @param y Top-left y position (Max: 47)
 * * @return uint8_t 1 if point is out of bounds. Otherwise, 0.
 */
uint8_t lcd_draw_bitmap_8x8(const uint8_t bitmap[8], uint8_t x, uint8_t y) {
    if (x > 83 || y > 47) {
        printf("(LCD) lcd_draw_bitmap_8x8: Coordinates out of bounds.\n");
        return 1;
    }
    uint16_t start_index = ((y / 8) * 84) + x;

    for (uint16_t i = 0; i <= 7; i++) {
        if (start_index + i >= LCD_BUF_SIZE)
            return 0;

        display_buffer[start_index + i] = bitmap[i];
    }

    return 0;
}

/**
 * @brief Prints given character at cursor location and increments the cursor.
 * 
 * @param c Character to print
 * @param autoflush If true, buffer will automatically flushed to LCD when finished.
 */
void lcd_print_char(const char c, bool autoflush) {
    // Check for newline
    if (c == '\n') {
        lcd_newline();
        return;
    }

    // Get actual index in font table
    uint8_t table_index = c - ' ';

    // Check if the index is out of range
    if (table_index < 0 || table_index > 57) {
        puts("(LCD) You are attempting to print an unsupported character.");
        _increment_cursor();
        return;
    }

    uint16_t x_pos = cursor_x_pos * 8;
    uint16_t y_pos = cursor_y_pos * 8;

    // Draw bitmap for this character from table
    lcd_draw_bitmap_8x8(_FONT_TABLE[table_index], x_pos, y_pos);

    _increment_cursor();

    if (autoflush) {
        flush_lcd_buffer();
    }
}

/**
 * @brief Prints given character array at cursor location and shifts the cursor
 * to after the string.
 * 
 * @param str Characters to print. MUST be null terminated. 
 * @param autoflush If true, buffer will automatically flushed to LCD when finished.
 */
uint16_t lcd_print_str(const char* str, bool autoflush) {
    int i = 0;
    while (*(str + i) != '\0') {
        lcd_print_char(*(str + i), false);
        i++;
    }

    if (autoflush) {
        flush_lcd_buffer();
    }

    return i+1;
}

/**
 * @brief Clears the line
 * 
 * @param line 
 * @return uint8_t 1 if line is > 5. Otherwise, 0.
 */
uint8_t lcd_clear_line(uint8_t line) {
    if (line > 5) {
        puts("(LCD) You are attempting to clear a line greater than the final index 5");
        return 1;
    }

    memset(display_buffer + (84 * line), 0, 84);

    return 0;
}