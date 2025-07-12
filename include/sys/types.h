#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

// Standard integer types
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
typedef unsigned int        uint32_t;
typedef unsigned long long  uint64_t;

typedef signed char         int8_t;
typedef short               int16_t;
typedef int                 int32_t;
typedef long long           int64_t;

typedef unsigned int        size_t;
typedef int                 ssize_t;
typedef int                 pid_t;
typedef int                 uid_t;
typedef int                 gid_t;
typedef int                 mode_t;
typedef int                 off_t;

// File descriptor type
typedef int                 fd_t;

// For time-related functions
typedef uint32_t            time_t;
typedef uint32_t            clock_t;

// For directory operations
typedef struct {
    uint32_t d_ino;         // Inode number
    char     d_name[256];   // Filename
} dirent_t;

// For file status
struct stat {
    uint32_t st_dev;        // ID of device containing file
    uint32_t st_ino;        // Inode number
    uint32_t st_mode;       // File type and mode
    uint32_t st_nlink;      // Number of hard links
    uint32_t st_uid;        // User ID of owner
    uint32_t st_gid;        // Group ID of owner
    uint32_t st_rdev;       // Device ID (if special file)
    off_t    st_size;       // Total size, in bytes
    uint32_t st_blksize;    // Block size for filesystem I/O
    uint32_t st_blocks;     // Number of 512B blocks allocated
    time_t   st_atime;      // Time of last access
    time_t   st_mtime;      // Time of last modification
    time_t   st_ctime;      // Time of last status change
};

// File types for st_mode
#define S_IFMT   0170000   // Bit mask for the file type bit field
#define S_IFSOCK 0140000   // Socket
#define S_IFLNK  0120000   // Symbolic link
#define S_IFREG  0100000   // Regular file
#define S_IFBLK  0060000   // Block device
#define S_IFDIR  0040000   // Directory
#define S_IFCHR  0020000   // Character device
#define S_IFIFO  0010000   // FIFO

// File mode bits for st_mode
#define S_ISUID  0004000   // Set-user-ID bit
#define S_ISGID  0002000   // Set-group-ID bit
#define S_ISVTX  0001000   // Sticky bit

// File mode bits for user
#define S_IRWXU  00700     // Mask for file owner permissions
#define S_IRUSR  00400     // Owner has read permission
#define S_IWUSR  00200     // Owner has write permission
#define S_IXUSR  00100     // Owner has execute permission

// File mode bits for group
#define S_IRWXG  00070     // Mask for group permissions
#define S_IRGRP  00040     // Group has read permission
#define S_IWGRP  00020     // Group has write permission
#define S_IXGRP  00010     // Group has execute permission

// File mode bits for others
#define S_IRWXO  00007     // Mask for permissions for others
#define S_IROTH  00004     // Others have read permission
#define S_IWOTH  00002     // Others have write permission
#define S_IXOTH  00001     // Others have execute permission

// Helper macros for checking file types
#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)  // Regular file
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)  // Directory
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)  // Character device
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)  // Block device
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)  // FIFO/pipe
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)  // Symbolic link
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK) // Socket

#endif // _SYS_TYPES_H
