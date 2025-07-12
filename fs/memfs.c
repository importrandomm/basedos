#include "fs/vfs.h"
#include <string.h>
#include <sys/types.h>

#define MAX_FILES 64
#define MAX_BLOCKS 1024
#define BLOCK_SIZE 4096

typedef struct memfs_inode {
    char* name;
    uint32_t size;
    uint8_t* data;
    uint32_t is_dir;
    struct memfs_inode* parent;
    struct memfs_inode* children;
    struct memfs_inode* next;
} memfs_inode_t;

static memfs_inode_t* root_node = NULL;
static memfs_inode_t* nodes = NULL;
static uint8_t* block_bitmap = NULL;
static uint8_t* blocks = NULL;

// Forward declarations
static uint32_t memfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
static uint32_t memfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
static void memfs_open(fs_node_t* node, uint32_t flags);
static void memfs_close(fs_node_t* node);
static dirent_t* memfs_readdir(fs_node_t* node, uint32_t index);
static fs_node_t* memfs_finddir(fs_node_t* node, char* name);

// File system operations
static filesystem_ops_t memfs_ops = {
    .name = "memfs",
    .mount = memfs_mount,
    .unmount = NULL
};

// Initialize the memory file system
void memfs_initialize(void) {
    // Allocate memory for nodes
    nodes = (memfs_inode_t*)kmalloc(MAX_FILES * sizeof(memfs_inode_t));
    memset(nodes, 0, MAX_FILES * sizeof(memfs_inode_t));
    
    // Allocate block bitmap (1 bit per block)
    block_bitmap = (uint8_t*)kmalloc(MAX_BLOCKS / 8);
    memset(block_bitmap, 0, MAX_BLOCKS / 8);
    
    // Allocate blocks
    blocks = (uint8_t*)kmalloc(MAX_BLOCKS * BLOCK_SIZE);
    
    // Create root directory
    root_node = &nodes[0];
    root_node->name = "/";
    root_node->is_dir = 1;
    root_node->parent = NULL;
    root_node->children = NULL;
    root_node->next = NULL;
    
    // Register the file system
    register_filesystem(&memfs_ops);
}

// Allocate a new inode (used internally)
#if 0
static memfs_inode_t* memfs_alloc_inode(const char* name, uint32_t is_dir) {
    static uint32_t inode_counter = 1;
    memfs_inode_t* inode = (memfs_inode_t*)kmalloc(sizeof(memfs_inode_t));
    if (!inode) return NULL;
    
    memset(inode, 0, sizeof(memfs_inode_t));
    inode->name = (char*)kmalloc(strlen(name) + 1);
    if (!inode->name) {
        kfree(inode);
        return NULL;
    }
    strcpy(inode->name, name);
    inode->is_dir = is_dir;
    inode->inode = inode_counter++;
    
    return inode;
}
#else
// Temporary implementation that just returns NULL
static memfs_inode_t* memfs_alloc_inode(const char* name, uint32_t is_dir) {
    (void)name;     // Unused parameter
    (void)is_dir;   // Unused parameter
    return NULL;
}
#endif

// Suppress unused function warning
static void __attribute__((unused)) suppress_unused_warning() {
    (void)memfs_alloc_inode(NULL, 0);
}

// Mount the memory file system
fs_node_t* memfs_mount(const char* device) {
    (void)device; // Unused parameter
    fs_node_t* root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    if (!root) return NULL;
    
    memset(root, 0, sizeof(fs_node_t));
    
    // Initialize the root directory
    strncpy(root->name, "root", sizeof(root->name) - 1);
    root->mask = 0x1FF; // rwxrwxrwx
    root->uid = 0;      // root
    root->gid = 0;      // root
    root->flags = FS_DIRECTORY;
    root->inode = 0;
    root->length = 0;
    
    // Set up the operations
    root->read = memfs_read;
    root->write = memfs_write;
    root->open = memfs_open;
    root->close = memfs_close;
    root->readdir = memfs_readdir;
    root->finddir = memfs_finddir;
    
    root->fs_specific = root_node;
    
    return root;
}

// Read from a file
static uint32_t memfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    memfs_inode_t* inode = (memfs_inode_t*)node->fs_specific;
    
    if (offset >= inode->size) {
        return 0;
    }
    
    if (offset + size > inode->size) {
        size = inode->size - offset;
    }
    
    if (inode->data) {
        memcpy(buffer, inode->data + offset, size);
    }
    
    return size;
}

// Write to a file
static uint32_t memfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    memfs_inode_t* inode = (memfs_inode_t*)node->fs_specific;
    
    // If this is a directory, we can't write to it
    if (inode->is_dir) {
        return 0;
    }
    
    // If we need more space, allocate it
    if (offset + size > inode->size) {
        // TODO: Implement block allocation
        uint8_t* new_data = (uint8_t*)kmalloc(offset + size);
        if (inode->data) {
            memcpy(new_data, inode->data, inode->size);
            kfree(inode->data);
        }
        inode->data = new_data;
        inode->size = offset + size;
    }
    
    // Write the data
    memcpy(inode->data + offset, buffer, size);
    return size;
}

// Open a file
static void memfs_open(fs_node_t* node, uint32_t flags) {
    (void)node;   // Unused parameter
    (void)flags;  // Unused parameter
    // Implementation can be added later
}

// Close a file
static void memfs_close(fs_node_t* node) {
    (void)node;  // Unused parameter - suppress warning
    // Nothing special to do here for now
}

// Read directory entry
static dirent_t* memfs_readdir(fs_node_t* node, uint32_t index) {
    memfs_inode_t* inode = (memfs_inode_t*)node->fs_specific;
    static dirent_t dir_entry;
    
    if (!inode->is_dir) {
        return NULL;
    }
    
    memfs_inode_t* child = inode->children;
    for (uint32_t i = 0; i < index && child; i++) {
        child = child->next;
    }
    
    if (child) {
        strcpy(dir_entry.d_name, child->name);
        dir_entry.d_ino = (uint32_t)child;
        return &dir_entry;
    }
    
    return NULL;
}

// Find a directory entry
static fs_node_t* memfs_finddir(fs_node_t* node, char* name) {
    memfs_inode_t* inode = (memfs_inode_t*)node->fs_specific;
    
    if (!inode->is_dir) {
        return NULL;
    }
    
    for (memfs_inode_t* child = inode->children; child; child = child->next) {
        if (strcmp(child->name, name) == 0) {
            fs_node_t* fs_node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
            memset(fs_node, 0, sizeof(fs_node_t));
            
            strcpy(fs_node->name, child->name);
            fs_node->mask = 0; // TODO: Set proper permissions
            fs_node->uid = 0;
            fs_node->gid = 0;
            fs_node->flags = child->is_dir ? FS_DIRECTORY : FS_FILE;
            fs_node->inode = (uint32_t)child;
            fs_node->length = child->size;
            fs_node->read = memfs_read;
            fs_node->write = memfs_write;
            fs_node->open = memfs_open;
            fs_node->close = memfs_close;
            fs_node->readdir = child->is_dir ? memfs_readdir : NULL;
            fs_node->finddir = child->is_dir ? memfs_finddir : NULL;
            fs_node->fs_specific = child;
            
            return fs_node;
        }
    }
    
    return NULL;
}
