#include "basedos.h"

// Simple IDT entry structure
struct idt_entry {
 uint16_t base_lo;
 uint16_t sel;
 uint8_t always0;
 uint8_t flags;
 uint16_t base_hi;
} __attribute__((packed));

// IDT descriptor
struct idt_ptr {
 uint16_t limit;
 uint32_t base;
} __attribute__((packed));

// IDT and keyboard buffer
static struct idt_entry idt[256];
static struct idt_ptr idtp;
static char key_buffer[256];
static int key_buffer_pos = 0;

// Scancode to ASCII mapping (simplified)
static const char scancode_to_ascii[] = {
 0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
 '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\r',
 0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// External handler for IRQ1
void keyboard_handler(void);

void init_interrupts(void) {
 // Set up IDT for IRQ1 (keyboard, interrupt 0x21)
 uint32_t handler_addr = (uint32_t)&keyboard_handler;
 idt[0x21].base_lo = handler_addr & 0xFFFF;
 idt[0x21].base_hi = (handler_addr >> 16) & 0xFFFF;
 idt[0x21].sel = 0x08; // Code segment from bootloader
 idt[0x21].always0 = 0;
 idt[0x21].flags = 0x8E; // Present, ring 0, interrupt gate

 // Remap PIC to avoid conflicts
 outb(0x20, 0x11); // ICW1: Initialize master PIC
 outb(0xA0, 0x11); // Initialize slave PIC
 outb(0x21, 0x20); // ICW2: Master offset 0x20
 outb(0xA1, 0x28); // Slave offset 0x28
 outb(0x21, 0x04); // ICW3: Master has slave on IR2
 outb(0xA1, 0x02); // Slave ID
 outb(0x21, 0x01); // ICW4: 8086 mode
 outb(0xA1, 0x01);
 outb(0x21, 0xFD); // Mask all except IRQ1 (keyboard)
 outb(0xA1, 0xFF); // Mask all slave IRQs

 // Load IDT
 idtp.limit = sizeof(idt) - 1;
 idtp.base = (uint32_t)&idt;
 asm volatile("lidt %0" : : "m"(idtp));
}

void enable_interrupts(void) {
 asm volatile("sti");
}

void disable_interrupts(void) {
 asm volatile("cli");
}

char keyboard_getchar(void) {
 while (key_buffer_pos == 0) {
 asm volatile("hlt");
 }
 char c = key_buffer[0];
 for (int i = 0; i < key_buffer_pos - 1; i++) {
 key_buffer[i] = key_buffer[i + 1];
 }
 key_buffer_pos--;
 return c;
}

// Keyboard handler (in assembly for proper interrupt handling)
asm (
 "keyboard_handler:\n"
 " pusha\n"
 " mov $0x20, %al\n"
 " out $0x20, %al\n" // Send EOI to PIC
 " in $0x60, %al\n" // Read scancode
 " mov %al, %bl \n"
 " and $0x7F, %ebx\n" // Ignore break codes
 " cmp $128, %bl\n"
 " jge .end_handler\n"
 " mov scancode_to_ascii(,%ebx,1), %al\n"
 " test %al, %al\n"
 " jz .end_handler\n"
 " mov key_buffer_pos, %edx\n"
 " cmp $256, %edx\n"
 " jge .end_handler\n"
 " movb %al, key_buffer(,%edx,1)\n"
 " inc %edx\n"
 " mov %edx, key_buffer_pos\n"
 ".end_handler:\n"
 " popa\n"
 " iret\n"
);