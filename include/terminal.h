#ifndef TERMINAL_H
#define TERMINAL_H

#include <sys/types.h>

// Terminal colors
#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGHT_GREEN 10
#define COLOR_LIGHT_CYAN 11
#define COLOR_LIGHT_RED 12
#define COLOR_LIGHT_MAGENTA 13
#define COLOR_YELLOW 14
#define COLOR_WHITE 15

// Terminal functions
void terminal_initialize(void);
void terminal_clear(void);
void terminal_setcolor(uint8_t fg, uint8_t bg);
void terminal_writestring(const char* str);
void terminal_write_hex(uint32_t value);
void terminal_write_dec(uint32_t value);
void putchar(char c);
void printk(const char* format, ...);

// Global variables (declared as extern)
extern uint16_t* video_memory;
extern int cursor_x, cursor_y;

#endif // TERMINAL_H
