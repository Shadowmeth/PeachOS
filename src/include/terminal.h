#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

#define VGA_HEIGHT 20
#define VGA_WIDTH  80

enum color {
    COLOR_BLACK         = 0,
    COLOR_BLUE          = 1,
    COLOR_GREEN         = 2,
    COLOR_CYAN          = 3,
    COLOR_RED           = 4,
    COLOR_MAGENTA       = 5,
    COLOR_BROWN         = 6,
    COLOR_LIGHT_GRAY    = 7,
    COLOR_DARK_GRAY     = 8,
    COLOR_LIGHT_BLUE    = 9,
    COLOR_LIGHT_GREEN   = 10,
    COLOR_LIGHT_CYAN    = 11,
    COLOR_LIGHT_RED     = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_YELLOW        = 14,
    COLOR_WHITE         = 15,
};

void terminal_clear();
uint16_t terminal_make_char(char c, int8_t color);
void terminal_writechar(uint16_t x, uint16_t y, char c);
void terminal_putchar(uint16_t c);
void terminal_print(const char *str);

#endif
