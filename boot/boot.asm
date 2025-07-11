[org 0x7C00]
[bits 16]

; Constants
KERNEL_OFFSET equ 0x1000        ; Where to load kernel in memory
KERNEL_SECTORS equ 64           ; Number of sectors to load (32KB kernel)

start:
    ; Clear interrupts and set up segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00                  ; Stack grows down from bootloader
    sti

    ; Clear screen and set video mode
    mov ax, 0x0003                  ; 80x25 text mode
    int 0x10

    ; Save boot drive
    mov [boot_drive], dl

    ; Print loading message
    mov si, loading_msg
    call print_string

    ; Load kernel from disk
    call load_kernel

    ; Print success message
    mov si, loaded_msg
    call print_string

    ; Switch to protected mode
    cli
    call enable_a20
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; Far jump to flush pipeline and enter protected mode
    jmp 0x08:protected_mode

;===============================================================================
; Load kernel from disk
;===============================================================================
load_kernel:
    pusha
    
    ; Set up disk read parameters
    mov bx, KERNEL_OFFSET           ; Destination buffer
    mov dh, 0                       ; Head 0
    mov dl, [boot_drive]            ; Drive number
    mov ch, 0                       ; Cylinder 0
    mov cl, 2                       ; Start from sector 2 (after boot sector)
    mov al, KERNEL_SECTORS          ; Number of sectors to read
    
    ; Read sectors
    mov ah, 0x02                    ; BIOS read function
    int 0x13                        ; Call BIOS
    
    jc disk_error                   ; Jump if carry flag set (error)
    
    ; Verify we read the right number of sectors
    cmp al, KERNEL_SECTORS
    jne disk_error
    
    popa
    ret

disk_error:
    mov si, disk_error_msg
    call print_string
    
    ; Print error code
    mov ah, 0x01                    ; Get last disk status
    int 0x13
    call print_hex_byte
    
    ; Hang system
    cli
    hlt

;===============================================================================
; Print string (SI = string address, null terminated)
;===============================================================================
print_string:
    pusha
    mov ah, 0x0E                    ; BIOS teletype function
.loop:
    lodsb                           ; Load byte from SI into AL
    cmp al, 0                       ; Check for null terminator
    je .done
    int 0x10                        ; Print character
    jmp .loop
.done:
    popa
    ret

;===============================================================================
; Print hex byte (AL = byte to print)
;===============================================================================
print_hex_byte:
    pusha
    mov ah, 0x0E
    
    ; Print high nibble
    mov bl, al
    shr bl, 4
    and bl, 0x0F
    add bl, '0'
    cmp bl, '9'
    jle .print_high
    add bl, 7                       ; Convert to A-F
.print_high:
    mov al, bl
    int 0x10
    
    ; Print low nibble
    mov bl, al
    and bl, 0x0F
    add bl, '0'
    cmp bl, '9'
    jle .print_low
    add bl, 7                       ; Convert to A-F
.print_low:
    mov al, bl
    int 0x10
    
    popa
    ret

;===============================================================================
; Enable A20 line (required for protected mode)
;===============================================================================
enable_a20:
    pusha
    
    ; Try fast A20 method first
    in al, 0x92
    test al, 2
    jnz .done
    or al, 2
    and al, 0xFE
    out 0x92, al
    
.done:
    popa
    ret

;===============================================================================
; Protected mode entry point
;===============================================================================
[bits 32]
protected_mode:
    ; Set up protected mode segments
    mov ax, 0x10                    ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000                ; Set up stack in protected mode
    
    ; Jump to kernel
    jmp KERNEL_OFFSET

;===============================================================================
; Global Descriptor Table
;===============================================================================
gdt_start:
    ; Null descriptor
    dd 0x0
    dd 0x0
    
    ; Code segment descriptor
    dw 0xFFFF                       ; Limit (bits 0-15)
    dw 0x0000                       ; Base (bits 0-15)
    db 0x00                         ; Base (bits 16-23)
    db 0x9A                         ; Access byte (present, ring 0, executable, readable)
    db 0xCF                         ; Flags and limit (bits 16-19)
    db 0x00                         ; Base (bits 24-31)
    
    ; Data segment descriptor
    dw 0xFFFF                       ; Limit (bits 0-15)
    dw 0x0000                       ; Base (bits 0-15)
    db 0x00                         ; Base (bits 16-23)
    db 0x92                         ; Access byte (present, ring 0, writable)
    db 0xCF                         ; Flags and limit (bits 16-19)
    db 0x00                         ; Base (bits 24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1      ; Size of GDT
    dd gdt_start                    ; Address of GDT

;===============================================================================
; Data section
;===============================================================================
loading_msg     db 'Loading BasedOS kernel...', 0x0D, 0x0A, 0
loaded_msg      db 'Kernel loaded successfully!', 0x0D, 0x0A, 0
disk_error_msg  db 'Disk read error: ', 0

boot_drive      db 0

;===============================================================================
; Boot sector padding and signature
;===============================================================================
times 510-($-$$) db 0               ; Pad to 510 bytes
dw 0xAA55                           ; Boot signature