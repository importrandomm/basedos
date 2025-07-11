[org 0x7C00]
[bits 16]

KERNEL_OFFSET equ 0x1000
KERNEL_SECTORS equ 32

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    mov [boot_drive], dl

    ; Load kernel
    mov bx, KERNEL_OFFSET
    mov dh, 0
    mov dl, [boot_drive]
    mov ch, 0
    mov cl, 2
    mov al, KERNEL_SECTORS
    mov ah, 0x02
    int 0x13
    jc disk_error

    ; Switch to protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp 0x08:protected_mode

disk_error:
    cli
    hlt

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000
    jmp KERNEL_OFFSET

gdt_start:
    dd 0x0, 0x0
    dw 0xFFFF, 0x0000
    db 0x00, 0x9A, 0xCF, 0x00
    dw 0xFFFF, 0x0000
    db 0x00, 0x92, 0xCF, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

boot_drive db 0

times 510-($-$$) db 0
dw 0xAA55