#include "basedos.h"

void kmain(void) {
    // Disable interrupts during initialization
    disable_interrupts();
    
    // Initialize terminal
    terminal_initialize();
    
    // Print startup message
    printk("BasedOS v0.1 Loading...\n");
    printk("Initializing system...\n");
    
    // Initialize interrupt system
    init_interrupts();
    
    // Print ready message
    printk("System initialized successfully!\n");
    printk("Starting shell...\n");
    
    // Enable interrupts
    enable_interrupts();
    
    // Start the shell - this should never return
    if (!start_shell()) {
        printk("FATAL ERROR: Shell failed to start!\n");
        // If shell fails, halt the system
        disable_interrupts();
        while (1) {
            asm volatile("hlt");
        }
    }
    
    // If we somehow get here, halt
    printk("FATAL ERROR: Shell returned unexpectedly!\n");
    disable_interrupts();
    while (1) {
        asm volatile("hlt");
    }
}