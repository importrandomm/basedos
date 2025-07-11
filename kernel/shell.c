#include "basedos.h"

#define MAX_INPUT 128
#define PROMPT "user@based:~$ "
#define HISTORY_SIZE 10

static char history[HISTORY_SIZE][MAX_INPUT];
static int history_pos = 0;
static int current_history = -1;

void delay(int milliseconds) {
    for (int i = 0; i < milliseconds * 1000; i++) {
        asm volatile("nop");
    }
}

static void add_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;
    
    if (history_pos > 0 && strcmp(history[history_pos - 1], cmd) == 0) {
        return;
    }
    
    strcpy(history[history_pos], cmd);
    history_pos = (history_pos + 1) % HISTORY_SIZE;
}

static void show_history(void) {
    printk("Command history:\n");
    for (int i = 0; i < HISTORY_SIZE; i++) {
        int idx = (history_pos + i) % HISTORY_SIZE;
        if (history[idx][0] != '\0') {
            printk(" %s\n", history[idx]);
        }
    }
}

static void execute_command(const char* input) {
    add_to_history(input);
    
    if (strcmp(input, "help") == 0) {
        printk("Available commands:\n");
        printk(" help - Show this help\n");
        printk(" clear - Clear screen\n");
        printk(" about - System info\n");
        printk(" beep - Test PC speaker\n");
        printk(" history - Show command history\n");
    } else if (strcmp(input, "clear") == 0) {
        terminal_clear();
    } else if (strcmp(input, "about") == 0) {
        printk("BasedOS v0.1 - 32-bit UNIX-like OS\n");
    } else if (strcmp(input, "beep") == 0) {
        beep(1000);
        delay(500);
        nosound();
    } else if (strcmp(input, "history") == 0) {
        show_history();
    } else if (input[0] != '\0') {
        printk("Command not found: %s\n", input);
    }
}

int start_shell(void) {
    char input[MAX_INPUT];
    int position = 0;
    
    printk("\nBasedOS Shell\n");
    printk("------------\n");
    
    while (1) {
        printk(PROMPT);
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
            } else if (c == 0x48 && history_pos > 0) {
                if (current_history == -1) {
                    current_history = history_pos - 1;
                } else if (current_history > 0) {
                    current_history--;
                }
                
                while (position > 0) {
                    position--;
                    printk("\b \b");
                }
                
                strcpy(input, history[current_history]);
                position = strlen(input);
                printk("%s", input);
            } else if (position < MAX_INPUT - 1 && c >= 32 && c <= 126) {
                input[position++] = c;
                input[position] = '\0';
                putchar(c);
            }
        }
        
        execute_command(input);
        current_history = -1;
    }
    
    return 1;
}