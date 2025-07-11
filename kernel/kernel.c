#include "basedos.h"

void kmain(void) {
 disable_interrupts();
 terminal_initialize();
 printk("BasedOS Running\n");
 if (!start_shell()) {
 printk("Error: Failed to start shell\n");
 while (1) {
 asm volatile ("hlt");
 }
 }
 enable_interrupts();
 while (1) {
 asm volatile ("hlt");
 }
}