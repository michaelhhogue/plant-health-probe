#ifndef LCD_H
#define LCD_H

#include "pico/stdlib.h"

void lcd_init(void);

void flush_lcd_buffer(void);

void clear_lcd(void);

void show_splashscreen(void);

uint8_t lcd_draw_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool fill);

uint8_t lcd_draw_bitmap_8x8(const uint8_t bitmap[8], uint8_t x, uint8_t y);

void lcd_newline(void);

int lcd_set_cursor(uint8_t x, uint8_t y);

void lcd_get_cursor_position(uint8_t* x, uint8_t* y);

void lcd_print_char(char c, bool autoflush);

uint16_t lcd_print_str(const char* str, bool autoflush);

uint8_t lcd_clear_line(uint8_t line);

#endif