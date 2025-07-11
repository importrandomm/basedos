[org 0x7C00]
[bits 16]

KERNEL_OFFSET equ 0x1000
MAX_SECTORS equ 128
SECTORS_PER_TRACK equ 18

start:
 cli
 xor ax, ax
 mov ds, ax
 mov es, ax
 mov ss, ax
 mov sp, 0x7C00
 sti

 mov ax, 0x0003
 int 0x10

 mov si, loading_msg
 call print

 mov bx, KERNEL_OFFSET
 mov dl, [boot_drive]
 mov dh, 0
 mov ch, 0
 mov cl, 2

.load_kernel:
 mov al, SECTORS_PER_TRACK
 cmp [sectors_remaining], al
 jge .read_sectors
 mov al, [sectors_remaining]

.read_sectors:
 mov ah, 0x02
 int 0x13
 jnc .read_success

 mov si, error_msg
 call print
 mov dx, ax
 call print_hex
 jmp halt

.read_success:
 sub [sectors_remaining], al
 jz .done_loading

 add cl, al
 cmp cl, SECTORS_PER_TRACK + 1
 jle .next_sector
 mov cl, 1
 xor dh, 1 
 jnz .next_sector
 inc ch

.next_sector:
 movzx dx, al
 shl dx, 9
 add bx, dx
 jmp .load_kernel

.done_loading:
 mov si, done_msg
 call print

 cli
 mov al, 0xD1
 out 0x64, al
 mov al, 0xDF
 out 0x60, al

 lgdt [gdt_descriptor]
 mov eax, cr0
 or eax, 1
 mov cr0, eax
 jmp 0x08:protected_mode_entry

print:
 lodsb
 or al, al
 jz .done
 mov ah, 0x0E
 int 0x10
 jmp print
.done:
 ret

print_hex:
 mov cx, 4
.hex_loop:
 rol dx, 4
 mov al, dl
 and al, 0xF
 add al, '0'
 cmp al, '9'
 jbe .print_char
 add al, 7
.print_char:
 mov ah, 0x0E
 int 0x10
 loop .hex_loop
 ret

halt:
 cli
 hlt

[bits 32]
protected_mode_entry:
 mov ax, 0x10
 mov ds, ax
 mov es, ax
 mov ss, ax
 mov esp, 0x90000
 jmp KERNEL_OFFSET

gdt_start:
 dq 0x0
 dw 0xFFFF
 dw 0x0000
 db 0x00
 db 0x9A
 db 0xCF
 db 0x00
 dw 0xFFFF
 dw 0x0000
 db 0x00
 db 0x92
 db 0xCF
 db 0x00
gdt_end:

gdt_descriptor:
 dw gdt_end - gdt_start - 1
 dd gdt_start

section .data
loading_msg db "Loading kernel...", 0
done_msg db "OK", 0x0D, 0x0A, 0
error_msg db "Disk error: ", 0
sectors_remaining dw MAX_SECTORS
boot_drive db 0

times 510-($-$$) db 0
dw 0xAA55