#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <sys/types.h>

// Initialize the interrupt descriptor table (IDT)
void init_interrupts(void);

// Enable/disable interrupts
void enable_interrupts(void);
void disable_interrupts(void);

// Register an interrupt handler for a specific interrupt number
typedef void (*interrupt_handler_t)(void);
void register_interrupt_handler(uint8_t interrupt_number, interrupt_handler_t handler);

// Default interrupt handler (for unhandled interrupts)
void default_interrupt_handler(void);

#endif // INTERRUPTS_H
