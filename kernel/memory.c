#include "basedos.h"

#define HEAP_START 0x10000
#define HEAP_SIZE 0x10000

static uint8_t heap[HEAP_SIZE];
static uint32_t heap_pos = 0;
static uint32_t total_memory = 0;

// Memory map entry structure would be defined here in a full implementation

// Detect available memory using BIOS E820 function
uint32_t detect_memory(void) {
    // This is a simplified version - in a real OS, you'd use BIOS interrupts
    // For now, we'll just return a fixed value (64MB) for demonstration
    // In a real implementation, you'd parse the memory map from BIOS
    
    total_memory = 64 * 1024 * 1024; // 64MB
    return total_memory;
}

// Memory statistics are now handled in kernel.c

void* kmalloc(size_t size) {
 if (heap_pos + size > HEAP_SIZE) {
 return NULL;
 }
 void* ptr = &heap[heap_pos];
 heap_pos += (size + 3) & ~3; // Align to 4 bytes
 return ptr;
}

void kfree(void* ptr) {
 // Simple heap: no actual freeing, just reset if at end
 if (ptr == &heap[heap_pos - 1]) {
 heap_pos = (uint32_t)((uint8_t*)ptr - heap);
 }
}