#ifndef MEMORY_H
#define MEMORY_H

#include <sys/types.h>

// Memory management functions
uint32_t detect_memory(void);
void* kmalloc(size_t size);
void kfree(void* ptr);

// Memory statistics
void get_memory_stats(uint32_t* total, uint32_t* free);

#endif // MEMORY_H
