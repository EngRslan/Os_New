
CFLAGS = -c -std=gnu99 -ffreestanding -Wno-unused-parameter -Wno-unused-variable -O0 -Wall -Wextra -g
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -o kernel.elf -O0 -nostdlib

CLIB_DIR=./clib
SYS_DIR=./sys
GDT_DIR=./sys/gdt
IDT_DIR=./sys/idt
IRQ_DIR=./sys/irq
MEM_DIR=./sys/memory
DRIVERS_DIR=./drivers

CSRC= kernel.c \
	$(GDT_DIR)/gdt.c $(IDT_DIR)/syscalls.c $(IDT_DIR)/idt.c $(IRQ_DIR)/irq.c $(DRIVERS_DIR)/pic.c $(SYS_DIR)/port_io.c \
	$(MEM_DIR)/kheap.c $(MEM_DIR)/vmm.c $(MEM_DIR)/pmm.c \
	$(DRIVERS_DIR)/vga.c $(DRIVERS_DIR)/keyboard.c  $(DRIVERS_DIR)/serial.c \
	logging/logger.c \
  	$(CLIB_DIR)/stdio/printf.c $(CLIB_DIR)/stdlib/itoa.c $(CLIB_DIR)/string/string.c $(CLIB_DIR)/ctype/ctype.c

SSRC= kernel_entry.asm $(IDT_DIR)/idt_exceptions.asm $(IRQ_DIR)/idt_irq.asm
OBJS= $(CSRC:.c=.o) $(SSRC:.asm=.o)

build: $(OBJS)
	$(LINKER) $(LDFLAGS) -o kernel.elf $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -I ./include -o $@ $<

%.o: %.asm
	$(AS) $(ASFLAGS) -o $@ $<

clean:
	rm -rf $(OBJS)
	rm -rf *.o
	rm -rf *.elf