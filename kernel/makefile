
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
DATA_STRUCT_DIR=./datastruct
FS=./filesystems
NET=./net
EXEC_DIR=./exec
SCHEDULAR_DIR=./scheduler

CSRC= kernel.c \
	$(GDT_DIR)/gdt.c $(IDT_DIR)/syscalls.c $(IDT_DIR)/idt.c $(IRQ_DIR)/irq.c $(DRIVERS_DIR)/pic.c $(SYS_DIR)/port_io.c $(SYS_DIR)/intr.c \
	$(MEM_DIR)/kheap.c $(MEM_DIR)/vmm.c $(MEM_DIR)/pmm.c \
	$(NET)/ethernet.c $(NET)/arp.c $(NET)/addr.c $(NET)/ip.c $(NET)/udp.c $(NET)/dhcp.c $(NET)/manager.c $(NET)/icmp.c $(NET)/tcp.c\
	$(EXEC_DIR)/readelf.c $(EXEC_DIR)/elf.c \
	$(SCHEDULAR_DIR)/scheduler.c \
	$(DRIVERS_DIR)/rtc.c $(DRIVERS_DIR)/vga.c $(DRIVERS_DIR)/keyboard.c  $(DRIVERS_DIR)/serial.c $(DRIVERS_DIR)/pit.c $(DRIVERS_DIR)/pci.c $(DRIVERS_DIR)/ide.c $(DRIVERS_DIR)/rtl8139.c\
	logging/logger.c \
	$(FS)/fs.c $(FS)/vfs.c $(FS)/iso9660.c\
  	$(CLIB_DIR)/stdio/scanf.c $(CLIB_DIR)/stdio/printf.c $(CLIB_DIR)/stdlib/itoa.c $(CLIB_DIR)/string/string.c $(CLIB_DIR)/ctype/ctype.c $(CLIB_DIR)/time.c\
	$(DATA_STRUCT_DIR)/list.c $(DATA_STRUCT_DIR)/tree.c $(DATA_STRUCT_DIR)/gtree.c $(DATA_STRUCT_DIR)/stack.c

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