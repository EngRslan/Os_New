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
#include <kernel/drivers/serial.h>
#include <kernel/drivers/vesa.h>
#include <kernel/types.h>
#include <logger.h>
#include <stdio.h>

void tick_handler(register_t * reg);
void keyboard_event(keyboard_event_t event);
void keyboard_event2(keyboard_event_t event);
void kernel_main (uint64_t magic, multiboot_info_t * mbi);
void kernel_main(uint64_t magic, multiboot_info_t * mbi) 
{
  
  logger_install(LOG_LEVEL_TRACE);
  string_t logo = "\
   _____ ______   _____  ____      \n\r \
 | ____/ ___\\ \\ / / _ \\/ ___|  \n\r \
 |  _|| |  _ \\ V / | | \\___ \\  \n\r \
 | |__| |_| | | || |_| |___) |    \n\r \
 |_____\\____| |_| \\___/|____/   FUTURE IS NOW\n\r \
                                  \n\r \
  ";
  serial_print(COM1,logo);
  
  log_information("OS Starting");

  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    log_fatal("Invalid magic number: 0x%x\n", (uint32_t) magic);
    return;
  }

  // printf("addr: 0x%x, size: 0x%x, num: 0x%x",mbi->u.elf_sec.addr,mbi->u.elf_sec.size,mbi->u.elf_sec.num);
  if(BITREAD(mbi->flags,12)){
    vga_install(mbi->framebuffer_addr,mbi->framebuffer_width,mbi->framebuffer_height,mbi->framebuffer_bpp);
  }
  printf(logo);
  gdt_install();
  log_information("Installing GDT .installed");
  

  idt_install();
  log_information("Installing IDT .installed");

  syscalls_install();
  // if(BITREAD(mbi->flags,11)){
  //   struct VbeInfoBlock * info_block = (struct VbeInfoBlock *)mbi->vbe_control_info;
  //   uint16_t * mode = (uint16_t *)((info_block->VideoModePtr[1]*0x10)+info_block->VideoModePtr[0]);
  //   string_t Oem = (string_t)((info_block->OemStringPtr[1]*0x10)+info_block->OemStringPtr[0]);
  //   while (*mode != 0xFFFF)
  //   {
  //     log_trace("mode: 0x%x",(uint32_t)*mode);
  //     mode++;
  //   }
  //   struct VbeModeInfoBlock * mdeblock = (struct VbeModeInfoBlock *)((mbi->vbe_interface_seg * 0x10)+mbi->vbe_interface_off);
  //   log_trace("VESA table available Version:%d, Total Memory:%d",(uint32_t)info_block->VbeVersion,(uint32_t)info_block->TotalMemory);
  // }
  if(BITREAD(mbi->flags,6)){
    pmm_install((multiboot_memory_map_t *)mbi->mmap_addr,mbi->mmap_length);
    log_information("Installing Memory Map .installed");
  }else{
    log_fatal("Cannot install memory FAILED");
    return;
  }
  
  vmm_install();
  log_information("Install Virtual Memory .installed");
  // printf("\nTest Virtual Memory .");
  // allocate_page(kernel_directory,0x800000,0,1);

  // uint32_t * mm = (uint32_t *)0x800000;
  // *mm = 0xFFFFFFFF;
  // free_page(kernel_directory,0x800000);
  // *mm = 0x0;

  kheap_install();
  log_information("Install Kernel Heap .installed");


  keyboard_install();
  log_information("Install Keyboard Driver .installed");


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
  // register_keyboard_event_handler(keyboard_event,KEY_UP_EVENT);
  // register_keyboard_event_handler(keyboard_event2,KEY_DOWN_EVENT);
  int s=0;
  char ss[50];
  char kk;
  float f = 155.0/29.0;
  scanf("your number please? %d yourname please %s your fav char %c",&s,&ss,&kk);
  printf("\nyou write [ %d ] and hex [ 0x%x ] and name is %s fav %c float %f",s,s,ss,kk,f);
  for (;;) { }
  

}


void tick_handler(register_t * reg){

}

void keyboard_event(keyboard_event_t event)
{
    // if(event.ascii)
    // putchar(event.ascii);
    log_trace("\n\rreleased char:%c, code : 0x%x , is_Shift: %d , is_ALT : %d, isCtrl: %d",(uint32_t)(event.ascii?event.ascii:' '),(uint32_t)event.key_code,(uint32_t)event.shift,(uint32_t)event.alt,(uint32_t)event.ctrl);
}

void keyboard_event2(keyboard_event_t event)
{
  if(event.ascii)
    putchar(event.ascii);
    //printf("\n\rpressed code : 0x%x , is_Shift: %d , is_ALT : %d, isCtrl: %d",(uint32_t)event.key_code,(uint32_t)event.shift,(uint32_t)event.alt,(uint32_t)event.ctrl);
}