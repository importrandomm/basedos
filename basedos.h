#ifndef BASEDOS_H
#define BASEDOS_H

// Include our custom types
#include "sys/types.h"

// Standard definitions
#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef bool
typedef int bool;
#define true 1
#define false 0
#endif

// System includes
#include "io.h"
#include "interrupts.h"
#include "terminal.h"
#include "sound.h"

// File system includes
#include "fs/vfs.h"
#include "fs/memfs.h"

// Variadic argument definitions for freestanding environment
typedef char* va_list;
#define va_start(ap, last) ((ap) = (char*)&(last) + ((sizeof(last) + 3) & ~3))
#define va_arg(ap, type) (*(type*)((ap) += ((sizeof(type) + 3) & ~3), (ap) - ((sizeof(type) + 3) & ~3)))
#define va_end(ap) ((ap) = NULL)

// Video
extern uint16_t* video_memory; // Pointer to VGA text mode memory (e.g., 0xB8000)
extern int cursor_x, cursor_y; // Cursor position for terminal output

// Core I/O
void terminal_initialize(void); // Initialize VGA text mode
void terminal_clear(void); // Clear the terminal screen
void printk(const char* format, ...); // Print formatted string to terminal
void putchar(char c); // Print a single character
char keyboard_getchar(void); // Get a character from keyboard input

// System
void outb(uint16_t port, uint8_t value); // Write byte to I/O port
uint8_t inb(uint16_t port); // Read byte from I/O port
void init_interrupts(void); // Initialize interrupt descriptor table
void enable_interrupts(void); // Enable interrupts (STI)
void disable_interrupts(void); // Disable interrupts (CLI)

// Sound
void beep(uint32_t frequency); // Play a sound via PC speaker
void nosound(void); // Stop PC speaker sound

// Memory Management
void* kmalloc(size_t size); // Allocate memory from kernel heap
void kfree(void* ptr); // Free allocated memory
uint32_t detect_memory(void); // Detect available system memory

// String
size_t strlen(const char* str); // Calculate string length
char* strcpy(char* dest, const char* src); // Copy string
int strcmp(const char* s1, const char* s2); // Compare strings
int strncmp(const char* s1, const char* s2, size_t n); // Compare first n characters
char* strtok(char* str, const char* delim); // String tokenizer
size_t strspn(const char* str, const char* accept); // Get length of initial segment with characters in accept
size_t strcspn(const char* str, const char* reject); // Get length of initial segment without characters in reject
int atoi(const char* str); // Convert string to integer

// Shell
int start_shell(void); // Start the OS shell, returns 0 on failure

// Kernel
void kmain(void); // Kernel entry point
void kernel_shutdown(void); // Shutdown the kernel
uint32_t get_uptime(void); // Get system uptime in seconds
void get_memory_stats(uint32_t* total, uint32_t* free); // Get memory statistics

// Memory management
void* kmalloc(size_t size);
void kfree(void* ptr);

// File system functions
int32_t open(const char* filename, uint32_t flags);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, uint32_t size);
int32_t write(int32_t fd, const void* buf, uint32_t size);
int32_t lseek(int32_t fd, int32_t offset, int32_t whence);
int32_t vfs_mount(const char* device, const char* mountpoint, const char* fs_type);

// Interrupt handling
void register_interrupt_handler(uint8_t n, void (*handler)(void));

// Terminal enhancements
void terminal_writestring(const char* str); // Write a string to the terminal
void terminal_write_hex(uint32_t value); // Write a hexadecimal number to the terminal
void terminal_write_dec(uint32_t value); // Write a decimal number to the terminal
void terminal_setcolor(uint8_t fg, uint8_t bg); // Set terminal text and background colors

// VGA colors
#define VGA_COLOR_WHITE 15
#define VGA_COLOR_RED 4
#define VGA_COLOR_GREEN 2
#define VGA_COLOR_BLUE 1
#define VGA_COLOR_YELLOW 14

#endif