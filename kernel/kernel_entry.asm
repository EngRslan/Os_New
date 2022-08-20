[bits 32]
MULTIBOOT_PAGE_ALIGN	equ 1<<0
MULTIBOOT_MEMORY_INFO	equ 1<<1
MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM	    equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
HIGHER_HALF_KERNEL_ADDR equ 0xc0000000

section .multiboot.data alloc write 
align 4
dd MULTIBOOT_HEADER_MAGIC
dd MULTIBOOT_HEADER_FLAGS
dd MULTIBOOT_CHECKSUM

section .bootstrap_stack nobits alloc write 
stack_bottom:
resb 16384
stack_top:

section .bss nobits alloc write
align 4096
boot_page_directory:
    resb 4096
boot_page_table1:
    resb 4096

section .multiboot.text alloc
global _start 
extern _kernel_start
extern _kernel_end
_start:
    xchg bx,bx
    mov edi, boot_page_table1-HIGHER_HALF_KERNEL_ADDR
    mov esi,0
    mov ecx,1024

;;
;; Map First 4 MB TO 0xC0000000
;;
fill_page_table:
    mov edx, esi
    or dword edx , 0x003
    mov dword [edi] , edx

    add esi, 0x1000
    add edi,4
    loop fill_page_table
    

step3:
    mov dword [boot_page_directory - HIGHER_HALF_KERNEL_ADDR] , (boot_page_table1 - HIGHER_HALF_KERNEL_ADDR + 0x003)
	mov dword [boot_page_directory - HIGHER_HALF_KERNEL_ADDR + 768 * 4] , (boot_page_table1 - HIGHER_HALF_KERNEL_ADDR + 0x003)

    mov ecx , boot_page_directory - HIGHER_HALF_KERNEL_ADDR
	mov cr3 , ecx

    mov ecx , cr0 
	or ecx, 0x80010000
	mov cr0 , ecx

    lea  ecx, [hight_kernel_start]
	jmp ecx
    ;     jmp hight_kernel_start
section .text
extern  kernel_main
hight_kernel_start:
    ; mov dword [boot_page_directory + 0], 0 
    ; mov ecx , cr3
	; mov cr3, ecx

    mov     esp , stack_top
    add ebx, HIGHER_HALF_KERNEL_ADDR
    push   ebx
    push   eax
    call kernel_main
    cli
    hlt
halt:
    jmp halt




