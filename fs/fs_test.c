#include "fs/vfs.h"
#include "fs/memfs.h"
#include "string.h"
#include "basedos.h"
#include "sys/types.h"

// Forward declarations
typedef struct fs_node fs_node_t;

void fs_test(void) {
    printk("=== Starting File System Test ===\n");
    
    // Initialize VFS and memory filesystem
    vfs_initialize();
    memfs_initialize();
    
    // Mount the memory filesystem
    if (vfs_mount("memdisk", "/", "memfs") != 0) {
        printk("Failed to mount root filesystem!\n");
        return;
    }
    
    printk("Mounted memory filesystem at /\n");
    
    // Test file creation and writing
    printk("Creating test file...\n");
    int fd = open("/test.txt", O_WRONLY | O_CREAT);
    if (fd < 0) {
        printk("Failed to create test file\n");
        return;
    }
    
    const char* test_data = "Hello, BasedOS File System!";
    int len = strlen(test_data);
    int written = write(fd, test_data, len);
    
    if (written != len) {
        printk("Failed to write to test file\n");
        close(fd);
        return;
    }
    
    close(fd);
    printk("Wrote %d bytes to /test.txt\n", written);
    
    // Test reading the file back
    printk("Reading back test file...\n");
    fd = open("/test.txt", O_RDONLY);
    if (fd < 0) {
        printk("Failed to open test file for reading\n");
        return;
    }
    
    char buffer[128];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    
    if (bytes_read <= 0) {
        printk("Failed to read from test file\n");
        close(fd);
        return;
    }
    
    buffer[bytes_read] = '\0';
    close(fd);
    
    printk("Read %d bytes: %s\n", bytes_read, buffer);
    
    // Test directory listing
    printk("Root directory contents:\n");
    fs_node_t* dir = vfs_open("/", 0);
    if (!dir) {
        printk("Failed to open root directory\n");
        return;
    }
    
    dirent_t* entry;
    uint32_t i = 0;
    while ((entry = vfs_readdir(dir, i++)) != NULL) {
        if (entry && entry->d_name[0] != '\0') {
            printk("  %s\n", entry->d_name);
        }
    }
    
    vfs_close(dir);
    
    printk("=== File System Test Complete ===\n");
}
