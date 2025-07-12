#include "fs/vfs.h"
#include "string.h"

// Global root filesystem node
fs_node_t* fs_root = NULL;

// Maximum number of file descriptors
#define MAX_FILE_DESCRIPTORS 64
static file_descriptor_t file_descriptors[MAX_FILE_DESCRIPTORS];
static uint32_t next_fd = 0;

// List of registered filesystems
#define MAX_FILESYSTEMS 8
static filesystem_ops_t* filesystems[MAX_FILESYSTEMS];
static uint32_t num_filesystems = 0;

// Initialize the virtual file system
void vfs_initialize(void) {
    // Clear the file descriptors table
    memset(file_descriptors, 0, sizeof(file_descriptors));
    next_fd = 0;
    
    // Initialize the root filesystem
    fs_root = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    memset(fs_root, 0, sizeof(fs_node_t));
    
    // Create a simple root directory
    strcpy(fs_root->name, "root");
    fs_root->mask = 0x1FF; // rwxrwxrwx
    fs_root->uid = 0;      // root
    fs_root->gid = 0;      // root
    fs_root->flags = FS_DIRECTORY;
    fs_root->inode = 0;
    fs_root->length = 0;
    
    // Initialize filesystem list
    memset(filesystems, 0, sizeof(filesystems));
    num_filesystems = 0;
}

// Register a new filesystem
void register_filesystem(filesystem_ops_t* fs_ops) {
    if (num_filesystems >= MAX_FILESYSTEMS) {
        return; // No more space
    }
    
    filesystems[num_filesystems++] = fs_ops;
}

// Mount a filesystem
int32_t vfs_mount(const char* device, const char* mountpoint, const char* fs_type) {
    // Find the filesystem type
    filesystem_ops_t* fs_ops = NULL;
    for (uint32_t i = 0; i < num_filesystems; i++) {
        if (strcmp(filesystems[i]->name, fs_type) == 0) {
            fs_ops = filesystems[i];
            break;
        }
    }
    
    if (!fs_ops) {
        return -1; // Filesystem type not found
    }
    
    // Call the filesystem's mount function
    fs_node_t* fs_root = fs_ops->mount(device);
    if (!fs_root) {
        return -1; // Mount failed
    }
    
    // For now, we only support mounting at the root
    if (strcmp(mountpoint, "/") == 0) {
        // Copy the mounted filesystem's root to our global root
        if (fs_root) {
            memcpy(fs_root, fs_root, sizeof(fs_node_t));
            // Make sure the root node has the correct flags
            fs_root->flags |= FS_DIRECTORY;
        }
    }
    
    return 0; // Success
}

// Open a file
fs_node_t* vfs_open(const char* filename, uint32_t flags) {
    // Handle absolute paths
    if (filename[0] == '/') {
        // Start from the root
        fs_node_t* current = fs_root;
        
        // Skip the leading '/'
        const char* path = filename + 1;
        
        // If there's nothing after the '/', return the root
        if (*path == '\0') {
            return current;
        }
        
        // Split the path into components
        char component[256];
        while (*path) {
            // Extract the next component
            const char* slash = strchr(path, '/');
            size_t len = slash ? (size_t)(slash - path) : strlen(path);
            
            if (len >= sizeof(component)) {
                return NULL; // Component too long
            }
            
            strncpy(component, path, len);
            component[len] = '\0';
            
            // Find the component in the current directory
            fs_node_t* next = vfs_finddir(current, component);
            if (!next) {
                // If O_CREAT is set, create the file
                if ((flags & O_CREAT) && (!slash || *slash == '\0')) {
                    // TODO: Implement file creation
                    return NULL;
                }
                return NULL; // Not found
            }
            
            current = next;
            
            // Move to the next component
            path += len;
            if (*path == '/') {
                path++;
            }
        }
        
        return current;
    }
    
    // TODO: Handle relative paths
    return NULL;
}

// Close a file
void vfs_close(fs_node_t* node) {
    if (node->close) {
        node->close(node);
    }
}

// Read from a file
uint32_t vfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }
    return 0;
}

// Write to a file
uint32_t vfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->write) {
        return node->write(node, offset, size, buffer);
    }
    return 0;
}

// Read directory entry
dirent_t* vfs_readdir(fs_node_t* node, uint32_t index) {
    if (node->readdir != NULL) {
        return node->readdir(node, index);
    }
    return NULL;
}

// Find a directory entry
fs_node_t* vfs_finddir(fs_node_t* node, char* name) {
    if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir) {
        return node->finddir(node, name);
    }
    return NULL;
}

// Standard POSIX-like file operations

int32_t open(const char* filename, uint32_t flags) {
    // Find an available file descriptor
    int32_t fd = -1;
    for (uint32_t i = 0; i < MAX_FILE_DESCRIPTORS; i++) {
        if (file_descriptors[i].node == NULL) {
            fd = i;
            break;
        }
    }
    
    if (fd == -1) {
        return -1; // No available file descriptors
    }
    
    // Open the file
    fs_node_t* node = vfs_open(filename, flags);
    if (!node) {
        return -1; // Failed to open file
    }
    
    // Set up the file descriptor
    file_descriptors[fd].node = node;
    file_descriptors[fd].pos = 0;
    file_descriptors[fd].flags = flags;
    
    // Call the file's open function if it exists
    if (node->open) {
        node->open(node, flags);
    }
    
    return fd;
}

int32_t close(int32_t fd) {
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTORS || file_descriptors[fd].node == NULL) {
        return -1; // Invalid file descriptor
    }
    
    // Call the file's close function if it exists
    fs_node_t* node = file_descriptors[fd].node;
    if (node->close) {
        node->close(node);
    }
    
    // Clear the file descriptor
    file_descriptors[fd].node = NULL;
    file_descriptors[fd].pos = 0;
    file_descriptors[fd].flags = 0;
    
    return 0; // Success
}

int32_t read(int32_t fd, void* buf, uint32_t size) {
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTORS || file_descriptors[fd].node == NULL) {
        return -1; // Invalid file descriptor
    }
    
    file_descriptor_t* desc = &file_descriptors[fd];
    fs_node_t* node = desc->node;
    
    // Check if the file is opened for reading
    if ((desc->flags & O_RDONLY) == 0 && (desc->flags & O_RDWR) == 0) {
        return -1; // Not opened for reading
    }
    
    // Read from the file
    uint32_t bytes_read = vfs_read(node, desc->pos, size, (uint8_t*)buf);
    
    // Update the file position
    desc->pos += bytes_read;
    
    return bytes_read;
}

int32_t write(int32_t fd, const void* buf, uint32_t size) {
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTORS || file_descriptors[fd].node == NULL) {
        return -1; // Invalid file descriptor
    }
    
    file_descriptor_t* desc = &file_descriptors[fd];
    fs_node_t* node = desc->node;
    
    // Check if the file is opened for writing
    if ((desc->flags & O_WRONLY) == 0 && (desc->flags & O_RDWR) == 0) {
        return -1; // Not opened for writing
    }
    
    // Write to the file
    uint32_t bytes_written = vfs_write(node, desc->pos, size, (uint8_t*)buf);
    
    // Update the file position
    desc->pos += bytes_written;
    
    return bytes_written;
}

int32_t lseek(int32_t fd, int32_t offset, int32_t whence) {
    if (fd < 0 || fd >= MAX_FILE_DESCRIPTORS || file_descriptors[fd].node == NULL) {
        return -1; // Invalid file descriptor
    }
    
    file_descriptor_t* desc = &file_descriptors[fd];
    fs_node_t* node = desc->node;
    
    switch (whence) {
        case SEEK_SET:
            desc->pos = offset;
            break;
            
        case SEEK_CUR:
            desc->pos += offset;
            break;
            
        case SEEK_END:
            desc->pos = node->length + offset;
            break;
            
        default:
            return -1; // Invalid whence
    }
    
    return desc->pos;
}
