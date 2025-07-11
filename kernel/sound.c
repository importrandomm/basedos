#include "basedos.h"

void beep(uint32_t frequency) {
 if (frequency == 0) return;
 uint32_t divisor = 1193180 / frequency;
 outb(0x43, 0xB6);
 outb(0x42, (uint8_t)(divisor & 0xFF));
 outb(0x42, (uint8_t)((divisor >> 8) & 0xFF));
 uint8_t tmp = inb(0x61);
 outb(0x61, tmp | 0x03);
}

void nosound(void) {
 uint8_t tmp = inb(0x61);
 outb(0x61, tmp & 0xFC);
}