#ifndef BASEDOS_H
#define BASEDOS_H

// Type definitions
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int size_t;
typedef int bool;

#define true 1
#define false 0
#define NULL ((void*)0)

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

// String
size_t strlen(const char* str); // Calculate string length
char* strcpy(char* dest, const char* src); // Copy string
int strcmp(const char* s1, const char* s2); // Compare strings

// Shell
int start_shell(void); // Start the OS shell, returns 0 on failure

// Kernel
void kmain(void); // Kernel entry point

#endif