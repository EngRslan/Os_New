#include <kernel/multiboot.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/syscalls.h>>
#include <stdio.h>

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
void tick_handler(register_t * reg);
void kbd_handler(register_t * reg);
void kernel_main (unsigned long magic, multiboot_info_t *mbi);

void kernel_main(unsigned long magic, multiboot_info_t *mbi) 
{

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
    return;
  }

  
  printf("Installing GDT .");
  gdt_install();
  printf("installed \n");

  printf("Installing IDT .");
  idt_install();
  syscalls_install();
  printf("installed \n");

  register_interrupt_handler(0x20,tick_handler);
  register_interrupt_handler(0x21,kbd_handler);

  int a;
  __asm__ __volatile__("int $0x80":"=a"(a):"a"(0),"b"(0x120),"d"(0x160));
  printf("Installing Memory Map .");

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
  int i, scancode;
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
            printf("Key pressed %c\n", kbdus[scancode]);
        }
    }
}