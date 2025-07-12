#ifndef VFS_H
#define VFS_H

#include "basedos.h"

// Forward declarations
typedef struct fs_node fs_node_t;

// Include system types
#include <sys/types.h>  // For dirent_t, etc.

// File mode and permission flags
#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

// File open flags
#define O_RDONLY 0x0000
#define O_WRONLY 0x0001
#define O_RDWR   0x0002
#define O_CREAT  0x0100
#define O_TRUNC  0x0200
#define O_APPEND 0x0400

// File seek origins
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

// File system node structure
typedef struct fs_node {
    char name[256];       // The filename.
    uint32_t mask;        // The permissions mask.
    uint32_t uid;         // The owning user.
    uint32_t gid;         // The owning group.
    uint32_t flags;       // Includes the node type.
    uint32_t inode;       // This is device-specific - provides a way for a filesystem to identify files.
    uint32_t length;      // Size of the file, in bytes.
    uint32_t impl;        // An implementation-defined number.
    
    // File operations
    uint32_t (*read)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
    uint32_t (*write)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
    void (*open)(struct fs_node*, uint32_t flags);
    void (*close)(struct fs_node*);
    dirent_t* (*readdir)(struct fs_node*, uint32_t);
    struct fs_node* (*finddir)(struct fs_node*, char* name);
    
    // File pointer for this node (used by the file system)
    void* fs_specific;
} fs_node_t;

// Directory entry structure is now defined in sys/types.h

// File descriptor
typedef struct {
    fs_node_t* node;   // The node this file descriptor points to
    uint32_t pos;      // Current position in the file
    uint32_t flags;    // Flags used when opening the file
} file_descriptor_t;

// File system operations
typedef struct {
    const char* name;
    fs_node_t* (*mount)(const char* device);
    void (*unmount)(fs_node_t* fs_root);
} filesystem_ops_t;

// Standard file operations
extern fs_node_t* fs_root; // The root of the filesystem.

// File system functions
void vfs_initialize(void);
fs_node_t* vfs_open(const char* filename, uint32_t flags);
void vfs_close(fs_node_t* node);
uint32_t vfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
uint32_t vfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
dirent_t* vfs_readdir(fs_node_t* node, uint32_t index);
fs_node_t* vfs_finddir(fs_node_t* node, char* name);

// File descriptor operations
int32_t open(const char* filename, uint32_t flags);
int32_t close(int32_t fd);
int32_t read(int32_t fd, void* buf, uint32_t size);
int32_t write(int32_t fd, const void* buf, uint32_t size);
int32_t lseek(int32_t fd, int32_t offset, int32_t whence);

// File system registration
void register_filesystem(filesystem_ops_t* fs_ops);

#endif // VFS_H
