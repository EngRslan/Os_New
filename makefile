PREFIX=$(HOME)/opt/cross/bin
export PATH= $(shell printenv PATH):$(PREFIX)
export CC=i686-elf-gcc
export LINKER=i686-elf-ld
export AS = nasm


KERNEL_DIR = ./kernel
KERNELMODS_DIR = ./kernelmods
ISO_DIR = ./isodir
ISO_NAME = CloudOS.iso

build: kernel kernelmods
	cp $(KERNEL_DIR)/kernel.elf $(ISO_DIR)/boot/kernel.elf
	cp $(KERNELMODS_DIR)/*.so $(ISO_DIR)/boot/
	grub-mkrescue -o CloudOS.iso $(ISO_DIR)

kernel:
	$(MAKE) --directory=$(KERNEL_DIR) build

kernelmods:
	$(MAKE) --directory=$(KERNELMODS_DIR) build

run:
	qemu-system-i386 -boot d -s -S -serial stdio -vga std -nodefaults \
	-drive file=$(ISO_NAME),media=cdrom,if=ide \
	-drive file=hdd.img,media=disk,if=ide,format=raw \
	-netdev user,id=net0 \
	-device rtl8139,netdev=net0,id=nic0,mac=52:54:00:6a:40:01 \
	-object filter-dump,id=net0,netdev=net0,file=qdump \
	-monitor telnet:127.0.0.1:55555,server,nowait;

runs:
	sudo qemu-system-i386 -boot d -s -S -serial stdio -vga std -nodefaults \
	-drive file=$(ISO_NAME),media=cdrom,if=ide \
	-drive file=hdd.img,media=disk,if=ide,format=raw \
	-netdev tap,id=net0,helper=/usr/lib/qemu/qemu-bridge-helper \
	-device rtl8139,netdev=net0,id=nic0,mac=52:54:00:6a:40:01 \
	-object filter-dump,id=net0,netdev=net0,file=qdump \
	-monitor telnet:127.0.0.1:55555,server,nowait;
	
bochs: build
	bochs -q
hdd:
	dd if=/dev/zero of=hdd.img count=1440 bs=1024
.PHONY: modules kernel clean
clean:
	$(MAKE) --directory=$(KERNEL_DIR) clean
	$(MAKE) --directory=$(KERNELMODS_DIR) clean
	rm -rf $(ISO_DIR)/boot/kernel.elf
	rm -rf $(ISO_DIR)/boot/*.so
	rm -rf CloudOS.iso