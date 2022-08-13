sudo apt-get install grub2-common grub-pc xorriso

export PATH="$PATH:$HOME/opt/cross/bin"
set -e

nasm -f elf32 boot.asm -o boot.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-ld -T linker.ld -o kernel.elf -O0 -nostdlib boot.o kernel.o
cp kernel.elf isodir/boot/kernel.elf
grub-mkrescue -o myos.iso isodir
qemu-system-i386 -boot d -drive file=myos.iso,media=cdrom,if=ide -s -S