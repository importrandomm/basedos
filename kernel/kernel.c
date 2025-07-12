#include "basedos.h"
#include "memory.h"

// Kernel subsystem status flags
static struct {
    bool interrupts_enabled;
    bool scheduler_active;
    bool memory_manager_ready;
    uint32_t uptime_seconds;
    uint32_t total_memory;
    uint32_t free_memory;
} kernel_status = {0};

// Simple task structure for basic multitasking
typedef struct task {
    uint32_t id;
    uint32_t esp;
    uint32_t ebp;
    uint32_t state; // 0=running, 1=ready, 2=blocked
    struct task* next;
} task_t;

static task_t* current_task = NULL;
static task_t* task_queue = NULL;
static uint32_t next_task_id = 1;

// Timer callback for uptime tracking
static void timer_callback(void) {
    static uint32_t ticks = 0;
    ticks++;
    if (ticks >= 100) { // Assuming 100Hz timer
        kernel_status.uptime_seconds++;
        ticks = 0;
    }
    
    // Simple round-robin scheduler
    if (kernel_status.scheduler_active && current_task && current_task->next) {
        // Save current task state (simplified)
        asm volatile("mov %%esp, %0" : "=m"(current_task->esp));
        asm volatile("mov %%ebp, %0" : "=m"(current_task->ebp));
        
        // Switch to next task
        current_task = current_task->next;
        
        // Restore next task state (simplified)
        asm volatile("mov %0, %%esp" : : "m"(current_task->esp));
        asm volatile("mov %0, %%ebp" : : "m"(current_task->ebp));
    }
}

// Initialize basic memory management
static void init_memory_manager(void) {
    // Simple memory initialization
    kernel_status.total_memory = detect_memory(); // Assume this function exists
    kernel_status.free_memory = kernel_status.total_memory - 0x100000; // Reserve 1MB for kernel
    kernel_status.memory_manager_ready = true;
}

// Initialize basic task scheduler
static void init_scheduler(void) {
    // Create main task
    current_task = (task_t*)kmalloc(sizeof(task_t));
    if (current_task) {
        current_task->id = next_task_id++;
        current_task->state = 0; // running
        current_task->next = current_task; // circular list
        task_queue = current_task;
        kernel_status.scheduler_active = true;
    }
}

// Enhanced interrupt initialization with timer
static void init_enhanced_interrupts(void) {
    init_interrupts();
    
    // Set up timer interrupt for scheduler and uptime
    register_interrupt_handler(0x20, timer_callback);
    
    // Initialize PIT (Programmable Interval Timer) for 100Hz
    outb(0x43, 0x36); // Command byte: channel 0, lobyte/hibyte, rate generator
    uint16_t divisor = 1193180 / 100; // 100Hz
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    
    kernel_status.interrupts_enabled = true;
}

// System information display
static void show_system_info(void) {
    terminal_writestring("BasedOS Enhanced Kernel\n");
    terminal_writestring("=======================\n");
    
    // Show memory info
    terminal_writestring("Memory: ");
    terminal_write_hex(kernel_status.total_memory);
    terminal_writestring(" bytes total, ");
    terminal_write_hex(kernel_status.free_memory);
    terminal_writestring(" bytes free\n");
    
    // Show uptime
    terminal_writestring("Uptime: ");
    terminal_write_dec(kernel_status.uptime_seconds);
    terminal_writestring(" seconds\n");
    
    // Show subsystem status
    terminal_writestring("Interrupts: ");
    terminal_writestring(kernel_status.interrupts_enabled ? "Enabled" : "Disabled");
    terminal_writestring("\nScheduler: ");
    terminal_writestring(kernel_status.scheduler_active ? "Active" : "Inactive");
    terminal_writestring("\nMemory Manager: ");
    terminal_writestring(kernel_status.memory_manager_ready ? "Ready" : "Not Ready");
    terminal_writestring("\n\n");
}

// Panic function for critical errors
void kernel_panic(const char* message) {
    disable_interrupts();
    terminal_setcolor(VGA_COLOR_WHITE, VGA_COLOR_RED);
    terminal_writestring("\n*** KERNEL PANIC ***\n");
    terminal_writestring(message);
    terminal_writestring("\nSystem halted.\n");
    
    // Log panic to memory or serial port if available
    // serial_writestring(message);
    
    while (1) {
        asm volatile("hlt");
    }
}

// Safe shutdown procedure
void kernel_shutdown(void) {
    terminal_writestring("Shutting down BasedOS...\n");
    
    // Disable scheduler
    kernel_status.scheduler_active = false;
    
    // Clean up resources
    if (task_queue) {
        // Free task structures (simplified)
        kfree(task_queue);
    }
    
    // Final cleanup
    disable_interrupts();
    terminal_writestring("System halted safely.\n");
    
    while (1) {
        asm volatile("hlt");
    }
}

// Enhanced kernel main function
void kmain(void) {
    // Initialize terminal
    terminal_initialize();
    
    // Initialize memory manager
    init_memory_manager();
    
    // Initialize interrupts
    init_enhanced_interrupts();
    
    // Initialize scheduler
    init_scheduler();
    
    // Initialize virtual file system
    vfs_initialize();
    
    // Initialize memory file system
    memfs_initialize();
    
    // Mount the root filesystem
    if (vfs_mount("memdisk", "/", "memfs") != 0) {
        printk("Failed to mount root filesystem!\n");
    }
    
    // Print welcome message
    printk("BasedOS Kernel v0.1\n");
    
    // Run file system test
    extern void fs_test(void);
    fs_test();
    
    // Start shell
    start_shell();
    
    // Halt if shell returns
    while (1) {
        asm volatile("hlt");
    }
}

// Additional utility functions for the enhanced kernel

// Get system uptime
uint32_t get_uptime(void) {
    return kernel_status.uptime_seconds;
}

// Get memory statistics
void get_memory_stats(uint32_t* total, uint32_t* free) {
    *total = kernel_status.total_memory;
    *free = kernel_status.free_memory;
}

// Check if subsystem is ready
bool is_subsystem_ready(const char* subsystem) {
    if (strcmp(subsystem, "interrupts") == 0) {
        return kernel_status.interrupts_enabled;
    } else if (strcmp(subsystem, "scheduler") == 0) {
        return kernel_status.scheduler_active;
    } else if (strcmp(subsystem, "memory") == 0) {
        return kernel_status.memory_manager_ready;
    }
    return false;
}

// Create a new task (simplified)
uint32_t create_task(void (*entry_point)(void)) {
    (void)entry_point; // Currently unused, will be used in a future implementation
    if (!kernel_status.scheduler_active) {
        return 0;
    }
    
    task_t* new_task = (task_t*)kmalloc(sizeof(task_t));
    if (!new_task) {
        return 0;
    }
    
    new_task->id = next_task_id++;
    new_task->state = 1; // ready
    
    // Set up stack (simplified)
    new_task->esp = (uint32_t)kmalloc(4096) + 4096; // 4KB stack
    new_task->ebp = new_task->esp;
    
    // Add to task queue
    if (task_queue) {
        new_task->next = task_queue->next;
        task_queue->next = new_task;
    } else {
        new_task->next = new_task;
        task_queue = new_task;
    }
    
    return new_task->id;
}

// Yield CPU to next task
void yield(void) {
    if (kernel_status.scheduler_active) {
        asm volatile("int $0x20"); // Trigger timer interrupt
    }
}