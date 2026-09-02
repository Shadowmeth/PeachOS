#include "terminal.h"
#include "string.h"

static uint16_t *video_mem = (uint16_t *)0xb8000;

/* First nibble is background, second nibble is foreground */
static const int8_t default_color = COLOR_BLACK << 4 | COLOR_LIGHT_GRAY;
static const int8_t clear_color = 0;

static int row = 0;
static int col = 0;

void terminal_clear()
{
    for (int8_t y = 0; y < VGA_HEIGHT; ++y) {
        for (int8_t x = 0; x < VGA_WIDTH; ++x) {
            const uint16_t position = y * VGA_WIDTH + x;
            video_mem[position] = terminal_make_char(' ', clear_color);
        }
    }
}

uint16_t terminal_make_char(char c, int8_t color)
{
    return (color << 8) | c;
}

void terminal_writechar(uint16_t x, uint16_t y, char c)
{
    const uint16_t position = y * VGA_WIDTH + x;
    video_mem[position] = terminal_make_char(c, default_color);
}

void terminal_putchar(uint16_t c)
{
    if (c == '\n') {
        row++;
        col = 0;
        return;
    }

    if (col >= VGA_WIDTH) {
        row++;
        col = 0;
    }

    terminal_writechar(col, row, c);
    col++;
}

void terminal_print(const char *str)
{
    size_t len = strlen(str);
    for (size_t i = 0; i < len; ++i) {
        terminal_putchar(str[i]);
    }
}
