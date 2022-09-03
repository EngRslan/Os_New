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
#include <kernel/drivers/pit.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/ide.h>
#include <kernel/types.h>
#include <kernel/datastruct/list.h>
#include <kernel/datastruct/gtree.h>
#include <kernel/filesystems/vfs.h>
#include <kernel/system.h>
#include <logger.h>
#include <stdio.h>

// void print_tree(gtree_node_t * node);
// int find_by_value(gtree_node_t * s);
// void print_hierarchy(gtree_node_t * s,int depth);

uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
 
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
 
    // Write out the address
    outportl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = inportl(0xCFC);
    return tmp;
}

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
  

  kheap_install();
  log_information("Install Kernel Heap .installed");


  keyboard_install();
  log_information("Install Keyboard Driver .installed");

  pit_install();
  log_information("Install Programable Interval Timer (PIT) .installed");
  
  vfs_install();
  log_information("Install Virtual File System (VFS) .installed");

  log_information("Install PCI.");
  pci_install();
  log_information("PCI Installed Successfully");

  log_information("Installing IDE Controller");
  ide_install();
  
  uint32_t ss = pciConfigReadWord(0,1,1,PCI_BAR4);
  // list_t * list = list_create();
  // pci_scan_list(list);
  // foreach(item,list){
  //   pci_config_t * config = (pci_config_t *)item->value_ptr;
    
    
  //   switch (config->header.type)
  //   {
  //     case PCI_HEADER_DEVICE:
  //     {

  //       pci_config_space_0_t * cfg0 = (pci_config_space_0_t *)config->config;
  //       log_trace("Device Found (bus:%d, slot:%d, function: %d) . Vendor:0x%x, id:0x%x, class:0x%x, subclass:0x%x",
  //         config->bus,config->device,config->function,config->config->vendor_id,config->config->device_id,config->config->class_code,config->config->sub_class);
  //       break;
  //     }
  //     case PCI_HEADER_BRIDGE:
  //     {

  //       pci_config_space_1_t * cfg1 = (pci_config_space_1_t *)config->config;
  //       log_trace("Bridge Found (bus:%d, slot:%d, function: %d) . Vendor:0x%x, id:0x%x, class:0x%x, subclass:0x%x",
  //         config->bus,config->device,config->function,config->config->vendor_id,config->config->device_id,config->config->class_code,config->config->sub_class);
  //       break;
  //     }
  //     case PCI_HEADER_CARDBUS:
  //     {

  //       pci_config_space_2_t * cfg2 = (pci_config_space_2_t *)config->config;
  //       log_trace("Cardbus Found (bus:%d, slot:%d, function: %d) . Vendor:0x%x, id:0x%x, class:0x%x, subclass:0x%x",
  //           config->bus,config->device,config->function,config->config->vendor_id,config->config->device_id,config->config->class_code,config->config->sub_class);
  //     }
  //       break;
  //   }

  // }
  
  // pci_header_type_t h = pci_read_header_type(1,22,6);

  // pci_header_type_t pci_ht = pci_read_header_type(0,0,0);
  // size_t ss0 = sizeof(struct pci_config_space_0);
  // size_t ss1 = sizeof(struct pci_config_space_1);
  // size_t ss2 = sizeof(struct pci_config_space_2);

  // pci_command_t cmd = {.enable=1, .bus = 0,.device=0,.function=0,.offset=0};
  // size_t s = sizeof(pci_command_t);
  // uint32_t va = cmd.bits;
  // uint32_t val = pci_read(cmd);
  // list_t * pci_list = list_create();
  // struct pci_heder * h = (struct pci_heder *)kmalloc(sizeof(struct pci_heder));
  // for(uint16_t i = 0;i<256;i++){
  //   for(uint16_t j = 0;j<32;j++){
  //     read_pci((intptr_t *)h,i,j,0);
  //     if(h->vendor_id != 0xFFFF){
  //       list_add_item(pci_list,(ptr_t *)h);
  //       if(h->header_type & 0x80){
  //         h = (struct pci_heder *)kmalloc(sizeof(struct pci_heder));
  //         for (uint32_t s = 1; s < 8; s++)
  //         {
  //           read_pci((intptr_t *)h,i,j,s);
  //           if(h->vendor_id != 0xFFFF){
  //             list_add_item(pci_list,(ptr_t *)h);
  //             h = (struct pci_heder *)kmalloc(sizeof(struct pci_heder));
  //           }
  //         }
          
  //       }
  //       h = (struct pci_heder *)kmalloc(sizeof(struct pci_heder));
  //     }
  //   }
  // }
  // uint32_t device_id = read_pci(0,0,0,2);
  // uint32_t command = read_pci(0,0,0,4);
  // uint32_t status = read_pci(0,0,0,6);

  for (;;) { }
  

}

  // printf("\nTest Virtual Memory .");
  // allocate_page(kernel_directory,0x800000,0,1);

  // uint32_t * mm = (uint32_t *)0x800000;
  // *mm = 0xFFFFFFFF;
  // free_page(kernel_directory,0x800000);
  // *mm = 0x0;

  //register_interrupt_handler(0x20,tick_handler);

  // ptr_t all = kalloc(sizeof(int));
  // ptr_t all2 = kalloc(sizeof(int));
  // ptr_t all3 = kalloc(sizeof(int));
  // ptr_t all4 = kalloc(sizeof(int));
  // kfree(all2);
  // kfree(all4);
  // kfree(all3);
  // ptr_t all5 = kalloc(sizeof(int));
  // ptr_t all6 = kalloc(0x1200);

  // int32_t a;
  // __asm__ __volatile__("int $0x80":"=a"(a):"a"(0),"b"(0x120),"d"(0x160));
  
  // printf("\n\rOS successfully Installed");
  // printf("\n\rcmd > ");
  // register_keyboard_event_handler(keyboard_event,KEY_UP_EVENT);
  // register_keyboard_event_handler(keyboard_event2,KEY_DOWN_EVENT);
  // int s=0;
  // char ss[50];
  // char kk;
  // float f = 155.0/29.0;
  // scanf("your number please? %d yourname please %s your fav char %c",&s,&ss,&kk);
  // printf("\nyou write [ %d ] and hex [ 0x%x ] and name is %s fav %c float %f",s,s,ss,kk,f);
  // int s1 = 10;
  // int s2 = 20;
  // int s3 = 30;

  // list_t * list = list_create();
  // list_add_item(list,(ptr_t)&s1);
  // list_add_item(list,(ptr_t)&s2);
  // list_add_item(list,(ptr_t)&s3);

  // foreach(item,list){
  //   printf("\nitem value %d",*(intptr_t)item->value_ptr);
  // }
  
  // list_destroy(list);

  // gtree_t * tree = gtree_create(1000);
  // gtree_node_t * node10 = gtree_create_node(tree,NULL,0xa);
  //   gtree_node_t * node11 = gtree_create_node(tree,node10,0xb);
  //   gtree_node_t * node12 = gtree_create_node(tree,node10,0xc);
  //   gtree_node_t * node13 = gtree_create_node(tree,node10,0xd);
  // gtree_node_t * node20 = gtree_create_node(tree,NULL,0x14);
  //   gtree_node_t * node21 = gtree_create_node(tree,node20,0x15);
  //   gtree_node_t * node22 = gtree_create_node(tree,node20,0x16);
  //   gtree_node_t * node23 = gtree_create_node(tree,node20,0x17);
  // gtree_node_t * node30 = gtree_create_node(tree,NULL,0x1E);
  //   gtree_node_t * node31 = gtree_create_node(tree,node30,0x1F);
  //   gtree_node_t * node32 = gtree_create_node(tree,node30,0x20);
  //   gtree_node_t * node33 = gtree_create_node(tree,node30,21);
  // gtree_node_t * node40 = gtree_create_node(tree,NULL,0x28);
  //   gtree_node_t * node41 = gtree_create_node(tree,node40,0x29);
  //   gtree_node_t * node42 = gtree_create_node(tree,node40,0x2A);
  //   gtree_node_t * node43 = gtree_create_node(tree,node40,0x2B);
  // gtree_node_t * node50 = gtree_create_node(tree,NULL,0x32);
  //   gtree_node_t * node51 = gtree_create_node(tree,node50,0x33);
  //   gtree_node_t * node52 = gtree_create_node(tree,node50,0x34);
  //   gtree_node_t * node53 = gtree_create_node(tree,node50,0x35);
// for (uint32_t i = 100; i > 0; i-=10)
// {
//     gtree_node_t * upnode = gtree_create_child(tree->root,i);
//     for (uint32_t j = 9; j >= 1; j--)
//     {
//         gtree_node_t * upnodess = gtree_create_child(upnode,(i-j));
//         for(uint32_t s = 5; s >= 1; s--){
//           gtree_create_child(upnodess,s);
//         }
//     }
    
// }

//  gtree_node_t * upnode02 = gtree_descendant_query(tree->root,find_by_value);
//   //  print_tree(tree->root);
  //  gtree_remove_descendant(tree ,tree->root->first_child);
  //  gtree_remove_descendant(tree ,tree->root->next_subling);
  //  gtree_remove_descendant(tree ,tree->root->next_subling->next_subling->next_subling->next_subling);
//  if(upnode02)
//     gtree_descendant_exec(upnode02,print_hierarchy,0);
    // gtree_descendant_exec(tree->root,print_hierarchy,0);

// int spaces = 0;
// char tabs[50];
// void print_hierarchy(gtree_node_t * node,int depth){
//   if(depth == 0){
//     tabs[0] = 0;
//   }
//   for (int i = 0; i < depth; i++)
//   {
//     tabs[i]='\t';
//     tabs[i+1]=0x0;
//   }
//   log_trace("%s--| %d",tabs, node->value);
// }

// int find_by_value(gtree_node_t * s){
//   if(s->value == 10){
//     return 1;
//   }
  
//   return 0;
// }