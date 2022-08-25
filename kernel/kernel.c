#include <kernel/multiboot.h>
#include <kernel/bits.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/syscalls.h>
#include <kernel/mem/pmm.h>
#include <kernel/mem/vmm.h>
#include <kernel/mem/kheap.h>
#include <kernel/drivers/vga.h>
#include <kernel/drivers/keyboard.h>
#include <stdio.h>

void tick_handler(register_t * reg);
void kbd_handler(register_t * reg);
void kernel_main (uint64_t magic, multiboot_info_t * mbi);
void kernel_main(uint64_t magic, multiboot_info_t * mbi) 
{

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    printf ("Invalid magic number: 0x%x\n", (uint32_t) magic);
    return;
  }

  // printf("addr: 0x%x, size: 0x%x, num: 0x%x",mbi->u.elf_sec.addr,mbi->u.elf_sec.size,mbi->u.elf_sec.num);
  if(BITREAD(mbi->flags,12)){
    vga_install(mbi->framebuffer_addr,mbi->framebuffer_width,mbi->framebuffer_height,mbi->framebuffer_bpp);
  }
  printf("Installing GDT .");
  gdt_install();
  printf("installed");

  printf("\n\rInstalling IDT .");
  idt_install();
  syscalls_install();
  printf("installed");
  printf("\n\rInstalling Memory Map .");
  if(BITREAD(mbi->flags,6)){
    pmm_install((multiboot_memory_map_t *)mbi->mmap_addr,mbi->mmap_length);
    printf("installed");
  }else{
    printf("Cannot install memory FAILED");
  }
  
  printf("\n\rInstall Virtual Memory .");
  vmm_install();
  printf("Installed");

  // printf("\nTest Virtual Memory .");
  // allocate_page(kernel_directory,0x800000,0,1);

  // uint32_t * mm = (uint32_t *)0x800000;
  // *mm = 0xFFFFFFFF;
  // free_page(kernel_directory,0x800000);
  // *mm = 0x0;

  printf("\n\rInstall Kernel Heap ");
  kheap_install();
  printf("installed");

  printf("\n\rInstall Keyboard Driver ");
  keyboard_install();
  printf("installed");

  register_interrupt_handler(0x20,tick_handler);

  // ptr_t all = kalloc(sizeof(int));
  // ptr_t all2 = kalloc(sizeof(int));
  // ptr_t all3 = kalloc(sizeof(int));
  // ptr_t all4 = kalloc(sizeof(int));
  // kfree(all2);
  // kfree(all4);
  // kfree(all3);
  // ptr_t all5 = kalloc(sizeof(int));
  // ptr_t all6 = kalloc(0x1200);

  int32_t a;
  __asm__ __volatile__("int $0x80":"=a"(a):"a"(0),"b"(0x120),"d"(0x160));
  
  printf("\n\rOS successfully Installed");
  printf("\n\rcmd > ");
  for (;;) { }
  

}


void tick_handler(register_t * reg){

}

char kbdus[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
    '9', '0', '-', '=', '\b',   /* Backspace */
    '\t',           /* Tab */
    'q', 'w', 'e', 'r', /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',       /* Enter key */
    0,          /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',   /* 39 */
    '\'', '`',   0,     /* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
    'm', ',', '.', '/',   0,                    /* Right shift */
    '*',
    0,  /* Alt */
    ' ',    /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
    '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
    '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

void kbd_handler(register_t * reg){
  int32_t i, scancode;
  for(i = 1000; i > 0; i++) {
        // Check if scan code is ready
        if((inportb(0x64) & 1) == 0) continue;
        // Get the scan code
        scancode = inportb(0x60);
        break;
    }
    if(i > 0) {
        if(scancode & 0x80) {
            // Key release
        }
        else {
            // Key down
            printf("Key pressed %c\n\r", kbdus[scancode]);
        }
    }
}