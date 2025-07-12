ASM = nasm
CC = gcc
LD = ld
QEMU = qemu-system-i386
CFLAGS = -ffreestanding -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -Wextra -std=gnu99 -m32 -I. -I./include -I./include/sys -I./include/fs -I./kernel -fno-pie -fno-pic
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary
OBJECTS = \
	kernel/entry.o \
	kernel/terminal.o \
	kernel/interrupts.o \
	kernel/sound.o \
	kernel/shell.o \
	kernel/kernel.o \
	kernel/memory.o \
	fs/vfs.o \
	fs/memfs.o \
	fs/fs_test.o \
	lib/stdio.o \
	lib/string.o

all: basedos.img

basedos.img: boot/boot.bin kernel.bin
	dd if=/dev/zero of=basedos.img bs=512 count=2880
	dd if=boot/boot.bin of=basedos.img conv=notrunc
	dd if=kernel.bin of=basedos.img seek=1 conv=notrunc

boot/boot.bin: boot/boot.asm
	$(ASM) -f bin $< -o $@

kernel.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.asm
	$(ASM) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) kernel.bin basedos.img boot/boot.bin

run: basedos.img
	$(QEMU) -drive file=basedos.img,format=raw,if=floppy -vga std -display gtk

.PHONY: all clean run