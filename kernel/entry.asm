[bits 32]
section .text
extern kmain
global _start

_start:
    ; Set up segments properly
    mov ax, 0x10        ; Data segment selector from bootloader GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000    ; Set up stack
    
    ; Clear direction flag
    cld
    
    ; Initialize FPU
    fninit
    
    ; Display entry message
    mov edi, 0xB8000
    mov esi, entry_msg
    mov ah, 0x07        ; Light gray on black
    
.loop_print:
    lodsb
    test al, al
    jz .done_print
    mov [edi], ax
    add edi, 2
    jmp .loop_print
    
.done_print:
    ; Call the main kernel function
    call kmain
    
    ; If kmain returns, display error and halt
    mov edi, 0xB8000 + 160  ; Second line
    mov esi, error_msg
    mov ah, 0x04            ; Red on black
    
.error_loop:
    lodsb
    test al, al
    jz .halt
    mov [edi], ax
    add edi, 2
    jmp .error_loop
    
.halt:
    cli
    hlt
    jmp .halt

section .data
entry_msg db "BasedOS kernel starting...", 0
error_msg db "ERROR: Kernel main returned!", 0