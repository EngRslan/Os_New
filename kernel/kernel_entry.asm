[bits 32]
MULTIBOOT_PAGE_ALIGN	equ 1<<0
MULTIBOOT_MEMORY_INFO	equ 1<<1
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM	    equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

section .multiboot.data alloc write 
align 4
dd MULTIBOOT_HEADER_MAGIC
dd MULTIBOOT_HEADER_FLAGS
dd MULTIBOOT_CHECKSUM

section .bootstrap_stack nobits alloc write 
align 16
stack_bottom:
resb 16384
stack_top:

section .bss nobits alloc write
align 4096
boot_page_directory:
    resb 4096
boot_page_table1:
    resb 4096

section .low_kernel_text alloc exec
global _start
extern  kernel_main
extern _start_kernel 
_start:
    mov edi, boot_page_table1 - 0xC0000000
    mov esi, 0
    mov ecx,1023

    cmp esi, _start_kernel



section .text
global _start
extern  kernel_main
hight_kernel_start:
    mov esp , stack_top
    push   ebx
    push   eax
    call kernel_main
    cli
    hlt
    jmp 1b



