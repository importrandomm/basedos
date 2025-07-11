#include "basedos.h"

uint16_t* video_memory = (uint16_t*)0xB8000;
int cursor_x = 0, cursor_y = 0;
static uint8_t text_attribute = 0x07;

static void itoa(int value, char* buffer, int base);

static void update_cursor(void) {
 uint16_t pos = cursor_y * 80 + cursor_x;
 outb(0x3D4, 0x0F);
 outb(0x3D5, (uint8_t)(pos & 0xFF));
 outb(0x3D4, 0x0E);
 outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_initialize(void) {
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
 } else {
 video_memory[cursor_y * 80 + cursor_x] = (text_attribute << 8) | c;
 cursor_x ++;
 }
 if (cursor_x >= 80) {
 cursor_x = 0;
 cursor_y++;
 }
 if (cursor_y >= 25) {
 for (int y = 0; y < 24; y++) {
 for (int x = 0; x < 80; x++) {
 video_memory[y * 80 + x] = video_memory[(y + 1) * 80 + x];
 }
 }
 for (int x = 0; x < 80; x++) {
 video_memory[24 * 80 + x] = (text_attribute << 8) | ' ';
 }
 cursor_y = 24;
 }
 update_cursor();
}

void printk(const char* format, ...) {
 va_list args;
 va_start(args, format);
 char* s;
 int d;
 char buffer[16];

 while (*format) {
 if (*format == '%') {
 format++;
 switch (*format) {
 case 's':
 s = va_arg(args, char*);
 while (*s) {
 putchar(*s++);
 }
 break;
 case 'd':
 d = va_arg(args, int);
 itoa(d, buffer, 10);
 s = buffer;
 while (*s) {
 putchar(*s++);
 }
 break;
 case 'x':
 d = va_arg(args, int);
 itoa(d, buffer, 16);
 s = buffer;
 while (*s) {
 putchar(*s++);
 }
 break;
 default:
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

static void itoa(int value, char* buffer, int base) {
 int i = 0;
 int is_negative = 0;

 if (value == 0) {
 buffer[i++] = '0';
 buffer[i] = '\0';
 return;
 }

 if (value < 0 && base == 10) {
 is_negative = 1;
 value = -value;
 }

 while (value) {
 int digit = value % base;
 buffer[i++] = (digit < 10) ? '0' + digit : 'a' + digit - 10;
 value /= base;
 }

 if (is_negative) {
 buffer[i++] = '-';
 }

 buffer[i] = '\0';

 int start = 0, end = i - 1;
 while (start < end) {
 char temp = buffer[start];
 buffer[start] = buffer[end];
 buffer[end] = temp;
 start++;
 end--;
 }
}