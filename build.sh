sudo apt-get install grub2-common grub-pc xorriso

export PATH="$PATH:$HOME/opt/cross/bin"
set -e

nasm -f elf32 boot.asm -o boot.o
i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O0 -Wall -Wextra -g
i686-elf-ld -T linker.ld -o kernel.elf -O0 -nostdlib boot.o kernel.o
cp kernel.elf isodir/boot/kernel.elf
grub-mkrescue -o myos.iso isodir
qemu-system-i386 -boot d -drive file=myos.iso,media=cdrom,if=ide -s -S




sudo ip link set dev br0 up
sudo ip link set dev br0 down
sudo ip link delete tap0
sudo brctl delbr br0
ip link add name br0 type bridge nf_call_iptables 0
sudo iptables -P FORWARD ACCEPT
ip -d link show br0
brctl addif br0 tap0
ip addr add 192.168.0.1/24 dev br0
sudo ip tuntap add tap0 mode tap


https://wiki.qemu.org/Documentation/Networking/NAT
https://wiki.qemu.org/Documentation/Networking
https://www.rfc-editor.org/rfc/rfc2132