#include "basedos.h"
#include "string.h"

// Terminal state
uint16_t* video_memory = (uint16_t*)0xB8000;
int cursor_x = 0, cursor_y = 0;
static uint8_t text_attribute = 0x07;

static void update_cursor(void) {
    uint16_t pos = cursor_y * 80 + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_initialize(void) {
    // Clear screen
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            video_memory[y * 80 + x] = (text_attribute << 8) | ' ';
        }
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void terminal_clear(void) {
    terminal_initialize();
}

void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            video_memory[cursor_y * 80 + cursor_x] = (text_attribute << 8) | ' ';
        }
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else {
        video_memory[cursor_y * 80 + cursor_x] = (text_attribute << 8) | c;
        cursor_x++;
    }

    // Handle line wrap
    if (cursor_x >= 80) {
        cursor_x = 0;
        cursor_y++;
    }

    // Scroll if needed
    if (cursor_y >= 25) {
        // Move all lines up
        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 80; x++) {
                video_memory[y * 80 + x] = video_memory[(y + 1) * 80 + x];
            }
        }
        // Clear the last line
        for (int x = 0; x < 80; x++) {
            video_memory[24 * 80 + x] = (text_attribute << 8) | ' ';
        }
        cursor_y = 24;
    }

    update_cursor();
}

void terminal_writestring(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

void terminal_write_hex(uint32_t value) {
    char buffer[9];
    static const char* digits = "0123456789ABCDEF";
    
    buffer[8] = '\0';
    for (int i = 7; i >= 0; i--) {
        buffer[i] = digits[value & 0xF];
        value >>= 4;
    }
    
    terminal_writestring("0x");
    terminal_writestring(buffer);
}

void terminal_write_dec(uint32_t value) {
    char buffer[11]; // Maximum 10 digits for 32-bit number + null terminator
    char* p = buffer + sizeof(buffer) - 1;
    *p = '\0';
    
    if (value == 0) {
        *--p = '0';
    } else {
        while (value > 0) {
            *--p = '0' + (value % 10);
            value /= 10;
        }
    }
    
    terminal_writestring(p);
}

void terminal_setcolor(uint8_t fg, uint8_t bg) {
    text_attribute = (bg << 4) | (fg & 0x0F);
}

void printk(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    char buffer[32];
                    itoa(num, buffer, 10);
                    terminal_writestring(buffer);
                    break;
                }
                case 'u': {
                    unsigned int num = va_arg(args, unsigned int);
                    char buffer[32];
                    itoa(num, buffer, 10);
                    terminal_writestring(buffer);
                    break;
                }
                case 'x':
                case 'X': {
                    unsigned int num = va_arg(args, unsigned int);
                    char buffer[32];
                    itoa(num, buffer, 16);
                    terminal_writestring(buffer);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    putchar(c);
                    break;
                }
                case 's': {
                    const char* str = va_arg(args, const char*);
                    if (str == NULL) {
                        terminal_writestring("(null)");
                    } else {
                        terminal_writestring(str);
                    }
                    break;
                }
                case '%':
                    putchar('%');
                    break;
                default:
                    putchar('%');
                    putchar(*format);
                    break;
            }
        } else {
            putchar(*format);
        }
        format++;
    }
    
    va_end(args);
}
