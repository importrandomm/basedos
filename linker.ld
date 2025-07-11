ENTRY(_start)
OUTPUT_FORMAT(binary)
MEMORY {
 ram (rwx) : ORIGIN = 0x1000, LENGTH = 64K
}
SECTIONS {
 .text 0x1000 : ALIGN(4) { *(.text) } > ram
 .rodata : ALIGN(4) { *(.rodata*) } > ram
 .data : ALIGN(4) { *(.data) } > ram
 .bss : ALIGN(4) { *(COMMON) *(.bss) } > ram
 /DISCARD/ : { *(.comment) *(.eh_frame) }
}