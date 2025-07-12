#ifndef MEMFS_H
#define MEMFS_H

#include "fs/vfs.h"

// Forward declaration for fs_node_t
typedef struct fs_node fs_node_t;

// Initialize the memory file system
void memfs_initialize(void);

// Mount a memory file system
fs_node_t* memfs_mount(const char* device);

#endif // MEMFS_H
