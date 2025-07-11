[bits 32]
section .text
extern kmain
global _start

_start:
 cld
 fninit
 cmp dword [kmain], 0
 je .error_kmain_missing
 mov edi, 0xB8000
 mov esi, entry_msg
 mov ah, 0x07
.loop_print:
 lodsb
 test al, al
 jz .done_print
 mov [edi], ax
 add edi, 2
 jmp .loop_print
.done_print:
 call kmain
 mov edi, 0xB8000 + 160
 mov esi, error_msg
 mov ah, 0x04
.error_loop:
 lodsb
 test al, al
 jz .halt
 mov [edi], ax
 add edi, 2
 jmp .error_loop
.error_kmain_missing:
 mov edi, 0xB8000
 mov esi, error_kmain_msg
 mov ah, 0x04
 jmp .error_loop
.halt:
 cli
 hlt
 jmp .halt

section .data
entry_msg db "Entered kernel...", 0
error_msg db "Error: kmain returned", 0
error_kmain_msg db "Error: kmain missing", 0