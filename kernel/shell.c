#include "basedos.h"
#include "string.h"
#include "fs/vfs.h"
#include "sound.h"
#include "io.h"

// External VFS root
extern fs_node_t* fs_root;

#define MAX_INPUT 128
#define PROMPT "user@based:~$ "
#define HISTORY_SIZE 10
#define MAX_ARGS 16
#define MAX_ALIASES 20

// Command history
static char history[HISTORY_SIZE][MAX_INPUT];
static int history_pos = 0;
static int current_history = -1;

// Command aliases
typedef struct {
    char alias[32];
    char command[MAX_INPUT];
} alias_t;

static alias_t aliases[MAX_ALIASES];
static int alias_count = 0;

// Shell state
static struct {
    bool echo_enabled;
    bool debug_mode;
    char current_directory[64];
    int last_exit_code;
    bool color_enabled;
} shell_state = {
    .echo_enabled = true,
    .debug_mode = false,
    .current_directory = "/",
    .last_exit_code = 0,
    .color_enabled = true
};

// Enhanced delay function
void delay(int milliseconds) {
    for (int i = 0; i < milliseconds * 1000; i++) {
        asm volatile("nop");
    }
}

// Color printing functions
static void print_color(const char* text, uint8_t color) {
    if (shell_state.color_enabled) {
        terminal_setcolor(color, 0);
        printk("%s", text);
        terminal_setcolor(15, 0); // Reset to white
    } else {
        printk("%s", text);
    }
}

static void print_error(const char* text) {
    print_color(text, 12); // Red
}

static void print_success(const char* text) {
    print_color(text, 10); // Green
}

static void print_info(const char* text) {
    print_color(text, 11); // Cyan
}

static void print_warning(const char* text) {
    print_color(text, 14); // Yellow
}

// Command parsing
static int parse_command(const char* input, char* args[]) {
    static char buffer[MAX_INPUT];
    strcpy(buffer, input);
    
    int argc = 0;
    char* token = strtok(buffer, " \t");
    
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    
    args[argc] = NULL;
    return argc;
}

// Alias management
static void add_alias(const char* alias_name, const char* command) {
    if (alias_count >= MAX_ALIASES) {
        print_error("Maximum aliases reached\n");
        return;
    }
    
    // Check if alias already exists
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, alias_name) == 0) {
            strcpy(aliases[i].command, command);
            print_success("Alias updated\n");
            return;
        }
    }
    
    strcpy(aliases[alias_count].alias, alias_name);
    strcpy(aliases[alias_count].command, command);
    alias_count++;
    print_success("Alias added\n");
}

static const char* resolve_alias(const char* cmd) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, cmd) == 0) {
            return aliases[i].command;
        }
    }
    return cmd;
}

static void list_aliases(void) {
    if (alias_count == 0) {
        print_info("No aliases defined\n");
        return;
    }
    
    print_info("Defined aliases:\n");
    for (int i = 0; i < alias_count; i++) {
        printk("  %s -> %s\n", aliases[i].alias, aliases[i].command);
    }
}

// History management
static void add_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;
    if (history_pos > 0 && strcmp(history[history_pos - 1], cmd) == 0) {
        return;
    }
    strcpy(history[history_pos], cmd);
    history_pos = (history_pos + 1) % HISTORY_SIZE;
}

static void show_history(void) {
    print_info("Command history:\n");
    for (int i = 0; i < HISTORY_SIZE; i++) {
        int idx = (history_pos + i) % HISTORY_SIZE;
        if (history[idx][0] != '\0') {
            printk(" %d: %s\n", i + 1, history[idx]);
        }
    }
}

// System information commands
static void show_system_info(void) {
    print_info("BasedOS System Information\n");
    print_info("==========================\n");
    
    printk("OS Version: BasedOS v0.1\n");
    printk("Architecture: x86 32-bit\n");
    printk("Uptime: %d seconds\n", get_uptime());
    
    uint32_t total_mem, free_mem;
    get_memory_stats(&total_mem, &free_mem);
    printk("Memory: %d KB total, %d KB free\n", total_mem / 1024, free_mem / 1024);
    
    printk("Current directory: %s\n", shell_state.current_directory);
    printk("Debug mode: %s\n", shell_state.debug_mode ? "ON" : "OFF");
    printk("Color mode: %s\n", shell_state.color_enabled ? "ON" : "OFF");
    printk("Last exit code: %d\n", shell_state.last_exit_code);
}

static void show_memory_info(void) {
    uint32_t total_mem, free_mem;
    get_memory_stats(&total_mem, &free_mem);
    
    print_info("Memory Information\n");
    print_info("==================\n");
    printk("Total memory: %d KB (%d bytes)\n", total_mem / 1024, total_mem);
    printk("Free memory: %d KB (%d bytes)\n", free_mem / 1024, free_mem);
    printk("Used memory: %d KB (%d bytes)\n", 
           (total_mem - free_mem) / 1024, total_mem - free_mem);
    
    int usage_percent = ((total_mem - free_mem) * 100) / total_mem;
    printk("Memory usage: %d%%\n", usage_percent);
    
    if (usage_percent > 80) {
        print_warning("Warning: High memory usage!\n");
    }
}

// Fun commands
static void display_banner(void) {
    print_info("  ____                   _  ___  ____\n");
    print_info(" | __ )  __ _ ___  ___  __| |/ _ \\/ ___|\n");
    print_info(" |  _ \\ / _` / __|/ _ \\/ _` | | | \\___ \\\n");
    print_info(" | |_) | (_| \\__ \\  __/ (_| | |_| |___) |\n");
    print_info(" |____/ \\__,_|___/\\___|\\__,_|\\___/|____/\n");
    printk("\n");
}

void play_startup_sound(void) {
    // Play a nice startup melody
    int notes[] = {523, 587, 659, 698, 784, 880, 988, 1047}; // C major scale
    for (int i = 0; i < 8; i++) {
        beep(notes[i]);
        delay(200);
        nosound();
        delay(50);
    }
}

static void echo_test(int argc, char* args[]) {
    if (argc < 2) {
        printk("Usage: echo <text>\n");
        return;
    }
    
    for (int i = 1; i < argc; i++) {
        printk("%s", args[i]);
        if (i < argc - 1) printk(" ");
    }
    printk("\n");
}

// Calculator function
static void calculator(int argc, char* args[]) {
    if (argc != 4) {
        printk("Usage: calc <num1> <operator> <num2>\n");
        printk("Operators: +, -, *, /, %\n");
        return;
    }
    
    int a = atoi(args[1]);
    int b = atoi(args[3]);
    char op = args[2][0];
    int result = 0;
    bool valid = true;
    
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': 
            if (b == 0) {
                print_error("Error: Division by zero\n");
                return;
            }
            result = a / b; 
            break;
        case '%': 
            if (b == 0) {
                print_error("Error: Division by zero\n");
                return;
            }
            result = a % b; 
            break;
        default:
            print_error("Error: Unknown operator\n");
            valid = false;
    }
    
    if (valid) {
        printk("%d %c %d = %d\n", a, op, b, result);
    }
}

// Enhanced command execution
static void execute_command(const char* input) {
    if (strlen(input) == 0) return;
    
    add_to_history(input);
    
    char* args[MAX_ARGS];
    int argc = parse_command(input, args);
    
    if (argc == 0) return;
    
    // Resolve aliases
    const char* resolved_cmd = resolve_alias(args[0]);
    if (resolved_cmd != args[0]) {
        // Re-parse if alias was resolved
        argc = parse_command(resolved_cmd, args);
    }
    
    shell_state.last_exit_code = 0;
    
    // Command execution
    if (strcmp(args[0], "help") == 0) {
        print_info("Available commands:\n");
        printk("  help          - Show this help\n");
        printk("  clear         - Clear screen\n");
        printk("  echo          - Print arguments to the screen\n");
        printk("  exit          - Exit the shell\n");
        printk("  shutdown      - Shut down the system\n");
        printk("  reboot        - Reboot the system\n");
        printk("  beep          - Play a beep sound\n");
        printk("  memory        - Show memory information\n");
        printk("  uptime        - Show system uptime\n");
        printk("  ls            - List directory contents\n");
        printk("  cd <dir>      - Change directory\n");
        printk("  pwd           - Print working directory\n");
        printk("  cat <file>    - Display file contents\n");
        printk("  mkdir <dir>   - Create a directory\n");
        printk("  touch <file>  - Create an empty file\n");
        printk("  rm <file>     - Remove a file or directory\n");
        printk("  write <file>  - Write text to a file\n");
        printk("  date          - Show system uptime\n");
        printk("  banner        - Show ASCII banner\n");
        printk("  alias         - Manage command aliases\n");
        printk("  set           - Change shell settings\n");
        printk("  history       - Show command history\n");
        printk("  sysinfo       - Show system information\n");
        printk("  calc          - Simple calculator\n");
        printk("  sound         - Play startup sound\n");
        
    } else if (strcmp(args[0], "clear") == 0) {
        terminal_clear();
        
    } else if (strcmp(args[0], "echo") == 0) {
        echo_test(argc, args);
        
    } else if (strcmp(args[0], "exit") == 0) {
        print_info("Goodbye!\n");
        return;
        
    } else if (strcmp(args[0], "shutdown") == 0) {
        print_info("Shutting down system...\n");
        kernel_shutdown();
        
    } else if (strcmp(args[0], "reboot") == 0) {
        print_warning("Rebooting system...\n");
        // Trigger keyboard controller reset
        outb(0x64, 0xFE);
        while (1) asm volatile("hlt");
        
    } else if (strcmp(args[0], "pwd") == 0) {
        printk("%s\n", shell_state.current_directory);
        
    } else if (strcmp(args[0], "cd") == 0) {
        const char* path = "/";  // Default to root
        if (argc > 1) {
            path = args[1];
        }
        
        // Cast away const for compatibility with VFS function signature
        fs_node_t* node = vfs_finddir(fs_root, (char*)path);
        if (node && (node->flags & FS_DIRECTORY)) {
            strncpy(shell_state.current_directory, path, sizeof(shell_state.current_directory) - 1);
            shell_state.current_directory[sizeof(shell_state.current_directory) - 1] = '\0';
        } else {
            print_error("No such directory: ");
            printk("%s\n", path);
            shell_state.last_exit_code = 1;
        }
        
    } else if (strcmp(args[0], "beep") == 0) {
        beep(1000);
        delay(500);
        nosound();
        
    } else if (strcmp(args[0], "memory") == 0) {
        show_memory_info();
        
    } else if (strcmp(args[0], "uptime") == 0) {
        printk("System uptime: %d seconds\n", get_uptime());
        
    } else if (strcmp(args[0], "ls") == 0) {
        fs_node_t* dir = vfs_open("/", 0);
        if (!dir) {
            printk("Error: Could not open directory /\n");
            return;
        }
        
        if ((dir->flags & 0x7) != FS_DIRECTORY) {
            printk("Error: / is not a directory\n");
            vfs_close(dir);
            return;
        }
        
        printk("Contents of /\n");
        
        dirent_t* entry;
        int i = 0;
        while ((entry = vfs_readdir(dir, i++)) != NULL) {
            printk("  %s\n", entry->d_name);
        }
        
        vfs_close(dir);
        
    } else if (strcmp(args[0], "cat") == 0) {
        if (argc < 2) {
            printk("Usage: cat <file>\n");
            return;
        }
        
        int fd = open(args[1], O_RDONLY);
        if (fd < 0) {
            printk("Error: Could not open file %s\n", args[1]);
            return;
        }
        
        char buffer[1024];
        int bytes_read;
        
        while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            printk("%s", buffer);
        }
        
        close(fd);
        printk("\n");
        
    } else if (strcmp(args[0], "mkdir") == 0) {
        if (argc < 2) {
            printk("Usage: mkdir <directory>\n");
            return;
        }
        
        // TODO: Implement directory creation
        printk("mkdir: Not implemented yet\n");
        
    } else if (strcmp(args[0], "touch") == 0) {
        if (argc < 2) {
            printk("Usage: touch <file>\n");
            return;
        }
        
        int fd = open(args[1], O_CREAT | O_WRONLY);
        if (fd < 0) {
            printk("Error: Could not create file %s\n", args[1]);
            return;
        }
        
        close(fd);
        printk("Created file %s\n", args[1]);
        
    } else if (strcmp(args[0], "rm") == 0) {
        if (argc < 2) {
            printk("Usage: rm <file>\n");
            return;
        }
        
        // TODO: Implement file/directory removal
        printk("rm: Not implemented yet\n");
        
    } else if (strcmp(args[0], "write") == 0) {
        if (argc < 3) {
            printk("Usage: write <file> <text>\n");
            return;
        }
        
        int fd = open(args[1], O_WRONLY | O_CREAT);
        if (fd < 0) {
            printk("Error: Could not open file %s\n", args[1]);
            return;
        }
        
        // Combine all arguments after the filename
        char buffer[1024] = {0};
        for (int i = 2; i < argc; i++) {
            strcat(buffer, args[i]);
            if (i < argc - 1) {
                strcat(buffer, " ");
            }
        }
        
        int len = strlen(buffer);
        int written = write(fd, buffer, len);
        
        if (written != len) {
            printk("Error: Could not write to file\n");
        } else {
            printk("Wrote %d bytes to %s\n", written, args[1]);
        }
        
        close(fd);
        
    } else if (strcmp(args[0], "date") == 0) {
        printk("System uptime: %d seconds\n", get_uptime());
        
    } else if (strcmp(args[0], "banner") == 0) {
        display_banner();
        
    } else if (strcmp(args[0], "alias") == 0) {
        if (argc == 1) {
            list_aliases();
        } else if (argc == 3) {
            add_alias(args[1], args[2]);
        } else {
            printk("Usage: alias [name] [command]\n");
        }
        
    } else if (strcmp(args[0], "set") == 0) {
        if (argc == 1) {
            printk("Shell settings:\n");
            printk("  debug: %s\n", shell_state.debug_mode ? "on" : "off");
            printk("  color: %s\n", shell_state.color_enabled ? "on" : "off");
            printk("  echo: %s\n", shell_state.echo_enabled ? "on" : "off");
        } else if (argc == 3) {
            if (strcmp(args[1], "debug") == 0) {
                shell_state.debug_mode = (strcmp(args[2], "on") == 0);
                print_success("Debug mode updated\n");
            } else if (strcmp(args[1], "color") == 0) {
                shell_state.color_enabled = (strcmp(args[2], "on") == 0);
                print_success("Color mode updated\n");
            } else if (strcmp(args[1], "echo") == 0) {
                shell_state.echo_enabled = (strcmp(args[2], "on") == 0);
                print_success("Echo mode updated\n");
            } else {
                print_error("Unknown setting\n");
            }
        } else {
            printk("Usage: set [setting] [on|off]\n");
        }
        
    } else if (strcmp(args[0], "history") == 0) {
        show_history();
        
    } else if (strcmp(args[0], "sysinfo") == 0) {
        show_system_info();
        
    } else if (strcmp(args[0], "calc") == 0) {
        calculator(argc, args);
        
    } else if (strcmp(args[0], "sound") == 0) {
        play_startup_sound();
        
    } else {
        // Check if it's a file in the current directory
        fs_node_t* node = vfs_finddir(fs_root, args[0]);
        if (node) {
            // TODO: Implement execution of binary files
            print_error("Execution of binary files is not yet implemented\n");
            shell_state.last_exit_code = 126; // Cannot execute
        } else {
            print_error("Command not found: ");
            printk("%s\n", args[0]);
            shell_state.last_exit_code = 127; // Command not found
        }
    }
    
    if (shell_state.debug_mode) {
        printk("[DEBUG] Command '%s' exited with code %d\n", 
               args[0], shell_state.last_exit_code);
    }
}

// Enhanced shell main function
int start_shell(void) {
    char input[MAX_INPUT];
    int position = 0;
    
    // Initialize shell
    display_banner();
    print_success("BasedOS Shell v2.0\n");
    print_info("Type 'help' for available commands\n");
    printk("\n");
    
    // Add some default aliases
    add_alias("ll", "ls -l");
    add_alias("la", "ls -a");
    add_alias("cls", "clear");
    add_alias("mem", "memory");
    add_alias("sys", "sysinfo");
    
    while (1) {
        // Enhanced prompt with directory and exit code
        if (shell_state.last_exit_code != 0) {
            print_error("[");
            printk("%d", shell_state.last_exit_code);
            print_error("] ");
        }
        
        if (shell_state.color_enabled) {
            print_success("user@based");
            print_info(":");
            print_warning(shell_state.current_directory);
            print_info("$ ");
        } else {
            printk(PROMPT);
        }
        
        position = 0;
        input[0] = '\0';
        
        while (1) {
            char c = keyboard_getchar();
            
            if (c == '\r') {
                printk("\n");
                input[position] = '\0';
                break;
                
            } else if (c == '\b' && position > 0) {
                position--;
                printk("\b \b");
                input[position] = '\0';
                
            } else if (c == 0x48 && history_pos > 0) { // Up arrow
                if (current_history == -1) {
                    current_history = history_pos - 1;
                } else if (current_history > 0) {
                    current_history--;
                }
                
                // Clear current input
                while (position > 0) {
                    position--;
                    printk("\b \b");
                }
                
                strcpy(input, history[current_history]);
                position = strlen(input);
                printk("%s", input);
                
            } else if (c == 0x50 && current_history != -1) { // Down arrow
                if (current_history < history_pos - 1) {
                    current_history++;
                    
                    // Clear current input
                    while (position > 0) {
                        position--;
                        printk("\b \b");
                    }
                    
                    strcpy(input, history[current_history]);
                    position = strlen(input);
                    printk("%s", input);
                } else {
                    current_history = -1;
                    // Clear current input
                    while (position > 0) {
                        position--;
                        printk("\b \b");
                    }
                    position = 0;
                    input[0] = '\0';
                }
                
            } else if (c == 0x09) { // Tab - simple completion
                if (position > 0) {
                    // Simple command completion
                    const char* commands[] = {
                        "help", "clear", "echo", "exit", "shutdown", "reboot",
                        "beep", "memory", "uptime", "date", "ls", "cat",
                        "mkdir", "touch", "rm", "write", "banner", "alias",
                        "set", "history", "sysinfo", "calc", "sound", NULL
                    };
                    
                    for (int i = 0; commands[i]; i++) {
                        if (strncmp(input, commands[i], position) == 0) {
                            // Complete the command
                            strcpy(input + position, commands[i] + position);
                            int new_pos = strlen(input);
                            while (position < new_pos) {
                                putchar(input[position]);
                                position++;
                            }
                            break;
                        }
                    }
                }
                
            } else if (position < MAX_INPUT - 2 && c >= 32 && c <= 126) {
                // Add character to input buffer
                input[position++] = c;
                input[position] = '\0';
                if (shell_state.echo_enabled) {
                    putchar(c);
                }
            } else if (position >= MAX_INPUT - 2) {
                // Buffer full, beep to alert user
                beep(1000);
                delay(100);
                nosound();
            }
        }
        
        execute_command(input);
        current_history = -1;
        
        // Check for exit command
        if (strcmp(input, "exit") == 0) {
            break;
        }
    }
    
    return shell_state.last_exit_code;
}