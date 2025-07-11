#include "basedos.h"

#define HEAP_START 0x10000
#define HEAP_SIZE 0x10000

static uint8_t heap[HEAP_SIZE];
static uint32_t heap_pos = 0;

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