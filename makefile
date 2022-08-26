PREFIX=$(HOME)/opt/cross/bin
export PATH= $(shell printenv PATH):$(PREFIX)
export CC=i686-elf-gcc
export LINKER=i686-elf-ld
export AS = nasm


KERNEL_DIR = ./kernel
ISO_DIR = ./isodir
ISO_NAME = CloudOS.iso

build: kernel
	cp $(KERNEL_DIR)/kernel.elf $(ISO_DIR)/boot/kernel.elf
	grub-mkrescue -o CloudOS.iso $(ISO_DIR)

kernel:
	$(MAKE) --directory=$(KERNEL_DIR) build

run:
	qemu-system-i386 -boot d -drive file=$(ISO_NAME),media=cdrom,if=ide -s -S -serial stdio
bochs: build
	bochs -q
.PHONY: kernel clean
clean:
	$(MAKE) --directory=$(KERNEL_DIR) clean
	rm -rf $(ISO_DIR)/boot/kernel.elf
	rm -rf CloudOS.iso